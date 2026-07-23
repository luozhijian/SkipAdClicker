[CmdletBinding()]
param(
    [Parameter()]
    [ValidateNotNullOrEmpty()]
    [string]$Version = "1.0.35.0",

    [Parameter()]
    [switch]$SkipBuild,

    [Parameter()]
    [string]$CertificatePath,

    [Parameter()]
    [string]$CertificatePassword
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Invoke-ExternalTool {
    param(
        [Parameter(Mandatory)]
        [string]$FilePath,

        [Parameter(Mandatory)]
        [string[]]$Arguments,

        [Parameter()]
        [string]$WorkingDirectory
    )

    $displayArguments = [string[]]$Arguments.Clone()
    if ([IO.Path]::GetFileName($FilePath) -ieq "signtool.exe") {
        for ($index = 0; $index -lt $displayArguments.Count - 1; $index++) {
            if ($displayArguments[$index] -eq "/p") {
                $displayArguments[$index + 1] = "<hidden>"
            }
        }
    }

    Write-Host "> $FilePath $($displayArguments -join ' ')" -ForegroundColor DarkGray
    if ($WorkingDirectory) {
        Push-Location -LiteralPath $WorkingDirectory
    }

    try {
        & $FilePath @Arguments
        if ($LASTEXITCODE -ne 0) {
            throw "'$FilePath' failed with exit code $LASTEXITCODE."
        }
    }
    finally {
        if ($WorkingDirectory) {
            Pop-Location
        }
    }
}

function Find-WindowsSdkTool {
    param(
        [Parameter(Mandatory)]
        [string]$ToolName
    )

    $kitsRoot = Join-Path ${env:ProgramFiles(x86)} "Windows Kits\10\bin"
    if (-not (Test-Path -LiteralPath $kitsRoot)) {
        throw "Windows 10/11 SDK tools were not found. Install the Windows SDK from Visual Studio Installer."
    }

    $sdkDirectories = Get-ChildItem -LiteralPath $kitsRoot -Directory |
        Where-Object { $_.Name -match '^\d+\.\d+\.\d+\.\d+$' } |
        Sort-Object { [version]$_.Name } -Descending

    foreach ($sdkDirectory in $sdkDirectories) {
        $candidate = Join-Path $sdkDirectory.FullName "x64\$ToolName"
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
    }

    throw "$ToolName was not found in any installed Windows SDK."
}

function Find-CMake {
    $cmakeCommand = Get-Command "cmake.exe" -ErrorAction SilentlyContinue
    if ($cmakeCommand) {
        return $cmakeCommand.Source
    }

    $visualStudioRoot = Join-Path $env:ProgramFiles "Microsoft Visual Studio"
    if (Test-Path -LiteralPath $visualStudioRoot) {
        $bundledCMake = Get-ChildItem -LiteralPath $visualStudioRoot -Directory |
            Sort-Object Name -Descending |
            ForEach-Object {
                Get-ChildItem -LiteralPath $_.FullName -Directory -ErrorAction SilentlyContinue
            } |
            ForEach-Object {
                Join-Path $_.FullName "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
            } |
            Where-Object { Test-Path -LiteralPath $_ } |
            Select-Object -First 1

        if ($bundledCMake) {
            return $bundledCMake
        }
    }

    throw "CMake was not found in PATH or in an installed Visual Studio instance."
}

function Remove-ArtifactDirectorySafely {
    param(
        [Parameter(Mandatory)]
        [string]$Path,

        [Parameter(Mandatory)]
        [string]$AllowedRoot
    )

    $fullPath = [IO.Path]::GetFullPath($Path)
    $fullRoot = [IO.Path]::GetFullPath($AllowedRoot).TrimEnd('\') + '\'
    if (-not $fullPath.StartsWith($fullRoot, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to remove '$fullPath' because it is outside '$fullRoot'."
    }

    if (Test-Path -LiteralPath $fullPath) {
        Get-ChildItem -LiteralPath $fullPath -Force |
            Remove-Item -Recurse -Force
    }
}

function Save-AppxManifest {
    param(
        [Parameter(Mandatory)]
        [string]$SourcePath,

        [Parameter(Mandatory)]
        [string]$DestinationPath,

        [Parameter(Mandatory)]
        [string]$PackageVersion
    )

    [xml]$manifest = Get-Content -LiteralPath $SourcePath
    $namespaceManager = [Xml.XmlNamespaceManager]::new($manifest.NameTable)
    $namespaceManager.AddNamespace(
        "appx",
        "http://schemas.microsoft.com/appx/manifest/foundation/windows10"
    )
    $namespaceManager.AddNamespace(
        "uap",
        "http://schemas.microsoft.com/appx/manifest/uap/windows10"
    )
    $namespaceManager.AddNamespace(
        "uap18",
        "http://schemas.microsoft.com/appx/manifest/uap/windows10/18"
    )
    $identity = $manifest.SelectSingleNode("/appx:Package/appx:Identity", $namespaceManager)
    $identity.SetAttribute("Version", $PackageVersion)
    $identity.SetAttribute("ProcessorArchitecture", "x64")

    $resource = $manifest.SelectSingleNode(
        "/appx:Package/appx:Resources/appx:Resource",
        $namespaceManager
    )
    $resource.SetAttribute("Language", "en-us")

    $application = $manifest.SelectSingleNode(
        "/appx:Package/appx:Applications/appx:Application",
        $namespaceManager
    )
    $application.SetAttribute("Executable", "SkipAdClicker.exe")
    $application.SetAttribute("EntryPoint", "Windows.FullTrustApplication")
    $application.RemoveAttribute(
        "TrustLevel",
        "http://schemas.microsoft.com/appx/manifest/uap/windows10/18"
    )
    $application.RemoveAttribute(
        "RuntimeBehavior",
        "http://schemas.microsoft.com/appx/manifest/uap/windows10/18"
    )

    # MakeAppx does not perform the Visual Studio resource-qualifier expansion.
    $visualElements = $application.SelectSingleNode("uap:VisualElements", $namespaceManager)
    $visualElements.SetAttribute(
        "Square150x150Logo",
        "Images\Square150x150Logo.scale-200.png"
    )
    $visualElements.SetAttribute(
        "Square44x44Logo",
        "Images\Square44x44Logo.scale-200.png"
    )

    $defaultTile = $visualElements.SelectSingleNode("uap:DefaultTile", $namespaceManager)
    $defaultTile.SetAttribute(
        "Wide310x150Logo",
        "Images\Wide310x150Logo.scale-200.png"
    )

    $splashScreen = $visualElements.SelectSingleNode("uap:SplashScreen", $namespaceManager)
    $splashScreen.SetAttribute("Image", "Images\SplashScreen.scale-200.png")

    $settings = [Xml.XmlWriterSettings]::new()
    $settings.Encoding = [Text.UTF8Encoding]::new($false)
    $settings.Indent = $true

    $writer = [Xml.XmlWriter]::Create($DestinationPath, $settings)
    try {
        $manifest.Save($writer)
    }
    finally {
        $writer.Dispose()
    }
}

function New-TestSigningCertificate {
    param(
        [Parameter(Mandatory)]
        [string]$Publisher,

        [Parameter(Mandatory)]
        [string]$PfxPath,

        [Parameter(Mandatory)]
        [string]$CerPath,

        [Parameter(Mandatory)]
        [string]$Password
    )

    $certificate = New-SelfSignedCertificate `
        -Type Custom `
        -Subject $Publisher `
        -FriendlyName "SkipAdClicker temporary test signing certificate" `
        -CertStoreLocation "Cert:\CurrentUser\My" `
        -KeyAlgorithm RSA `
        -KeyLength 2048 `
        -HashAlgorithm SHA256 `
        -KeyUsage DigitalSignature `
        -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.3") `
        -NotAfter (Get-Date).AddYears(1)

    $securePassword = ConvertTo-SecureString -String $Password -AsPlainText -Force
    Export-PfxCertificate `
        -Cert $certificate `
        -FilePath $PfxPath `
        -Password $securePassword | Out-Null
    Export-Certificate -Cert $certificate -FilePath $CerPath | Out-Null

    return $certificate
}

function Export-PublicCertificateFromPfx {
    param(
        [Parameter(Mandatory)]
        [string]$PfxPath,

        [Parameter()]
        [string]$Password,

        [Parameter(Mandatory)]
        [string]$CerPath
    )

    $certificate = [Security.Cryptography.X509Certificates.X509Certificate2]::new()
    $certificate.Import(
        $PfxPath,
        $Password,
        [Security.Cryptography.X509Certificates.X509KeyStorageFlags]::DefaultKeySet
    )
    [IO.File]::WriteAllBytes(
        $CerPath,
        $certificate.Export(
            [Security.Cryptography.X509Certificates.X509ContentType]::Cert
        )
    )
    $certificate.Dispose()
}

function Write-TestInstallScript {
    param(
        [Parameter(Mandatory)]
        [string]$DestinationPath,

        [Parameter(Mandatory)]
        [string]$BundleFileName,

        [Parameter(Mandatory)]
        [string]$CertificateFileName
    )

    $content = @'
[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

$currentIdentity = [Security.Principal.WindowsIdentity]::GetCurrent()
$currentPrincipal = [Security.Principal.WindowsPrincipal]::new($currentIdentity)
$isAdministrator = $currentPrincipal.IsInRole(
    [Security.Principal.WindowsBuiltInRole]::Administrator
)

if (-not $isAdministrator) {
    Write-Host "Administrator permission is required to trust the test certificate."
    $process = Start-Process `
        -FilePath "powershell.exe" `
        -Verb RunAs `
        -ArgumentList @(
            "-NoProfile",
            "-ExecutionPolicy", "Bypass",
            "-File", "`"$PSCommandPath`""
        ) `
        -Wait `
        -PassThru
    exit $process.ExitCode
}

$bundlePath = Join-Path $PSScriptRoot "__BUNDLE_FILE__"
$certificatePath = Join-Path $PSScriptRoot "__CERTIFICATE_FILE__"

if (-not (Test-Path -LiteralPath $bundlePath)) {
    throw "The APPX bundle was not found at '$bundlePath'."
}
if (-not (Test-Path -LiteralPath $certificatePath)) {
    throw "The test certificate was not found at '$certificatePath'."
}

$certificate = [Security.Cryptography.X509Certificates.X509Certificate2]::new(
    $certificatePath
)
$trustedCertificate = Get-ChildItem -LiteralPath "Cert:\LocalMachine\TrustedPeople" |
    Where-Object { $_.Thumbprint -eq $certificate.Thumbprint } |
    Select-Object -First 1

if (-not $trustedCertificate) {
    Write-Host "Trusting the SkipAdClicker test certificate for this computer..."
    Import-Certificate `
        -FilePath $certificatePath `
        -CertStoreLocation "Cert:\LocalMachine\TrustedPeople" | Out-Null
}

Write-Host "Installing SkipAdClicker..."
try {
    Add-AppxPackage `
        -Path $bundlePath `
        -ForceApplicationShutdown `
        -ForceUpdateFromAnyVersion
}
catch {
    Write-Host ""
    Write-Host "Installation failed. Windows deployment details:" -ForegroundColor Red
    $activityIdMatch = [regex]::Match(
        $_.Exception.Message,
        '\[ActivityId\]\s+([0-9a-fA-F-]+)'
    )
    if ($activityIdMatch.Success) {
        Get-AppPackageLog -ActivityID $activityIdMatch.Groups[1].Value |
            Select-Object Time,Id,Message |
            Format-List
    }
    throw
}

Write-Host "SkipAdClicker was installed successfully." -ForegroundColor Green
'@

    $content = $content.Replace("__BUNDLE_FILE__", $BundleFileName)
    $content = $content.Replace("__CERTIFICATE_FILE__", $CertificateFileName)
    [IO.File]::WriteAllText(
        $DestinationPath,
        $content,
        [Text.UTF8Encoding]::new($false)
    )
}

$parsedVersion = $null
if (-not [version]::TryParse($Version, [ref]$parsedVersion) -or
    $Version -notmatch '^\d+\.\d+\.\d+\.\d+$') {
    throw "Version must contain four numeric parts, for example 1.0.33.0."
}

$versionParts = $Version.Split('.') | ForEach-Object { [int]$_ }
if (@($versionParts | Where-Object { $_ -gt 65535 }).Count -gt 0) {
    throw "Every package version part must be between 0 and 65535."
}
if ($versionParts[3] -ne 0) {
    throw "The fourth package version part must be zero for a Microsoft Store submission."
}

$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot ".."))
$windowsScriptRoot = Join-Path $repoRoot "WindowsScript"
$manifestSource = Join-Path $windowsScriptRoot "Package.appxmanifest"
$imageSource = Join-Path $windowsScriptRoot "Images"
$releaseDirectory = Join-Path $repoRoot "out\build\x64-Release\x64\Release"
$artifactRoot = Join-Path $windowsScriptRoot "Artifacts"
$versionArtifactRoot = Join-Path $artifactRoot $Version
$stagingDirectory = Join-Path $versionArtifactRoot "Staging\x64"
$bundleInputDirectory = Join-Path $versionArtifactRoot "BundleInput"
$uploadInputDirectory = Join-Path $versionArtifactRoot "UploadInput"
$symbolStagingDirectory = Join-Path $versionArtifactRoot "Symbols"
$packageName = "SkipAdClicker_${Version}_x64"
$appxPath = Join-Path $versionArtifactRoot "$packageName.appx"
$bundlePath = Join-Path $versionArtifactRoot "${packageName}_bundle.appxbundle"
$symbolPath = Join-Path $versionArtifactRoot "$packageName.appxsym"
$uploadPath = Join-Path $versionArtifactRoot "${packageName}_bundle.appxupload"
$testCertificatePath = Join-Path $versionArtifactRoot "SkipAdClicker_TestCertificate.cer"
$temporaryPfxPath = Join-Path $versionArtifactRoot "SkipAdClicker_TemporarySigning.pfx"
$testInstallPath = Join-Path $versionArtifactRoot "TestInstall.ps1"

if (-not (Test-Path -LiteralPath $manifestSource)) {
    throw "The package manifest was not found at '$manifestSource'."
}
if (-not (Test-Path -LiteralPath $imageSource)) {
    throw "The Store image directory was not found at '$imageSource'."
}

if (-not $SkipBuild) {
    $cmake = Find-CMake

    Write-Host "Configuring the x64 Release build..." -ForegroundColor Cyan
    Invoke-ExternalTool `
        -FilePath $cmake `
        -Arguments @("--preset", "x64-Release") `
        -WorkingDirectory $repoRoot

    Write-Host "Building SkipAdClicker x64 Release..." -ForegroundColor Cyan
    Invoke-ExternalTool `
        -FilePath $cmake `
        -Arguments @(
            "--build", "--preset", "x64-Release", "--target", "SkipAdClicker"
        ) `
        -WorkingDirectory $repoRoot
}

$executablePath = Join-Path $releaseDirectory "SkipAdClicker.exe"
if (-not (Test-Path -LiteralPath $executablePath)) {
    throw "Release output was not found at '$releaseDirectory'. Build x64-Release first or omit -SkipBuild."
}

Remove-ArtifactDirectorySafely -Path $versionArtifactRoot -AllowedRoot $artifactRoot
New-Item -ItemType Directory -Path $stagingDirectory -Force | Out-Null
New-Item -ItemType Directory -Path $bundleInputDirectory -Force | Out-Null
New-Item -ItemType Directory -Path $uploadInputDirectory -Force | Out-Null

Write-Host "Staging application files..." -ForegroundColor Cyan
Get-ChildItem -LiteralPath $releaseDirectory -File |
    Copy-Item -Destination $stagingDirectory -Force

$excludedBuildDirectories = @("ALL_BUILD", "ZERO_CHECK")
Get-ChildItem -LiteralPath $releaseDirectory -Directory |
    Where-Object { $_.Name -notin $excludedBuildDirectories } |
    Copy-Item -Destination $stagingDirectory -Recurse -Force

Copy-Item -LiteralPath $imageSource -Destination $stagingDirectory -Recurse -Force
Save-AppxManifest `
    -SourcePath $manifestSource `
    -DestinationPath (Join-Path $stagingDirectory "AppxManifest.xml") `
    -PackageVersion $Version

$makeAppx = Find-WindowsSdkTool -ToolName "makeappx.exe"
$signTool = Find-WindowsSdkTool -ToolName "signtool.exe"
$temporaryCertificate = $null
$usingTemporaryCertificate = -not $CertificatePath

if ($CertificatePath) {
    $CertificatePath = [IO.Path]::GetFullPath($CertificatePath)
    if (-not (Test-Path -LiteralPath $CertificatePath)) {
        throw "Signing certificate '$CertificatePath' does not exist."
    }
    Export-PublicCertificateFromPfx `
        -PfxPath $CertificatePath `
        -Password $CertificatePassword `
        -CerPath $testCertificatePath
}
else {
    [xml]$sourceManifest = Get-Content -LiteralPath $manifestSource
    $publisher = [string]$sourceManifest.Package.Identity.Publisher
    $CertificatePassword = [Guid]::NewGuid().ToString("N")
    $CertificatePath = $temporaryPfxPath
    $temporaryCertificate = New-TestSigningCertificate `
        -Publisher $publisher `
        -PfxPath $CertificatePath `
        -CerPath $testCertificatePath `
        -Password $CertificatePassword
}

