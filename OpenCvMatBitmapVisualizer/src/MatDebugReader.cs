using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text.RegularExpressions;
using Microsoft.VisualStudio.Debugger.Interop;

namespace AutoTestWithAI.OpenCvMatBitmapVisualizer
{
    internal sealed class MatBitmapData
    {
        public int Width { get; set; }
        public int Height { get; set; }
        public int Channels { get; set; }
        public int Depth { get; set; }
        public int Type { get; set; }
        public int Stride { get; set; }
        public ulong DataAddress { get; set; }
        public byte[] Pixels { get; set; } = Array.Empty<byte>();
        public string SourceName { get; set; } = "cv::Mat";
    }

    internal static class MatDebugReader
    {
        private const int MaxImageBytes = 512 * 1024 * 1024;

        public static MatBitmapData Read(IDebugProperty3 property)
        {
            if (property == null)
            {
                throw new ArgumentNullException(nameof(property));
            }

            string sourceName = GetPropertyName(property);
            IDebugMemoryBytes2 memoryBytes = TryGetMemoryBytes(property);
            IDebugMemoryContext2 contextForAddressing = TryGetMemoryContext(property);

            MatBitmapData data = null;
            if (memoryBytes != null && contextForAddressing != null)
            {
                byte[] matBytes = ReadMemory(memoryBytes, contextForAddressing, 128);
                data = TryParseMat(matBytes, 8, sourceName) ?? TryParseMat(matBytes, 4, sourceName);
            }

            if (data == null)
            {
                data = TryReadFromChildren(property, sourceName, out IDebugProperty3 dataProperty);
                memoryBytes = TryGetMemoryBytes(dataProperty) ?? memoryBytes;
                contextForAddressing = TryGetMemoryContext(dataProperty) ?? contextForAddressing;
            }

            if (data == null)
            {
                throw new InvalidOperationException(
                    "The selected value does not look like a supported OpenCV cv::Mat layout." +
                    Environment.NewLine +
                    BuildDebugSummary(property));
            }

            if (data.Depth != 0)
            {
                throw new NotSupportedException($"Only CV_8U mats can be displayed as a bitmap. This mat depth is {data.Depth}.");
            }

            if (data.Channels != 1 && data.Channels != 3 && data.Channels != 4)
            {
                throw new NotSupportedException($"Only 1, 3, or 4 channel mats are supported. This mat has {data.Channels} channels.");
            }

            long minimumStride = (long)data.Width * data.Channels;
            if (minimumStride <= 0 || minimumStride > int.MaxValue)
            {
                throw new InvalidOperationException($"Invalid computed bitmap stride: width={data.Width}, channels={data.Channels}.");
            }

            if (data.Stride < minimumStride || data.Stride > minimumStride * 16)
            {
                data.Stride = (int)minimumStride;
            }

            long byteCountLong = (long)data.Height * data.Stride;
            if (byteCountLong <= 0 || byteCountLong > MaxImageBytes)
            {
                throw new InvalidOperationException(
                    $"Refusing to read {byteCountLong:n0} bytes. " +
                    $"Computed bitmap size is outside the safety limit. " +
                    $"width={data.Width}, height={data.Height}, channels={data.Channels}, stride={data.Stride}.");
            }

            if (memoryBytes == null || contextForAddressing == null)
            {
                throw new InvalidOperationException("Visual Studio did not provide readable debuggee memory for this value or its data pointer.");
            }

            IDebugMemoryContext2 pixelContext = AddAddressDelta(contextForAddressing, GetContextAddress(contextForAddressing), data.DataAddress);
            data.Pixels = ReadMemory(memoryBytes, pixelContext, (uint)byteCountLong);
            return data;
        }

        private static MatBitmapData TryParseMat(byte[] bytes, int pointerSize, string sourceName)
        {
            if (bytes.Length < (pointerSize == 8 ? 96 : 56))
            {
                return null;
            }

            int flags = ReadInt32(bytes, 0);
            int rows = ReadInt32(bytes, 8);
            int cols = ReadInt32(bytes, 12);
            ulong data = ReadPointer(bytes, 16, pointerSize);
            ulong step = pointerSize == 8 ? ReadUInt64(bytes, 80) : ReadUInt32(bytes, 48);

            int depth = flags & 7;
            int channels = ((flags & 0x0FF8) >> 3) + 1;
            int type = flags & 0x0FFF;

            if (rows <= 0 || cols <= 0 || rows > 100000 || cols > 100000 || data == 0 || channels <= 0 || channels > 512)
            {
                return null;
            }

            if (step == 0 || step > int.MaxValue)
            {
                step = (ulong)(cols * Math.Max(channels, 1));
            }

            return new MatBitmapData
            {
                Width = cols,
                Height = rows,
                Channels = channels,
                Depth = depth,
                Type = type,
                Stride = (int)step,
                DataAddress = data,
                SourceName = sourceName
            };
        }

