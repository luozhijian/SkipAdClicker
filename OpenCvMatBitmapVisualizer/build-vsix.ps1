$ErrorActionPreference = "Stop"

$root = $PSScriptRoot
$src = Join-Path $root "src"
$bin = Join-Path $root "bin"
$output = Join-Path $root "OpenCvMatBitmapVisualizer.vsix"

$vsRoot = "C:\Program Files\Microsoft Visual Studio\18\Community"
$csc = Join-Path $vsRoot "MSBuild\Current\Bin\Roslyn\csc.exe"
$vsixUtil = Join-Path $vsRoot "VSSDK\VisualStudioIntegration\Tools\Bin\VsixUtil.exe"
$refNet48 = "C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.8"
$publicAssemblies = Join-Path $vsRoot "Common7\IDE\PublicAssemblies"

if (-not (Test-Path -LiteralPath $csc)) { throw "Cannot find csc.exe at $csc" }
if (-not (Test-Path -LiteralPath $vsixUtil)) { throw "Cannot find VsixUtil.exe at $vsixUtil" }
if (-not (Test-Path -LiteralPath $refNet48)) { throw "Cannot find .NET Framework 4.8 reference assemblies at $refNet48" }

New-Item -ItemType Directory -Force -Path $bin | Out-Null
$dll = Join-Path $bin "OpenCvMatBitmapVisualizer.dll"

$references = @(
    (Join-Path $refNet48 "mscorlib.dll"),
    (Join-Path $refNet48 "System.dll"),
    (Join-Path $refNet48 "System.Core.dll"),
    (Join-Path $refNet48 "WindowsBase.dll"),
    (Join-Path $refNet48 "PresentationCore.dll"),
    (Join-Path $refNet48 "PresentationFramework.dll"),
    (Join-Path $publicAssemblies "Microsoft.VisualStudio.Debugger.Interop.11.0.dll"),
    (Join-Path $publicAssemblies "Microsoft.VisualStudio.Debugger.InteropA.dll"),
    (Join-Path $publicAssemblies "Microsoft.VisualStudio.Shell.15.0.dll"),
    (Join-Path $publicAssemblies "Microsoft.VisualStudio.Shell.Framework.dll"),
    (Join-Path $publicAssemblies "Microsoft.VisualStudio.Shell.Interop.dll"),
    (Join-Path $publicAssemblies "Microsoft.VisualStudio.OLE.Interop.dll"),
    (Join-Path $publicAssemblies "Microsoft.VisualStudio.Interop.dll")
)

$sourceFiles = Get-ChildItem -LiteralPath $src -Filter *.cs | ForEach-Object { $_.FullName }
& $csc /noconfig /nostdlib+ /target:library /platform:anycpu /langversion:latest /out:$dll ($references | ForEach-Object { "/reference:$_" }) $sourceFiles
if ($LASTEXITCODE -ne 0) { throw "C# compiler failed with exit code $LASTEXITCODE" }

if (Test-Path -LiteralPath $output) { Remove-Item -LiteralPath $output -Force }

& $vsixUtil package -sourceManifest (Join-Path $root "extension.vsixmanifest") -outputPath $output -setupProductArch amd64 -targetSdkVersion 18.0 -noValidate
if ($LASTEXITCODE -ne 0) { throw "VsixUtil failed with exit code $LASTEXITCODE" }

Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$zip = [System.IO.Compression.ZipFile]::Open($output, [System.IO.Compression.ZipArchiveMode]::Update)
try {
    foreach ($name in @("OpenCvMatBitmap.natvis", "OpenCvMatBitmapVisualizer.pkgdef", "OpenCvMatBitmapVisualizer.dll", "[Content_Types].xml")) {
        $entry = $zip.GetEntry($name)
        if ($entry) { $entry.Delete() }
    }

    [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip, (Join-Path $root "OpenCvMatBitmap.natvis"), "OpenCvMatBitmap.natvis") | Out-Null
    [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip, (Join-Path $root "OpenCvMatBitmapVisualizer.pkgdef"), "OpenCvMatBitmapVisualizer.pkgdef") | Out-Null
    [System.IO.Compression.ZipFileExtensions]::CreateEntryFromFile($zip, $dll, "OpenCvMatBitmapVisualizer.dll") | Out-Null

    $contentTypes = $zip.CreateEntry("[Content_Types].xml")
    $stream = $contentTypes.Open()
    $writer = New-Object IO.StreamWriter($stream, [Text.Encoding]::UTF8)
    try {
        $writer.Write('<?xml version="1.0" encoding="utf-8"?><Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types"><Default Extension="vsixmanifest" ContentType="text/xml" /><Default Extension="json" ContentType="application/json" /><Default Extension="natvis" ContentType="text/xml" /><Default Extension="pkgdef" ContentType="text/plain" /><Default Extension="dll" ContentType="application/octet-stream" /></Types>')
    }
    finally {
        $writer.Dispose()
    }
}
finally {
    $zip.Dispose()
}

Write-Host "Built $output"