try {
    Write-Host "Creating x64 APPX package..." -ForegroundColor Cyan
    Invoke-ExternalTool -FilePath $makeAppx -Arguments @(
        "pack", "/d", $stagingDirectory, "/p", $appxPath, "/o"
    )

    $signArguments = @("sign", "/fd", "SHA256", "/f", $CertificatePath)
    if ($CertificatePassword) {
        $signArguments += @("/p", $CertificatePassword)
    }
    $signArguments += $appxPath
    Invoke-ExternalTool -FilePath $signTool -Arguments $signArguments

    Copy-Item -LiteralPath $appxPath -Destination $bundleInputDirectory

    Write-Host "Creating APPX bundle..." -ForegroundColor Cyan
    Invoke-ExternalTool -FilePath $makeAppx -Arguments @(
        "bundle", "/d", $bundleInputDirectory, "/p", $bundlePath, "/o"
    )

    $signArguments = @("sign", "/fd", "SHA256", "/f", $CertificatePath)
    if ($CertificatePassword) {
        $signArguments += @("/p", $CertificatePassword)
    }
    $signArguments += $bundlePath
    Invoke-ExternalTool -FilePath $signTool -Arguments $signArguments
}
finally {
    if ($usingTemporaryCertificate) {
        if ($temporaryCertificate) {
            Remove-Item `
                -LiteralPath "Cert:\CurrentUser\My\$($temporaryCertificate.Thumbprint)" `
                -Force `
                -ErrorAction SilentlyContinue
        }
        Remove-Item -LiteralPath $temporaryPfxPath -Force -ErrorAction SilentlyContinue
    }
}

Copy-Item -LiteralPath $bundlePath -Destination $uploadInputDirectory
Write-TestInstallScript `
    -DestinationPath $testInstallPath `
    -BundleFileName ([IO.Path]::GetFileName($bundlePath)) `
    -CertificateFileName ([IO.Path]::GetFileName($testCertificatePath))

$pdbFiles = @(Get-ChildItem -LiteralPath $releaseDirectory -Recurse -File -Filter *.pdb)
if ($pdbFiles.Count -gt 0) {
    New-Item -ItemType Directory -Path $symbolStagingDirectory -Force | Out-Null
    foreach ($pdbFile in $pdbFiles) {
        Copy-Item -LiteralPath $pdbFile.FullName -Destination $symbolStagingDirectory -Force
    }

    Add-Type -AssemblyName System.IO.Compression.FileSystem
    [IO.Compression.ZipFile]::CreateFromDirectory(
        $symbolStagingDirectory,
        $symbolPath,
        [IO.Compression.CompressionLevel]::Optimal,
        $false
    )
    Copy-Item -LiteralPath $symbolPath -Destination $uploadInputDirectory
}
else {
    Write-Warning "No PDB files were found. The upload will not include Store crash-analysis symbols."
}

if (-not ("System.IO.Compression.ZipFile" -as [type])) {
    Add-Type -AssemblyName System.IO.Compression.FileSystem
}
[IO.Compression.ZipFile]::CreateFromDirectory(
    $uploadInputDirectory,
    $uploadPath,
    [IO.Compression.CompressionLevel]::Optimal,
    $false
)

Write-Host ""
Write-Host "Microsoft Store upload package created:" -ForegroundColor Green
Write-Host $uploadPath
Write-Host ""
Write-Host "Bundle:" -ForegroundColor Green
Write-Host $bundlePath
Write-Host ""
Write-Host "Local test installer:" -ForegroundColor Green
Write-Host $testInstallPath
