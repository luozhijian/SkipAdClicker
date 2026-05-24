#include "GridDetector.hpp"

#include "GridDetectorUtility.hpp"

#include "../../OpencvLib/LineDetector.hpp"
#include "../../Utilities/BitmapHelper.hpp"
#include "../../Utilities/ByteArrayLib.hpp"
#include "../../Utilities/RectangleHelper.hpp"
#include "../../Utilities/Types/IntPair.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace automationtest::userinterfacelib {

using automationtest::opencvlib::LineDetector;
using automationtest::utilities::BitmapHelper;
using automationtest::utilities::RectangleHelper;
using automationtest::utilities::types::IntPair;

namespace {

std::vector<IntPair> GroupCoordinates(const std::vector<int>& coordinates, int max_gap = 2)
{
    std::vector<IntPair> groups {};
    if (coordinates.empty()) {
        return groups;
    }

    auto sorted = coordinates;
    std::sort(sorted.begin(), sorted.end());

    int start = sorted.front();
    int previous = sorted.front();
    for (std::size_t index = 1; index < sorted.size(); ++index) {
        if (sorted[index] - previous <= max_gap) {
            previous = sorted[index];
            continue;
        }

        groups.emplace_back(start, previous);
        start = previous = sorted[index];
    }
    groups.emplace_back(start, previous);
    return groups;
}

std::vector<IntPair> PairWithNext(const std::vector<int>& values)
{
    std::vector<IntPair> result {};
    for (std::size_t index = 1; index < values.size(); ++index) {
        result.emplace_back(values[index - 1], values[index]);
    }
    return result;
}

std::vector<IntPair> NormalizeRows(const std::vector<IntPair>& rows)
{
    if (rows.empty()) {
        return {};
    }

    std::vector<int> lengths {};
    for (const auto& row : rows) {
        if (row.Length() > 2) {
            lengths.push_back(row.Length());
        }
    }
    if (lengths.empty()) {
        return rows;
    }

    std::sort(lengths.begin(), lengths.end());
    const auto expected = lengths[lengths.size() / 2];

    std::vector<IntPair> normalized {};
    for (const auto& row : rows) {
        if (expected > 0 && row.Length() >= expected * 2 - 2 && row.Length() <= expected * 3 + 2) {
            const auto pieces = std::max(1, static_cast<int>(std::lround(static_cast<double>(row.Length()) / static_cast<double>(expected))));
            const auto step = std::max(1, row.Length() / pieces);
            int current = row.First();
            for (int piece = 0; piece < pieces; ++piece) {
                const auto end = (piece == pieces - 1) ? row.Second() : (current + step);
                normalized.emplace_back(current, end);
                current = end;
            }
            continue;
        }
        normalized.push_back(row);
    }
    return normalized;
}

} // namespace

GridDetector::GridDetector(Bitmap bitmap, Rectangle rect)
    : bitmap_(std::move(bitmap)),
      rect_(rect)
{
}

GridDetector GridDetector::FindGrid(Bitmap bitmap, Rectangle rect, const std::string& grid_style, int threshold_x, int threshold_y)
{
    (void)grid_style;
    GridDetector detector(std::move(bitmap), rect);
    detector.grid_detector_resource_ = GridDetectorUtility::LoadDefault();
    detector.Detector2(threshold_x, threshold_y);
    return detector;
}

GridLayout GridDetector::Detector2(int threshold_x, int threshold_y, const TextProvider& provider) const
{
    auto clipped = BitmapHelper::GetBitmap(bitmap_, rect_);
    auto gray = BitmapHelper::ConvertToGrayscale(clipped);
    const LineDetector line_detector(gray);

    std::vector<int> horizontal_coordinates {};
    for (const auto& line : line_detector.FindAllHorizonLines(threshold_x, false)) {
        horizontal_coordinates.push_back(line.Point1().y);
    }

    std::vector<int> vertical_coordinates {};
    for (const auto& line : line_detector.FindAllVerticalLines(threshold_y, false)) {
        vertical_coordinates.push_back(line.Point1().x);
    }

    auto horizontal_groups = GroupCoordinates(horizontal_coordinates);
    auto vertical_groups = GroupCoordinates(vertical_coordinates);

    std::vector<int> row_boundaries {};
    row_boundaries.reserve(horizontal_groups.size());
    for (const auto& group : horizontal_groups) {
        row_boundaries.push_back((group.First() + group.Second()) / 2);
    }

    std::vector<int> column_boundaries {};
    column_boundaries.reserve(vertical_groups.size());
    for (const auto& group : vertical_groups) {
        column_boundaries.push_back((group.First() + group.Second()) / 2);
    }

    auto row_pairs = NormalizeRows(PairWithNext(row_boundaries));
    auto column_pairs = PairWithNext(column_boundaries);
    return ReadGrid(clipped, row_pairs, column_pairs, provider);
}

GridLayout GridDetector::ReadGrid(const Bitmap& clipped_bitmap, const std::vector<IntPair>& row_pairs, const std::vector<IntPair>& column_pairs, const TextProvider& provider) const
{
    GridLayout result {};
    result.grid_location = rect_;
    result.bitmap = clipped_bitmap;
    result.rows = row_pairs;
    result.columns = column_pairs;

    if (result.rows.empty() || result.columns.empty()) {
        return result;
    }

    result.data_table_wrapper.AddColumn("xxxxIndexxxxx", 0);
    for (std::size_t column_index = 0; column_index < result.columns.size(); ++column_index) {
        const auto cell = result.GetOneCell(0, static_cast<int>(column_index));
        const auto value = provider ? provider(BitmapHelper::GetBitmap(result.bitmap, cell)) : std::string {};
        result.headers.push_back(value);
        result.data_table_wrapper.AddColumn(value.empty() ? ("Column" + std::to_string(column_index)) : value, static_cast<int>(column_index) + 1);
    }

    for (std::size_t row_index = 1; row_index < result.rows.size(); ++row_index) {
        result.data_table_wrapper.GetNewRow();
        result.data_table_wrapper.AddIntToCurrentRow(0, static_cast<int>(row_index));
        for (std::size_t column_index = 0; column_index < result.columns.size(); ++column_index) {
            const auto cell = result.GetOneCell(static_cast<int>(row_index), static_cast<int>(column_index));
            const auto value = provider ? provider(BitmapHelper::GetBitmap(result.bitmap, cell)) : std::string {};
            result.data_table_wrapper.AddDataToCurrentRow(static_cast<int>(column_index) + 1, value);
        }
    }

    return result;
}

} // namespace automationtest::userinterfacelib
