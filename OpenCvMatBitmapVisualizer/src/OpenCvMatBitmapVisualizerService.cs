using System;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Debugger.Interop;

namespace AutoTestWithAI.OpenCvMatBitmapVisualizer
{
    [Guid(VisualizerGuids.ServiceGuidString)]
    public interface IOpenCvMatBitmapVisualizerService
    {
    }

    internal static class VisualizerGuids
    {
        public const string ServiceGuidString = "73D1AA4A-926B-41A9-9E54-D18FF81C6B22";
    }

    [Guid(VisualizerGuids.ServiceGuidString)]
    public sealed class OpenCvMatBitmapVisualizerService : IOpenCvMatBitmapVisualizerService, IVsCppDebugUIVisualizer
    {
        public int DisplayValue(uint ownerHwnd, uint visualizerId, IDebugProperty3 debugProperty)
        {
            try
            {
                MatBitmapData data = MatDebugReader.Read(debugProperty);
                BitmapViewerWindow.ShowModal(new IntPtr(ownerHwnd), data);
                return VSConstants.S_OK;
            }
            catch (Exception ex)
            {
                BitmapViewerWindow.ShowError(new IntPtr(ownerHwnd), new InvalidOperationException(ex.ToString(), ex));
                return VSConstants.E_FAIL;
            }
        }
    }

    internal static class VSConstants
    {
        public const int S_OK = 0;
        public const int E_FAIL = unchecked((int)0x80004005);
    }
}
