<#
.SYNOPSIS
    Installs or uninstalls Tithify (Nepali Date Taskbar Widget) for Windows via the command line.

.DESCRIPTION
    Downloads the latest release of Tithify from GitHub and performs
    a clean, silent installation into the user's Local AppData directory.

.EXAMPLE
    # Install via PowerShell one-liner:
    irm https://raw.githubusercontent.com/aayushlbef/Tithify/main/install.ps1 | iex

.EXAMPLE
    # Uninstall:
    powershell -ExecutionPolicy Bypass -File .\install.ps1 -Uninstall
#>

[CmdletBinding()]
param(
    [switch]$Uninstall,
    [string]$Version = "latest"
)

$ErrorActionPreference = "Stop"

# Configuration
$Repo = "aayushlbef/Tithify"
$AppName = "Tithify"
$ExeName = "Tithify.exe"
$InstallDir = Join-Path $env:LOCALAPPDATA $AppName
$TargetExe = Join-Path $InstallDir $ExeName
$Uninstaller = Join-Path $InstallDir "unins000.exe"

function Write-BrandHeader {
    Write-Host ""
    Write-Host "  Tithify - Nepali Date Windows Taskbar Widget" -ForegroundColor Cyan
    Write-Host "  ============================================" -ForegroundColor DarkGray
    Write-Host ""
}

# ---------------------------------------------------------------------------
# Spinner — animates on the current line while $Action runs.
# Uses ThreadJob when available (PS 7+); falls back to a simple dot-ticker
# on Windows PowerShell 5.1.
# ---------------------------------------------------------------------------
function Invoke-WithSpinner {
    param(
        [string]       $Message,
        [ScriptBlock]  $Action,
        [ConsoleColor] $Color = [ConsoleColor]::Cyan
    )

    $spinChars = [char[]]@(0x2838, 0x2830, 0x2810, 0x2800,
                            0x2801, 0x2803, 0x2807, 0x280F,
                            0x281F, 0x283F, 0x287F, 0x28FF,
                            0x28F7, 0x28E3, 0x28C1, 0x2880)
    $spinIndex = 0

    try { [Console]::CursorVisible = $false } catch {}

    $hasThreadJob = (Get-Command Start-ThreadJob -ErrorAction SilentlyContinue) -ne $null
    $result = $null
    $err    = $null

    if ($hasThreadJob) {
        $job = Start-ThreadJob -ScriptBlock $Action

        while ($job.State -eq 'Running') {
            $spin = $spinChars[$spinIndex % $spinChars.Count]
            $line = "  $spin  $Message"
            Write-Host "`r$line" -NoNewline -ForegroundColor $Color
            $spinIndex++
            Start-Sleep -Milliseconds 60
        }

        # Erase spinner line
        $pad = " " * ("  $Message  ".Length + 4)
        Write-Host "`r$pad`r" -NoNewline

        if ($job.State -eq 'Failed') {
            $err = $job.ChildJobs[0].Error[0]
        } else {
            $result = Receive-Job -Job $job -ErrorAction SilentlyContinue
        }
        Remove-Job -Job $job -Force -ErrorAction SilentlyContinue

    } else {
        # PS 5.1 fallback: dot ticker in the same process
        Write-Host "  ...  $Message" -NoNewline -ForegroundColor $Color
        try   { $result = & $Action }
        catch { $err = $_ }
        $pad = " " * ("  ...  $Message".Length + 2)
        Write-Host "`r$pad`r" -NoNewline
    }

    try { [Console]::CursorVisible = $true } catch {}

    if ($err) { throw $err }
    return $result
}

function Write-Step {
    param([string]$Text, [ConsoleColor]$Color = [ConsoleColor]::Cyan)
    Write-Host "  ==>  $Text" -ForegroundColor $Color
}
function Write-OK   { param([string]$Text) Write-Host "  [+]  $Text" -ForegroundColor Green  }
function Write-Warn { param([string]$Text) Write-Host "  [!]  $Text" -ForegroundColor Yellow }
function Write-Fail { param([string]$Text) Write-Host "  [-]  $Text" -ForegroundColor Red    }