        private static MatBitmapData TryReadFromChildren(IDebugProperty3 property, string sourceName, out IDebugProperty3 dataProperty)
        {
            dataProperty = null;
            Dictionary<string, DEBUG_PROPERTY_INFO> children = GetChildren(property, 2);

            if (!TryGetInt(children, "flags", out int flags) ||
                !TryGetInt(children, "rows", out int rows) ||
                !TryGetInt(children, "cols", out int cols) ||
                !TryGetPointer(children, "data", out ulong dataAddress))
            {
                return null;
            }

            if (children.TryGetValue("data", out DEBUG_PROPERTY_INFO dataInfo))
            {
                dataProperty = dataInfo.pProperty as IDebugProperty3;
            }

            int depth = flags & 7;
            int channels = ((flags & 0x0FF8) >> 3) + 1;
            int type = flags & 0x0FFF;
            int stride = cols * Math.Max(channels, 1);

            if (TryGetInt(children, "step", out int stepValue))
            {
                stride = stepValue;
            }
            else if (children.TryGetValue("step", out DEBUG_PROPERTY_INFO stepInfo) && stepInfo.pProperty is IDebugProperty3 stepProperty)
            {
                Dictionary<string, DEBUG_PROPERTY_INFO> stepChildren = GetChildren(stepProperty, 2);
                if (TryGetInt(stepChildren, "p", out int pValue) ||
                    TryGetInt(stepChildren, "[0]", out pValue) ||
                    TryGetInt(stepChildren, "0", out pValue))
                {
                    stride = pValue;
                }
            }

            if (rows <= 0 || cols <= 0 || rows > 100000 || cols > 100000 || dataAddress == 0 || channels <= 0 || channels > 512)
            {
                return null;
            }

            int minimumStride = cols * Math.Max(channels, 1);
            if (stride < minimumStride || stride > minimumStride * 16)
            {
                stride = minimumStride;
            }

            return new MatBitmapData
            {
                Width = cols,
                Height = rows,
                Channels = channels,
                Depth = depth,
                Type = type,
                Stride = stride,
                DataAddress = dataAddress,
                SourceName = sourceName
            };
        }

        private static string GetPropertyName(IDebugProperty3 property)
        {
            try
            {
                DEBUG_PROPERTY_INFO[] info = new DEBUG_PROPERTY_INFO[1];
                property.GetPropertyInfo(
                    enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_NAME | enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_FULLNAME,
                    10,
                    0,
                    null,
                    0,
                    info);
                return !string.IsNullOrWhiteSpace(info[0].bstrFullName) ? info[0].bstrFullName : info[0].bstrName ?? "cv::Mat";
            }
            catch
            {
                return "cv::Mat";
            }
        }

        private static string BuildDebugSummary(IDebugProperty3 property)
        {
            DEBUG_PROPERTY_INFO info = GetPropertyInfo(property);
            Dictionary<string, DEBUG_PROPERTY_INFO> children = GetChildren(property, 2);

            List<string> lines = new List<string>
            {
                "Debugger value details:",
                $"Name: {info.bstrName ?? "(none)"}",
                $"FullName: {info.bstrFullName ?? "(none)"}",
                $"Type: {info.bstrType ?? "(none)"}",
                $"Value: {info.bstrValue ?? "(none)"}",
                $"HasMemoryBytes: {TryGetMemoryBytes(property) != null}",
                $"HasMemoryContext: {TryGetMemoryContext(property) != null}",
                $"Children found: {children.Count}"
            };

            int shown = 0;
            foreach (KeyValuePair<string, DEBUG_PROPERTY_INFO> child in children)
            {
                if (shown++ >= 30)
                {
                    lines.Add("...");
                    break;
                }

                lines.Add($"  {child.Key}: type='{child.Value.bstrType}', value='{child.Value.bstrValue}'");
            }

            return string.Join(Environment.NewLine, lines);
        }

