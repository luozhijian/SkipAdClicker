#include "BitmapHelper.hpp"

#include "GlobalSetting.hpp"
#include "IdGenerator.hpp"
#include "Logger.hpp"
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace automationtest::utilities {

namespace {

int ResolveChannels(const Bitmap& bitmap) noexcept
{
    if (bitmap.channels > 0) {
        return bitmap.channels;
    }
    if (bitmap.width <= 0) {
        return 1;
    }
    const int guessed = bitmap.stride / bitmap.width;
    return guessed > 0 ? guessed : 1;
}

void WriteUInt16(std::ofstream& output, std::uint16_t value)
{
    output.put(static_cast<char>(value & 0xFF));
    output.put(static_cast<char>((value >> 8) & 0xFF));
}

void WriteUInt32(std::ofstream& output, std::uint32_t value)
{
    output.put(static_cast<char>(value & 0xFF));
    output.put(static_cast<char>((value >> 8) & 0xFF));
    output.put(static_cast<char>((value >> 16) & 0xFF));
    output.put(static_cast<char>((value >> 24) & 0xFF));
}

void WriteInt32(std::ofstream& output, std::int32_t value)
{
    WriteUInt32(output, static_cast<std::uint32_t>(value));
}

bool WriteBitmapFile(const Bitmap& bitmap, const std::filesystem::path& output_path)
{
    const int channels = ResolveChannels(bitmap);
    if (bitmap.width <= 0 || bitmap.height <= 0 || channels <= 0 || bitmap.stride <= 0 || bitmap.pixels.empty()) {
        return false;
    }

    const std::size_t required_size = static_cast<std::size_t>(bitmap.stride) * static_cast<std::size_t>(bitmap.height);
    if (bitmap.pixels.size() < required_size) {
        return false;
    }

    constexpr std::uint16_t bits_per_pixel = 24;
    constexpr std::uint32_t file_header_size = 14;
    constexpr std::uint32_t dib_header_size = 40;
    const std::uint32_t row_size = static_cast<std::uint32_t>(((bitmap.width * 3) + 3) & ~3);
    const std::uint32_t pixel_data_size = row_size * static_cast<std::uint32_t>(bitmap.height);
    const std::uint32_t file_size = file_header_size + dib_header_size + pixel_data_size;

    std::ofstream output(output_path, std::ios::binary);
    if (!output) {
        return false;
    }

    output.put('B');
    output.put('M');
    WriteUInt32(output, file_size);
    WriteUInt16(output, 0);
    WriteUInt16(output, 0);
    WriteUInt32(output, file_header_size + dib_header_size);

    WriteUInt32(output, dib_header_size);
    WriteInt32(output, bitmap.width);
    WriteInt32(output, -bitmap.height);
    WriteUInt16(output, 1);
    WriteUInt16(output, bits_per_pixel);
    WriteUInt32(output, 0);
    WriteUInt32(output, pixel_data_size);
    WriteInt32(output, 2835);
    WriteInt32(output, 2835);
    WriteUInt32(output, 0);
    WriteUInt32(output, 0);

    std::vector<unsigned char> row(row_size, 0);
    for (int y = 0; y < bitmap.height; ++y) {
        std::fill(row.begin(), row.end(), 0);
        const auto row_start = static_cast<std::size_t>(y) * static_cast<std::size_t>(bitmap.stride);
        for (int x = 0; x < bitmap.width; ++x) {
            const auto source_index = row_start + static_cast<std::size_t>(x) * static_cast<std::size_t>(channels);
            if (source_index >= bitmap.pixels.size()) {
                return false;
            }

            unsigned char red = 0;
            unsigned char green = 0;
            unsigned char blue = 0;
            if (channels == 1) {
                red = green = blue = static_cast<unsigned char>(bitmap.pixels[source_index]);
            } else {
                red = static_cast<unsigned char>(bitmap.pixels[source_index]);
                green = source_index + 1 < bitmap.pixels.size() ? static_cast<unsigned char>(bitmap.pixels[source_index + 1]) : red;
                blue = source_index + 2 < bitmap.pixels.size() ? static_cast<unsigned char>(bitmap.pixels[source_index + 2]) : red;
            }

            const auto target_index = static_cast<std::size_t>(x) * 3;
            row[target_index] = blue;
            row[target_index + 1] = green;
            row[target_index + 2] = red;
        }
        output.write(reinterpret_cast<const char*>(row.data()), static_cast<std::streamsize>(row.size()));
    }

    return output.good();
}
}

Bitmap BitmapHelper::ResizeImage(const Bitmap& bitmap, const Size& size)
{
    if (bitmap.width <= 0 || bitmap.height <= 0 || size.width <= 0 || size.height <= 0) {
        return Bitmap {};
    }

    const int channels = ResolveChannels(bitmap);
    Bitmap result;
    result.width = size.width;
    result.height = size.height;
    result.channels = channels;
    result.stride = size.width * channels;
    result.pixels.resize(static_cast<std::size_t>(result.stride * result.height));

    for (int y = 0; y < result.height; ++y) {
        const int source_y = y * bitmap.height / result.height;
        for (int x = 0; x < result.width; ++x) {
            const int source_x = x * bitmap.width / result.width;
            const int source_index = source_y * bitmap.stride + source_x * channels;
            const int target_index = y * result.stride + x * channels;
            for (int channel = 0; channel < channels; ++channel) {
                result.pixels[static_cast<std::size_t>(target_index + channel)] =
                    bitmap.pixels[static_cast<std::size_t>(source_index + channel)];
            }
        }
    }

    return result;
}

Bitmap BitmapHelper::CreateGrayscaleBitmap(const std::vector<std::byte>& pixel_data, int width, int height, int stride)
{
    return Bitmap {width, height, stride, pixel_data, 1};
}

Bitmap BitmapHelper::ConvertToGrayscale(const Bitmap& bitmap)
{
    if (bitmap.width <= 0 || bitmap.height <= 0) {
        return Bitmap {};
    }

    const int channels = ResolveChannels(bitmap);
    if (channels == 1) {
        return bitmap;
    }

    Bitmap result;
    result.width = bitmap.width;
    result.height = bitmap.height;
    result.channels = 1;
    result.stride = bitmap.width;
    result.pixels.resize(static_cast<std::size_t>(result.stride * result.height));

    for (int y = 0; y < bitmap.height; ++y) {
        for (int x = 0; x < bitmap.width; ++x) {
            const int source_index = y * bitmap.stride + x * channels;
            const auto r = static_cast<unsigned char>(bitmap.pixels[static_cast<std::size_t>(source_index)]);
            const auto g = channels > 1 ? static_cast<unsigned char>(bitmap.pixels[static_cast<std::size_t>(source_index + 1)]) : r;
            const auto b = channels > 2 ? static_cast<unsigned char>(bitmap.pixels[static_cast<std::size_t>(source_index + 2)]) : r;
            const auto gray = static_cast<unsigned char>(0.299 * r + 0.587 * g + 0.114 * b);
            result.pixels[static_cast<std::size_t>(y * result.stride + x)] = static_cast<std::byte>(gray);
        }
    }

    return result;
}

std::string BitmapHelper::InfoSave(const Bitmap& bitmap)
{
    if (!GlobalSetting::save_bitmap_files && ! Logger::IsAboveInfo() ) {
        return {};
    }

    std::string filename = "bitmap" + IdGenerator::IdWithDateTime() + ".bmp";
    return DebugSave(bitmap, filename);
}

std::string BitmapHelper::DebugSave(const Bitmap& bitmap )
{
    if (!GlobalSetting::save_bitmap_files && !Logger::IsAboveDebug() ) {
        return {};
    }
    std::string filename= "bitmap" + IdGenerator::IdWithDateTime() + ".bmp";
	return DebugSave(bitmap, filename);
}

std::string BitmapHelper::DebugSave(const Bitmap& bitmap, const std::string& filename)
{
    if (!GlobalSetting::save_bitmap_files && !Logger::IsAboveDebug()) {
        return {};
    }
    if (filename.empty())
        return {};

    std::filesystem::path output_path(GlobalSetting::ImageFileFolder());
    output_path /= filename;
    output_path.replace_extension(".bmp");

    if (const auto parent_path = output_path.parent_path(); !parent_path.empty()) {
        std::filesystem::create_directories(parent_path);
    }

    if (!WriteBitmapFile(bitmap, output_path)) {
        return {};
    }
    return output_path.string();
}


Bitmap BitmapHelper::CopyBitmap(const Bitmap& bitmap, const std::optional<Rectangle>& rectangle, Point& top_left)
{
    top_left = Point {};
    if (!rectangle.has_value()) {
        return bitmap;
    }
    top_left = Point {rectangle->Left(), rectangle->Top()};
    return GetBitmap(bitmap, *rectangle);
}

Bitmap BitmapHelper::GetBitmap(const Bitmap& bitmap, Rectangle rectangle)
{
    if (rectangle.IsEmpty()) {
        return bitmap;
    }

    const int channels = ResolveChannels(bitmap);
    const int left = std::max(0, rectangle.Left());
    const int top = std::max(0, rectangle.Top());
    const int right = std::min(bitmap.width, rectangle.Right());
    const int bottom = std::min(bitmap.height, rectangle.Bottom());
    if (right <= left || bottom <= top) {
        return bitmap;
    }

    Bitmap result;
    result.width = right - left;
    result.height = bottom - top;
    result.channels = channels;
    result.stride = result.width * channels;
    result.pixels.resize(static_cast<std::size_t>(result.stride * result.height));

    for (int y = 0; y < result.height; ++y) {
        const int source_index = (top + y) * bitmap.stride + left * channels;
        const int target_index = y * result.stride;
        for (int offset = 0; offset < result.stride; ++offset) {
            result.pixels[static_cast<std::size_t>(target_index + offset)] =
                bitmap.pixels[static_cast<std::size_t>(source_index + offset)];
        }
    }

    return result;
}

} // namespace automationtest::utilities
