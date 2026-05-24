using System;
using System.ComponentModel.Design;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell;

namespace AutoTestWithAI.OpenCvMatBitmapVisualizer
{
    [PackageRegistration(UseManagedResourcesOnly = true, AllowsBackgroundLoading = false)]
    [Guid(PackageGuidString)]
    public sealed class OpenCvMatBitmapVisualizerPackage : Package
    {
        public const string PackageGuidString = "F76EC0C6-4C70-491C-91F8-AD66215B183D";

        protected override void Initialize()
        {
            base.Initialize();
            ((IServiceContainer)this).AddService(
                typeof(IOpenCvMatBitmapVisualizerService),
                (_, __) => new OpenCvMatBitmapVisualizerService(),
                promote: true);
        }
    }
}