        private static DEBUG_PROPERTY_INFO GetPropertyInfo(IDebugProperty3 property)
        {
            DEBUG_PROPERTY_INFO[] info = new DEBUG_PROPERTY_INFO[1];
            try
            {
                property.GetPropertyInfo(
                    enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_NAME |
                    enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_FULLNAME |
                    enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_TYPE |
                    enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_VALUE |
                    enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_ATTRIB |
                    enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_PROP |
                    enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_NOFUNCEVAL,
                    10,
                    0,
                    null,
                    0,
                    info);
            }
            catch
            {
                return default(DEBUG_PROPERTY_INFO);
            }

            return info[0];
        }

        private static Dictionary<string, DEBUG_PROPERTY_INFO> GetChildren(IDebugProperty3 property, int depth)
        {
            Dictionary<string, DEBUG_PROPERTY_INFO> children = new Dictionary<string, DEBUG_PROPERTY_INFO>(StringComparer.OrdinalIgnoreCase);
            AddChildren(property, children, depth);
            return children;
        }

        private static void AddChildren(IDebugProperty3 property, Dictionary<string, DEBUG_PROPERTY_INFO> children, int depth)
        {
            if (property == null)
            {
                return;
            }

            IEnumDebugPropertyInfo2 enumChildren = TryEnumChildren(property, enum_DBG_ATTRIB_FLAGS.DBG_ATTRIB_NONE)
                ?? TryEnumChildren(property, enum_DBG_ATTRIB_FLAGS.DBG_ATTRIB_ALL);

            if (enumChildren == null)
            {
                return;
            }

            DEBUG_PROPERTY_INFO[] item = new DEBUG_PROPERTY_INFO[1];
            while (enumChildren.Next(1, item, out uint fetched) == VSConstants.S_OK && fetched == 1)
            {
                string name = NormalizeName(item[0].bstrName);
                if (!string.IsNullOrWhiteSpace(name) && !children.ContainsKey(name))
                {
                    children.Add(name, item[0]);
                }

                if (depth > 0 && item[0].pProperty is IDebugProperty3 childProperty)
                {
                    AddChildren(childProperty, children, depth - 1);
                }

                item[0] = default(DEBUG_PROPERTY_INFO);
            }
        }

        private static IEnumDebugPropertyInfo2 TryEnumChildren(IDebugProperty3 property, enum_DBG_ATTRIB_FLAGS attribFilter)
        {
            Guid filter = Guid.Empty;
            int hr = property.EnumChildren(
                enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_NAME |
                enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_VALUE |
                enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_TYPE |
                enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_PROP |
                enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_ATTRIB |
                enum_DEBUGPROP_INFO_FLAGS.DEBUGPROP_INFO_NOFUNCEVAL,
                10,
                ref filter,
                attribFilter,
                null,
                0,
                out IEnumDebugPropertyInfo2 enumChildren);

            return hr == VSConstants.S_OK ? enumChildren : null;
        }

        private static bool TryGetInt(Dictionary<string, DEBUG_PROPERTY_INFO> children, string name, out int value)
        {
            value = 0;
            if (!children.TryGetValue(name, out DEBUG_PROPERTY_INFO info))
            {
                return false;
            }

            if (TryParseInteger(info.bstrValue, out long parsed))
            {
                if (parsed < int.MinValue || parsed > int.MaxValue)
                {
                    return false;
                }

                value = (int)parsed;
                return true;
            }

            return false;
        }

        private static bool TryGetPointer(Dictionary<string, DEBUG_PROPERTY_INFO> children, string name, out ulong value)
        {
            value = 0;
            return children.TryGetValue(name, out DEBUG_PROPERTY_INFO info) && TryParsePointer(info.bstrValue, out value);
        }

        private static string NormalizeName(string name)
        {
            if (string.IsNullOrWhiteSpace(name))
            {
                return name;
            }

            name = name.Trim();
            int lastDot = name.LastIndexOf('.');
            if (lastDot >= 0 && lastDot + 1 < name.Length)
            {
                name = name.Substring(lastDot + 1);
            }

            return name;
        }

