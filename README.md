# SkipAdClicker

SkipAdClicker is a desktop automation tool that uses screenshots, OCR, and image detection to find and click "Skip Ad" style controls. The project is written in C++ with Qt for the UI, OpenCV for image processing, and Tesseract for OCR.

It is fully tested in windows, is working on Linux and MacOS.   Windows application is released at Microsoft Store: https://apps.microsoft.com/detail/9nr67kb8hr4d?hl=en-US&gl=US

The app is built around a small test-book/action engine, so the bundled `SkipAd` workflow can be adjusted through text files and settings instead of recompiling the application.

## Features

- Qt desktop application for running automation workflows.
- Screenshot-based UI detection with OpenCV.
- OCR support through Tesseract.
- Configurable action scripts under `SkipAdClicker/SkipAd`.
- Includes sample Skip Ad settings, resources, and test screenshots.
- Single-instance option and file-based logging.

## Repository Layout

```text
SkipAdClicker/              Qt application, icons, config, and sample SkipAd workflow
PlayTestBook/               Runtime for executing action/test-book scripts
TestBookLib/                Action parser, block actions, and flow controls
UserInterfaceLib/           Screenshot, UI detection, and Skip Ad detection helpers
OpencvLib/                  OpenCV wrappers and geometry/image utilities
OcrLib/                     OCR integration
Utilities/                  Shared types, logging, settings, and helpers
OpenCvMatBitmapVisualizer/  Optional Visual Studio debugger visualizer
```

## Prerequisites

- now Windows 10 or later
- Visual Studio 2026 or newer with C++ desktop development tools
- CMake 3.20+
- Qt, tested with `C:/Qt/6.11.0/msvc2022_64`
- vcpkg packages for OpenCV, Protobuf, and dependencies, tested with `x64-windows`
- Tesseract language data available at runtime if OCR is used

The checked-in `CMakePresets.json` assumes these local paths:

```text
C:/Tools/vcpkg
C:/Tools/vcpkg/installed/x64-windows
C:/Qt/6.11.0/msvc2022_64
```

Adjust `CMakePresets.json` if your machine uses different install locations.

## Build

From the repository root:

```powershell
cmake --preset x64-Debug
cmake --build --preset x64-Debug --target SkipAdClicker
```

The Debug executable is generated at:

```text
out/build/x64-Debug/x64/Debug/SkipAdClicker.exe
```

You can also configure a Visual Studio build folder directly:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Tools/vcpkg/installed/x64-windows;C:/Qt/6.11.0/msvc2022_64" -DOpenCV_DIR=C:/Tools/vcpkg/installed/x64-windows/share/opencv4 -DProtobuf_DIR=C:/Tools/vcpkg/installed/x64-windows/share/protobuf
cmake --build build --config Debug --target SkipAdClicker
```

That produces:

```text
build/x64/Debug/SkipAdClicker.exe
```

## Configuration

Runtime settings live in:

```text
SkipAdClicker/SkipAdClicker.ini
```

Important settings:

```ini
[General]
StartupFolder=.\SkipAd
TesseractEngineDataFolder=
TesseractEngineLanguage=eng
AllowOnlyOneInstance=true
DebugImagesLocation=\\Temp\\Logs\\Images

[Logging]
Enabled=true
Level=Info
File=\\Temp\\Logs\\SkipAdClicker.log
Console=false
```

The sample workflow starts at:

```text
SkipAdClicker/SkipAd/SkipAd.txt
```

## Sample Workflow

The bundled `SkipAd.txt` repeatedly takes a screenshot and runs the Skip Ad click action:

```text
$Endlessloop=StoreTrue
$SecondsDelay=StoreValue 0.5

WhileLoop $Endlessloop $SecondsDelay
    $screens=TakeFullScreenshot
    ClickOnSkipAd $screens Special_SettingLineDetectionSkipAd
```

## Notes

This project is intended for personal automation and testing workflows. Use it responsibly and follow the terms of service for any site or application you automate.

## License

MIT License. See `LICENSE`.
