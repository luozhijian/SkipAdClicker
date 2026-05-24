# OpenCV `cv::Mat` Bitmap Visual Studio Debugger Visualizer

This is a full Visual Studio C++ debugger UI visualizer for native OpenCV `cv::Mat` values.

It adds a QuickWatch/Watch visualizer entry named **OpenCV cv::Mat Bitmap** and renders supported mats as a real bitmap in a WPF popup window.

## Supported Mats

- `CV_8UC1`: gray bitmap
- `CV_8UC3`: BGR bitmap
- `CV_8UC4`: BGRA bitmap

The visualizer reads the debuggee's `cv::Mat` memory directly. It does not call OpenCV code inside the debuggee process.

## Build

```powershell
cd C:\Luo\Repos\AutomationTest\OpenCvMatBitmapVisualizer
.\build-vsix.ps1
```

This creates:

```text
C:\Luo\Repos\AutomationTest\OpenCvMatBitmapVisualizer\OpenCvMatBitmapVisualizer.vsix
```

## Install

Close Visual Studio, then run:

```powershell
cd C:\Luo\Repos\AutomationTest\OpenCvMatBitmapVisualizer
.\install.ps1
```

Or double-click:

```text
C:\Luo\Repos\AutomationTest\OpenCvMatBitmapVisualizer\OpenCvMatBitmapVisualizer.vsix
```

Restart Visual Studio after installation.

## Use

1. Debug a native C++ program that has a `cv::Mat` variable.
2. Stop at a breakpoint.
3. Open QuickWatch or Watch for the `cv::Mat` variable.
4. Use the visualizer dropdown/magnifying-glass entry **OpenCV cv::Mat Bitmap**.

The popup supports Fit, 100%, Ctrl+mouse-wheel zoom, and Save BMP.

## Files

- `OpenCvMatBitmap.natvis`: declares the C++ debugger UI visualizer entry for `cv::Mat`.
- `OpenCvMatBitmapVisualizer.dll`: compiled Visual Studio package and UI visualizer service.
- `OpenCvMatBitmapVisualizer.pkgdef`: registers the Visual Studio package and service GUID.
- `OpenCvMatBitmapVisualizer.vsix`: installable extension package.
- `src/`: C# source for the package, debug memory reader, and WPF viewer.