# --- UNINSTALL FLOW --------------------------------------------------------
if ($Uninstall) {
    Write-BrandHeader
    Write-Step "Uninstalling $AppName..." Yellow

    Get-Process -Name "Tithify","NepaliDateWidget" -ErrorAction SilentlyContinue |
        Stop-Process -Force -ErrorAction SilentlyContinue
    Start-Sleep -Milliseconds 500

    if (Test-Path $Uninstaller) {
        Invoke-WithSpinner -Message "Running uninstaller..." -Color Gray -Action {
            Start-Process -FilePath $using:Uninstaller `
                          -ArgumentList "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART" -Wait
        }
        Write-OK "$AppName uninstalled successfully."
    } elseif (Test-Path $InstallDir) {
        Invoke-WithSpinner -Message "Removing files..." -Color Gray -Action {
            Remove-Item -Path $using:InstallDir -Recurse -Force -ErrorAction SilentlyContinue
        }
        Write-OK "Removed: $InstallDir"
    } else {
        Write-Warn "$AppName does not appear to be installed."
    }
    Write-Host ""
    return
}

# --- INSTALL FLOW ----------------------------------------------------------
try {
    Write-BrandHeader

    [Net.ServicePointManager]::SecurityProtocol =
        [Net.SecurityProtocolType]::Tls12 -bor [Net.SecurityProtocolType]::Tls13

    # Close any running instance
    $running = Get-Process -Name "Tithify","NepaliDateWidget" -ErrorAction SilentlyContinue
    if ($running) {
        Write-Step "Closing active $AppName instance..." Yellow
        $running | Stop-Process -Force -ErrorAction SilentlyContinue
        Start-Sleep -Milliseconds 600
        Write-OK "Closed."
    }

    # Build download URL
    if ($Version -eq "latest") {
        $DownloadUrl = "https://github.com/$Repo/releases/latest/download/Tithify_Setup.exe"
    } else {
        $Tag = if ($Version.StartsWith("v")) { $Version } else { "v$Version" }
        $DownloadUrl = "https://github.com/$Repo/releases/download/$Tag/Tithify_Setup.exe"
    }

    $RandomId      = [System.Guid]::NewGuid().ToString('N')
    $TempInstaller = Join-Path $env:TEMP "Tithify_Setup_$RandomId.exe"

    Write-Host ""
    Write-Host "  From : " -NoNewline -ForegroundColor DarkGray
    Write-Host $DownloadUrl -ForegroundColor DarkCyan
    Write-Host "  To   : " -NoNewline -ForegroundColor DarkGray
    Write-Host $InstallDir  -ForegroundColor DarkCyan
    Write-Host ""

    # ── Step 1 · Download ──────────────────────────────────────────────
    Invoke-WithSpinner -Message "Downloading Tithify_Setup.exe..." -Action {
        $ProgressPreference = 'SilentlyContinue'
        Invoke-WebRequest -Uri $using:DownloadUrl -OutFile $using:TempInstaller -UseBasicParsing
    }

    if (-not (Test-Path $TempInstaller) -or ((Get-Item $TempInstaller).Length -lt 100000)) {
        throw "Download failed or file is corrupted."
    }

    $sizeMB = [math]::Round((Get-Item $TempInstaller).Length / 1MB, 2)
    Write-OK "Downloaded  ($sizeMB MB)"

    Unblock-File -Path $TempInstaller -ErrorAction SilentlyContinue

    # ── Step 2 · Install ───────────────────────────────────────────────
    $exitCode = $null
    Invoke-WithSpinner -Message "Installing Tithify to $InstallDir..." -Action {
        $p = Start-Process -FilePath $using:TempInstaller `
                           -ArgumentList "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP-" `
                           -PassThru -Wait
        return $p.ExitCode
    } | ForEach-Object { $exitCode = $_ }

    Remove-Item -Path $TempInstaller -Force -ErrorAction SilentlyContinue

    if ($exitCode -and $exitCode -ne 0) {
        throw "Installer exited with error code: $exitCode"
    }

    if (Test-Path $TargetExe) {
        Unblock-File -Path $TargetExe -ErrorAction SilentlyContinue
    }

    Write-OK "Installed."
    Write-Host ""
    Write-Host "  ================================================================" -ForegroundColor Green
    Write-Host "  [+]  Tithify is ready!" -ForegroundColor Green
    Write-Host "  ================================================================" -ForegroundColor Green
    Write-Host ""

    # ── Step 3 · Launch ────────────────────────────────────────────────
    if (Test-Path $TargetExe) {
        Write-Step "Launching $AppName..."
        try {
            Start-Process -FilePath $TargetExe
            Write-OK "Widget is running on your taskbar."
            Write-Host ""
            Write-Host "  Tip: Right-click the widget to change settings, lock position, or check for updates." -ForegroundColor DarkGray
        } catch {
            Write-Warn "Could not auto-launch ${AppName}: $_"
            Write-Warn "Launch manually from: $TargetExe"
        }
    } else {
        Write-Warn "Could not locate $TargetExe to auto-launch."
    }

    Write-Host ""

} catch {
    Write-Host ""
    Write-Fail "Installation failed: $_"
    Write-Host ""
    exit 1
}
