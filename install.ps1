<#
.SYNOPSIS
    Installs or uninstalls Nepali Date Widget for Windows via the command line.

.DESCRIPTION
    Downloads the latest release of Nepali Date Widget from GitHub and performs
    a clean, silent installation into the user's Local AppData directory.

.EXAMPLE
    # Install via PowerShell one-liner:
    irm https://raw.githubusercontent.com/aayushlbef/Nepali-Date-Widget/main/install.ps1 | iex

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
$Repo = "aayushlbef/Nepali-Date-Widget"
$AppName = "Nepali Date Widget"
$ExeName = "NepaliDateWidget.exe"
$InstallDir = Join-Path $env:LOCALAPPDATA $AppName
$TargetExe = Join-Path $InstallDir $ExeName
$Uninstaller = Join-Path $InstallDir "unins000.exe"

function Write-BrandHeader {
    Write-Host ""
    Write-Host "  Nepali Date Widget - Windows Taskbar Widget" -ForegroundColor Cyan
    Write-Host "  ===========================================" -ForegroundColor DarkGray
    Write-Host ""
}

# --- UNINSTALL FLOW ---
if ($Uninstall) {
    Write-BrandHeader
    Write-Host "==> Uninstalling $AppName..." -ForegroundColor Yellow

    # Terminate running process if any
    Get-Process -Name "NepaliDateWidget" -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
    Start-Sleep -Milliseconds 500

    if (Test-Path $Uninstaller) {
        Write-Host "==> Running uninstaller silently..." -ForegroundColor Gray
        Start-Process -FilePath $Uninstaller -ArgumentList "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART" -Wait
        Write-Host "[+] $AppName has been successfully uninstalled." -ForegroundColor Green
    } else {
        # Fallback: remove files manually if uninstaller was not found
        if (Test-Path $InstallDir) {
            Remove-Item -Path $InstallDir -Recurse -Force -ErrorAction SilentlyContinue
            Write-Host "[+] Removed directory: $InstallDir" -ForegroundColor Green
        } else {
            Write-Host "[!] $AppName does not appear to be installed." -ForegroundColor DarkYellow
        }
    }
    Write-Host ""
    return
}

# --- INSTALL FLOW ---
try {
    Write-BrandHeader

    # Ensure TLS 1.2+ is active
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12 -bor [Net.SecurityProtocolType]::Tls13

    # Close any running instance before updating
    $running = Get-Process -Name "NepaliDateWidget" -ErrorAction SilentlyContinue
    if ($running) {
        Write-Host "==> Closing active $AppName instance..." -ForegroundColor Yellow
        $running | Stop-Process -Force -ErrorAction SilentlyContinue
        Start-Sleep -Milliseconds 500
    }

    # Determine Download URL
    if ($Version -eq "latest") {
        $DownloadUrl = "https://github.com/$Repo/releases/latest/download/NepaliDateWidget_Setup.exe"
    } else {
        $Tag = if ($Version.StartsWith("v")) { $Version } else { "v$Version" }
        $DownloadUrl = "https://github.com/$Repo/releases/download/$Tag/NepaliDateWidget_Setup.exe"
    }

    $RandomId = [System.Guid]::NewGuid().ToString('N')
    $TempInstaller = Join-Path $env:TEMP "NepaliDateWidget_Setup_$RandomId.exe"

    Write-Host "==> Downloading latest installer from GitHub..." -ForegroundColor Cyan
    Write-Host "    $DownloadUrl" -ForegroundColor DarkGray

    # Use WebClient / Invoke-WebRequest with progress hidden for speed
    $ProgressPreference = 'SilentlyContinue'
    Invoke-WebRequest -Uri $DownloadUrl -OutFile $TempInstaller -UseBasicParsing
    $ProgressPreference = 'Continue'

    if (-not (Test-Path $TempInstaller) -or ((Get-Item $TempInstaller).Length -lt 100000)) {
        throw "Installer download failed or file is corrupted."
    }

    Write-Host "==> Installing silently to $InstallDir..." -ForegroundColor Cyan
    # Run Inno Setup installer silently
    $process = Start-Process -FilePath $TempInstaller -ArgumentList "/VERYSILENT /SUPPRESSMSGBOXES /NORESTART /SP-" -PassThru -Wait

    # Cleanup temp installer
    Remove-Item -Path $TempInstaller -Force -ErrorAction SilentlyContinue

    if ($process.ExitCode -ne 0) {
        throw "Installer exited with error code: $($process.ExitCode)"
    }

    Write-Host ""
    Write-Host "  ================================================================" -ForegroundColor Green
    Write-Host "  [+] Nepali Date Widget successfully installed!" -ForegroundColor Green
    Write-Host "  ================================================================" -ForegroundColor Green
    Write-Host ""

    # Start the installed application
    if (Test-Path $TargetExe) {
        Write-Host "==> Launching $AppName..." -ForegroundColor Cyan
        Start-Process -FilePath $TargetExe
        Write-Host "[+] Widget is now running on your taskbar / screen." -ForegroundColor Green
        Write-Host ""
        Write-Host "Tip: Right-click the widget anytime to change settings, lock position, or check for updates." -ForegroundColor DarkGray
    } else {
        Write-Host "[!] Note: Could not locate $TargetExe to start automatically." -ForegroundColor DarkYellow
    }

    Write-Host ""
} catch {
    Write-Host ""
    Write-Host "[-] Installation failed: $_" -ForegroundColor Red
    Write-Host ""
    exit 1
}
