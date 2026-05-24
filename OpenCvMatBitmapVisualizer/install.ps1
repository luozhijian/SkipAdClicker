param(
    [string]$VisualStudioRoot = "C:\Program Files\Microsoft Visual Studio\18\Community"
)

$ErrorActionPreference = "Stop"

$vsix = Join-Path $PSScriptRoot "OpenCvMatBitmapVisualizer.vsix"
if (-not (Test-Path -LiteralPath $vsix)) {
    & (Join-Path $PSScriptRoot "build-vsix.ps1")
}

$installer = Join-Path $VisualStudioRoot "Common7\IDE\VSIXInstaller.exe"
if (-not (Test-Path -LiteralPath $installer)) {
    throw "Cannot find VSIXInstaller.exe at $installer. Pass -VisualStudioRoot if Visual Studio is installed elsewhere."
}

Write-Host "Close Visual Studio before installing; VSIXInstaller cannot update this extension while devenv/debugger/MSBuild processes are running."`r`nWrite-Host "Installing $vsix"
Start-Process -FilePath $installer -ArgumentList @("/quiet", $vsix) -Wait
Write-Host "Installed. Restart Visual Studio before using the cv::Mat bitmap visualizer."

