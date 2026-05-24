using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace AutoTestWithAI.OpenCvMatBitmapVisualizer
{
    internal sealed class BitmapViewerWindow : Window
    {
        private readonly Image image;
        private readonly TextBlock status;
        private double zoom = 1.0;

        private BitmapViewerWindow(MatBitmapData data)
        {
            Title = "cv::Mat Bitmap Visualizer";
            Width = Math.Min(Math.Max(data.Width + 40, 520), 1200);
            Height = Math.Min(Math.Max(data.Height + 110, 420), 900);
            WindowStartupLocation = WindowStartupLocation.CenterOwner;
            Background = Brushes.White;

            DockPanel root = new DockPanel();
            Content = root;

            status = new TextBlock
            {
                Text = $"{data.SourceName}  |  {data.Width} x {data.Height}  |  channels {data.Channels}  |  stride {data.Stride}  |  0x{data.DataAddress:X}",
                Margin = new Thickness(8),
                TextWrapping = TextWrapping.Wrap
            };
            DockPanel.SetDock(status, Dock.Bottom);
            root.Children.Add(status);

            ToolBar toolbar = new ToolBar();
            DockPanel.SetDock(toolbar, Dock.Top);
            root.Children.Add(toolbar);

            Button fitButton = new Button { Content = "Fit", Margin = new Thickness(2), MinWidth = 52 };
            fitButton.Click += (_, __) => FitToWindow();
            toolbar.Items.Add(fitButton);

            Button actualButton = new Button { Content = "100%", Margin = new Thickness(2), MinWidth = 52 };
            actualButton.Click += (_, __) => SetZoom(1.0);
            toolbar.Items.Add(actualButton);

            Button saveButton = new Button { Content = "Save BMP", Margin = new Thickness(2), MinWidth = 72 };
            saveButton.Click += (_, __) => SaveBitmap(data);
            toolbar.Items.Add(saveButton);

            ScrollViewer scroll = new ScrollViewer
            {
                HorizontalScrollBarVisibility = ScrollBarVisibility.Auto,
                VerticalScrollBarVisibility = ScrollBarVisibility.Auto,
                Background = Brushes.DimGray
            };
            root.Children.Add(scroll);

            image = new Image
            {
                Source = CreateBitmapSource(data),
                Stretch = Stretch.None
            };
            RenderOptions.SetBitmapScalingMode(image, BitmapScalingMode.NearestNeighbor);
            scroll.Content = image;
            PreviewMouseWheel += OnPreviewMouseWheel;
        }

        public static void ShowModal(IntPtr ownerHwnd, MatBitmapData data)
        {
            BitmapViewerWindow window = new BitmapViewerWindow(data);
            if (ownerHwnd != IntPtr.Zero)
            {
                new WindowInteropHelper(window).Owner = ownerHwnd;
            }

            window.ShowDialog();
        }

        public static void ShowError(IntPtr ownerHwnd, Exception ex)
        {
            MessageBox.Show(ex.Message,
                "cv::Mat Bitmap Visualizer",
                MessageBoxButton.OK,
                MessageBoxImage.Error);
        }

        private static BitmapSource CreateBitmapSource(MatBitmapData data)
        {
            PixelFormat format;
            BitmapPalette palette = null;
            byte[] pixels = data.Pixels;

            if (data.Channels == 1)
            {
                format = PixelFormats.Gray8;
            }
            else if (data.Channels == 3)
            {
                format = PixelFormats.Bgr24;
            }
            else
            {
                format = PixelFormats.Bgra32;
            }

            BitmapSource source = BitmapSource.Create(
                data.Width,
                data.Height,
                96,
                96,
                format,
                palette,
                pixels,
                data.Stride);
            source.Freeze();
            return source;
        }

        private void OnPreviewMouseWheel(object sender, System.Windows.Input.MouseWheelEventArgs e)
        {
            if ((System.Windows.Input.Keyboard.Modifiers & System.Windows.Input.ModifierKeys.Control) == 0)
            {
                return;
            }

            e.Handled = true;
            SetZoom(e.Delta > 0 ? zoom * 1.2 : zoom / 1.2);
        }

        private void FitToWindow()
        {
            if (image.Source is BitmapSource bitmap)
            {
                double availableWidth = Math.Max(ActualWidth - 48, 100);
                double availableHeight = Math.Max(ActualHeight - 120, 100);
                SetZoom(Math.Min(availableWidth / bitmap.PixelWidth, availableHeight / bitmap.PixelHeight));
            }
        }

        private void SetZoom(double value)
        {
            zoom = Math.Max(0.05, Math.Min(value, 32.0));
            image.LayoutTransform = new ScaleTransform(zoom, zoom);
            status.Text = status.Text.Split(new[] { "  |  zoom " }, StringSplitOptions.None)[0] + $"  |  zoom {zoom:P0}";
        }

        private void SaveBitmap(MatBitmapData data)
        {
            string file = Path.Combine(Path.GetTempPath(), $"cv_mat_{DateTime.Now:yyyyMMdd_HHmmss}.bmp");
            using (FileStream stream = File.Create(file))
            {
                BmpBitmapEncoder encoder = new BmpBitmapEncoder();
                encoder.Frames.Add(BitmapFrame.Create(CreateBitmapSource(data)));
                encoder.Save(stream);
            }

            MessageBox.Show(this, file, "Saved bitmap", MessageBoxButton.OK, MessageBoxImage.Information);
        }
    }
}