        private static bool TryParseInteger(string text, out long value)
        {
            value = 0;
            if (string.IsNullOrWhiteSpace(text))
            {
                return false;
            }

            text = text.Trim();
            Match hex = Regex.Match(text, @"0x[0-9a-fA-F`']+");
            if (hex.Success)
            {
                string hexText = hex.Value.Substring(2).Replace("`", string.Empty).Replace("'", string.Empty);
                return long.TryParse(hexText, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
            }

            Match dec = Regex.Match(text, @"-?\d+");
            return dec.Success && long.TryParse(dec.Value, NumberStyles.Integer, CultureInfo.InvariantCulture, out value);
        }

        private static bool TryParsePointer(string text, out ulong value)
        {
            value = 0;
            if (string.IsNullOrWhiteSpace(text))
            {
                return false;
            }

            Match hex = Regex.Match(text, @"0x[0-9a-fA-F`']+");
            if (hex.Success)
            {
                string hexText = hex.Value.Substring(2).Replace("`", string.Empty).Replace("'", string.Empty);
                return ulong.TryParse(hexText, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out value);
            }

            Match dec = Regex.Match(text, @"\d+");
            return dec.Success && ulong.TryParse(dec.Value, NumberStyles.Integer, CultureInfo.InvariantCulture, out value);
        }

        private static IDebugMemoryBytes2 TryGetMemoryBytes(IDebugProperty3 property)
        {
            if (property == null || property.GetMemoryBytes(out IDebugMemoryBytes2 memoryBytes) != VSConstants.S_OK)
            {
                return null;
            }

            return memoryBytes;
        }

        private static IDebugMemoryContext2 TryGetMemoryContext(IDebugProperty3 property)
        {
            if (property == null || property.GetMemoryContext(out IDebugMemoryContext2 context) != VSConstants.S_OK)
            {
                return null;
            }

            return context;
        }

        private static byte[] ReadMemory(IDebugMemoryBytes2 memoryBytes, IDebugMemoryContext2 context, uint count)
        {
            byte[] buffer = new byte[checked((int)count)];
            uint read = 0;
            uint unreadable = 0;
            int hr = memoryBytes.ReadAt(context, count, buffer, out read, ref unreadable);
            if (hr != VSConstants.S_OK || read == 0)
            {
                throw new InvalidOperationException($"Unable to read debuggee memory. HRESULT=0x{hr:X8}.");
            }

            if (read < count)
            {
                Array.Resize(ref buffer, (int)read);
            }

            return buffer;
        }

        private static ulong GetContextAddress(IDebugMemoryContext2 context)
        {
            CONTEXT_INFO[] info = new CONTEXT_INFO[1];
            if (context.GetInfo(enum_CONTEXT_INFO_FIELDS.CIF_ADDRESSABSOLUTE | enum_CONTEXT_INFO_FIELDS.CIF_ADDRESS, info) != VSConstants.S_OK)
            {
                throw new InvalidOperationException("Unable to resolve the debuggee memory address.");
            }

            string address = info[0].bstrAddressAbsolute ?? info[0].bstrAddress;
            if (string.IsNullOrWhiteSpace(address))
            {
                throw new InvalidOperationException("Visual Studio returned an empty memory address.");
            }

            address = address.Trim();
            int hexIndex = address.IndexOf("0x", StringComparison.OrdinalIgnoreCase);
            if (hexIndex >= 0)
            {
                address = address.Substring(hexIndex + 2);
            }

            address = address.Replace("`", string.Empty).Replace("'", string.Empty);
            if (!ulong.TryParse(address, NumberStyles.HexNumber, CultureInfo.InvariantCulture, out ulong value))
            {
                throw new InvalidOperationException($"Unable to parse debuggee address '{address}'.");
            }

            return value;
        }

        private static IDebugMemoryContext2 AddAddressDelta(IDebugMemoryContext2 baseContext, ulong baseAddress, ulong targetAddress)
        {
            int hr;
            IDebugMemoryContext2 result;
            if (targetAddress >= baseAddress)
            {
                hr = baseContext.Add(targetAddress - baseAddress, out result);
            }
            else
            {
                hr = baseContext.Subtract(baseAddress - targetAddress, out result);
            }

            if (hr != VSConstants.S_OK || result == null)
            {
                throw new InvalidOperationException($"Unable to create a memory context for pixel data at 0x{targetAddress:X}.");
            }

            return result;
        }

        private static int ReadInt32(byte[] bytes, int offset)
        {
            return BitConverter.ToInt32(bytes, offset);
        }

        private static uint ReadUInt32(byte[] bytes, int offset)
        {
            return BitConverter.ToUInt32(bytes, offset);
        }

        private static ulong ReadUInt64(byte[] bytes, int offset)
        {
            return BitConverter.ToUInt64(bytes, offset);
        }

        private static ulong ReadPointer(byte[] bytes, int offset, int pointerSize)
        {
            return pointerSize == 8 ? ReadUInt64(bytes, offset) : ReadUInt32(bytes, offset);
        }
    }
}
