$ErrorActionPreference = "Stop"
Write-Host "🚀 Installing CNav for Windows..."

# GitHub release URL for the Windows version
$DownloadUrl = "https://github.com/Raiza04/cnav/releases/latest/download/cnav-windows-amd64.exe"
$InstallDir = "$HOME\.local\bin"
$ExePath = "$InstallDir\n.exe"

# Create the directory if it does not exist
if (!(Test-Path $InstallDir)) {
    New-Item -ItemType Directory -Force -Path $InstallDir | Out-Null
}

Write-Host "⬇️ Downloading pre-compiled CNav binary..."
Invoke-WebRequest -Uri $DownloadUrl -OutFile $ExePath

# Check if the PowerShell profile exists, otherwise create it
if (!(Test-Path $PROFILE)) {
    New-Item -ItemType File -Force -Path $PROFILE | Out-Null
}

# Add PATH and init hook to the profile (if not already present)
$ProfileContent = Get-Content $PROFILE -Raw -ErrorAction SilentlyContinue
if ($ProfileContent -notmatch "n --init") {
    Add-Content -Path $PROFILE -Value "`n# CNav (Command Navigation) Auto-Init"
    Add-Content -Path $PROFILE -Value "`$env:PATH += `";$InstallDir`""
    Add-Content -Path $PROFILE -Value "Invoke-Expression (n --init | Out-String)"
    Write-Host "✅ Hooks have been added to your PowerShell profile!"
} else {
    Write-Host "⚡ Hooks were already present in your profile."
}

Write-Host "`n🎉 Installation completed successfully!"
Write-Host "👉 Please restart PowerShell or run: . `$PROFILE"
