$ErrorActionPreference = "Stop"
Write-Host "🚀 Installing CNav for Windows..."

# GitHub Release URL for the Windows binary
$DownloadUrl = "https://github.com/Raiza04/cnav/releases/latest/download/cnav-windows-amd64.exe"
$InstallDir = "$HOME\.local\bin"
$ExePath = "$InstallDir\n.exe"

# Create the installation directory if it doesn't exist
if (!(Test-Path $InstallDir)) {
    New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null
}

Write-Host "⬇️ Downloading pre-compiled CNav binary..."
Invoke-WebRequest -Uri $DownloadUrl -OutFile $ExePath

# Check if the PowerShell profile exists, create it if it doesn't
if (!(Test-Path $PROFILE)) {
    New-Item -ItemType File -Force -Path $PROFILE | Out-Null
}

# FIX: Casting to [string] forces PowerShell to treat empty files as "" instead of $null
[string]$ProfileContent = Get-Content $PROFILE -Raw -ErrorAction SilentlyContinue

$Updated = $false

# 1. Check and inject PATH
if ($ProfileContent -notmatch "\.local\\bin") {
    Add-Content -Path $PROFILE -Value "`n# CNav PATH"
    Add-Content -Path $PROFILE -Value "`$env:PATH += `";$InstallDir`""
    Write-Host "✅ Path was added to your profile."
    $Updated = $true
}

# 2. Check and inject Auto-Init Hook
if ($ProfileContent -notmatch "n --init") {
    Add-Content -Path $PROFILE -Value "# CNav Auto-Init"
    Add-Content -Path $PROFILE -Value "Invoke-Expression (n --init | Out-String)"
    Write-Host "✅ Hooks were added to your profile."
    $Updated = $true
}

if (-not $Updated) {
    Write-Host "⚡ Hooks and Path were already present in your profile."
}

Write-Host "`n🎉 Installation completed successfully!"
Write-Host "👉 Please restart PowerShell (close and reopen the window)."
