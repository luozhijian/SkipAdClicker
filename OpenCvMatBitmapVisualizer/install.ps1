param(
    [string]$VisualStudioRoot,
    [string]$InstanceId
)

$ErrorActionPreference = "Stop"

$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not $VisualStudioRoot -or -not $InstanceId) {
    if (-not (Test-Path -LiteralPath $vswhere)) {
        throw "Cannot find vswhere.exe at $vswhere."
    }

    $instance = & $vswhere -latest -products * -version "[18.0,19.0)" -format json |
        ConvertFrom-Json |
        Select-Object -First 1
    if (-not $instance) {
        throw "Visual Studio 2026 (version 18.x) was not found."
    }

    if (-not $VisualStudioRoot) {
        $VisualStudioRoot = $instance.installationPath
    }
    if (-not $InstanceId) {
        $InstanceId = $instance.instanceId
    }
}

$vsix = Join-Path $PSScriptRoot "OpenCvMatBitmapVisualizer.vsix"
if (-not (Test-Path -LiteralPath $vsix)) {
    & (Join-Path $PSScriptRoot "build-vsix.ps1")
}

$installer = Join-Path $VisualStudioRoot "Common7\IDE\VSIXInstaller.exe"
if (-not (Test-Path -LiteralPath $installer)) {
    throw "Cannot find VSIXInstaller.exe at $installer."
}

Write-Host "Close Visual Studio before installing; VSIXInstaller cannot update this extension while Visual Studio is running."
Write-Host "Installing $vsix into Visual Studio instance $InstanceId"

& $installer "/quiet" "/instanceIds:$InstanceId" $vsix
if ($LASTEXITCODE -ne 0) {
    throw "VSIXInstaller failed with exit code $LASTEXITCODE."
}

Write-Host "Installed. Restart Visual Studio before using the cv::Mat bitmap visualizer."
