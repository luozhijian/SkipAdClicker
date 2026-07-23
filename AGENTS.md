# Repository Guidelines

## Project Structure & Module Organization

SkipAdClicker is a C++23 desktop application built with CMake. `SkipAdClicker/` contains the Qt entry point, UI, resources, and the configurable `SkipAd/` workflow. Shared functionality is split by responsibility: `Utilities/` for common services and types, `OpencvLib/` for image processing, `OcrLib/` for Tesseract integration, `UserInterfaceLib/` for screen/control detection, and `TestBookLib/` plus `PlayTestBook/` for parsing and executing workflows. Packaging lives in `snap/` and `WindowsScript/`; the optional Visual Studio debugger extension is under `OpenCvMatBitmapVisualizer/`.

Keep source/header pairs together as `Name.cpp` and `Name.hpp`. Do not commit generated content from `build/`, `out/`, or `.vs/`.

## Build, Test, and Development Commands

- `cmake --preset x64-Debug` configures the Windows debug build. Update local Qt/vcpkg paths in `CMakePresets.json` when necessary.
- `cmake --build --preset x64-Debug --target SkipAdClicker` builds the Windows executable.
- `bash scripts/ubuntu-install-deps.sh` installs Ubuntu build dependencies.
- `bash scripts/ubuntu-build.sh` configures and builds the Ubuntu release target.
- `./out/build/ubuntu-release/bin/SkipAdClicker` runs the Linux build.

See `README.md` for macOS commands and platform permissions.

## Coding Style & Naming Conventions

Follow the existing C++ style: four-space indentation, braces on the next line for functions, and scoped namespaces such as `automationtest::utilities`. Use PascalCase for classes and functions, snake_case with a trailing underscore for private data members, and uppercase include guards. Prefer Qt platform macros where Qt APIs are involved and isolate OS-specific code with preprocessor guards. No formatter is configured, so match neighboring code and keep includes grouped logically.

## Testing Guidelines

There is currently no CTest or unit-test target. Every change must at least compile on its affected platform. For detector changes, exercise representative fixtures in `SkipAdClicker/SkipAd/TestCase/Positive/` and `Negative/`, and add a clearly named image when fixing a visual regression. Manually verify tray behavior, detection start/stop, logging, and platform-specific screenshot/click behavior when relevant.

## Commit & Pull Request Guidelines

Recent history uses short, imperative, lowercase subjects such as `fix UI for windows` and `improve readme`. Keep each commit focused and state the observable change. Pull requests should explain the problem and solution, list tested platforms and commands, link related issues, and include screenshots for UI or detection changes. Call out changes to workflow JSON, `.ini` defaults, packaging, or permissions. Never commit credentials, private signing keys, local certificates, logs, or machine-specific paths.
