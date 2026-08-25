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
$ProgressPreference = "SilentlyContinue"

# Configure modern TLS and connection limits immediately
try {
    [Net.ServicePointManager]::SecurityProtocol =
        [Net.SecurityProtocolType]::Tls12 -bor [Net.SecurityProtocolType]::Tls13
    [Net.ServicePointManager]::DefaultConnectionLimit = 10
} catch {}

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

function Write-Step {
    param([string]$Text, [ConsoleColor]$Color = [ConsoleColor]::Cyan)
    Write-Host "  ==>  $Text" -ForegroundColor $Color
}
function Write-OK   { param([string]$Text) Write-Host "  [+]  $Text" -ForegroundColor Green  }
function Write-Warn { param([string]$Text) Write-Host "  [!]  $Text" -ForegroundColor Yellow }
function Write-Fail { param([string]$Text) Write-Host "  [-]  $Text" -ForegroundColor Red    }

# ---------------------------------------------------------------------------
# Spinner Helpers: Native .NET & PowerShell with ZERO external dependencies
# Works reliably in Windows PowerShell 5.1, PowerShell 7+, and irm | iex
# ---------------------------------------------------------------------------

function Download-FileWithSpinner {
    param(
        [string]$Url,
        [string]$Destination
    )

    $spinChars = [char[]]@(0x2838, 0x2830, 0x2810, 0x2800,
                            0x2801, 0x2803, 0x2807, 0x280F,
                            0x281F, 0x283F, 0x287F, 0x28FF,
                            0x28F7, 0x28E3, 0x28C1, 0x2880)
    $spinIdx = 0

    try { [Console]::CursorVisible = $false } catch {}

    $req = [System.Net.HttpWebRequest]::Create($Url)
    $req.AllowAutoRedirect = $true
    $req.UserAgent = "Tithify-Installer"
    $req.Timeout = 60000

    $res = $req.GetResponse()
    $totalBytes = $res.ContentLength
    $stream = $res.GetResponseStream()

    $destDir = Split-Path -Parent $Destination
    if ($destDir -and -not (Test-Path $destDir)) {
        New-Item -ItemType Directory -Path $destDir -Force | Out-Null
    }

    $fileStream = [System.IO.File]::Create($Destination)
    $buffer = New-Object byte[] 65536
    $totalRead = 0
    $lastUpdate = [System.DateTime]::MinValue

    try {
        while (($bytesRead = $stream.Read($buffer, 0, $buffer.Length)) -gt 0) {
            $fileStream.Write($buffer, 0, $bytesRead)
            $totalRead += $bytesRead

            $now = [System.DateTime]::Now
            if (($now - $lastUpdate).TotalMilliseconds -ge 60) {
                $lastUpdate = $now
                $c = $spinChars[$spinIdx % $spinChars.Count]
                $spinIdx++

                if ($totalBytes -gt 0) {
                    $pct = [math]::Round(($totalRead / $totalBytes) * 100)
                    $mbRec = [math]::Round($totalRead / 1MB, 2)
                    $mbTot = [math]::Round($totalBytes / 1MB, 2)
                    $line = "  $c  Downloading Tithify_Setup.exe ($pct% - $mbRec of $mbTot MB)..."
                } else {
                    $mbRec = [math]::Round($totalRead / 1MB, 2)
                    $line = "  $c  Downloading Tithify_Setup.exe ($mbRec MB)..."
                }
                Write-Host "`r$line" -NoNewline -ForegroundColor Cyan
            }
        }
    } finally {
        $fileStream.Flush()
        $fileStream.Close()
        $fileStream.Dispose()
        $stream.Close()
        $stream.Dispose()
        $res.Close()
        $res.Dispose()
    }

    # Erase spinner line
    $pad = " " * 80
    Write-Host "`r$pad`r" -NoNewline
    try { [Console]::CursorVisible = $true } catch {}
}

function Invoke-ProcessWithSpinner {
    param(
        [string]$FilePath,
        [string]$ArgumentList,
        [string]$Message,
        [ConsoleColor]$Color = [ConsoleColor]::Cyan
    )

    $spinChars = [char[]]@(0x2838, 0x2830, 0x2810, 0x2800,
                            0x2801, 0x2803, 0x2807, 0x280F,
                            0x281F, 0x283F, 0x287F, 0x28FF,
                            0x28F7, 0x28E3, 0x28C1, 0x2880)
    $spinIdx = 0

    try { [Console]::CursorVisible = $false } catch {}

    $proc = Start-Process -FilePath $FilePath -ArgumentList $ArgumentList -PassThru

    while (-not $proc.HasExited) {
        $c = $spinChars[$spinIdx % $spinChars.Count]
        $spinIdx++
        $line = "  $c  $Message"
        Write-Host "`r$line" -NoNewline -ForegroundColor $Color
        Start-Sleep -Milliseconds 60
    }

    # Erase spinner line
    $pad = " " * 80
    Write-Host "`r$pad`r" -NoNewline
    try { [Console]::CursorVisible = $true } catch {}

    return $proc.ExitCode
}

# --- UNINSTALL FLOW --------------------------------------------------------
if ($Uninstall) {
    Write-BrandHeader
    Write-Step "Uninstalling $AppName..." Yellow

    Get-Process -Name "Tithify","NepaliDateWidget" -ErrorAction SilentlyContinue |
        Stop-Process -Force -ErrorAction SilentlyContinue
    Start-Sleep -Milliseconds 500

    if (Test-Path $Uninstaller) {
        $exitCode = Invoke-ProcessWithSpinner `
            -FilePath $Uninstaller `
            -ArgumentList "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART" `
            -Message "Running uninstaller..." `
            -Color Gray

        Write-OK "$AppName uninstalled successfully."
    } elseif (Test-Path $InstallDir) {
        Remove-Item -Path $InstallDir -Recurse -Force -ErrorAction SilentlyContinue
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
    Download-FileWithSpinner -Url $DownloadUrl -Destination $TempInstaller

    if (-not (Test-Path $TempInstaller) -or ((Get-Item $TempInstaller).Length -lt 100000)) {
        throw "Download failed or file is corrupted."
    }

    $sizeMB = [math]::Round((Get-Item $TempInstaller).Length / 1MB, 2)
    Write-OK "Downloaded  ($sizeMB MB)"

    Unblock-File -Path $TempInstaller -ErrorAction SilentlyContinue

    # ── Step 2 · Install ───────────────────────────────────────────────
    $exitCode = Invoke-ProcessWithSpinner `
        -FilePath $TempInstaller `
        -ArgumentList "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP-" `
        -Message "Installing Tithify to $InstallDir..." `
        -Color Cyan

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
    return
}
