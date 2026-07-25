#include "SmallTriangleDetector.hpp"

#include "TriangleDetector.hpp"



using automationtest::utilities::TriangleWithDescription;
using automationtest::utilities::Point;

namespace automationtest::opencvlib {

using automationtest::utilities::Bitmap;
using automationtest::utilities::Rectangle;

  


//std::vector<Rectangle> SmallTriangleDetector::FindSmallTriangles() const
//{
//    std::vector<Rectangle> results {};
//    const auto triangles = TriangleDetector(bitmap_).FindTriangles({});
//    results.reserve(triangles.size());
//    for (const auto& triangle : triangles) {
//        const auto rect = triangle.GetCoveredRectangle();
//        if (rect.width >= 5 && rect.height >= 5 && rect.width <= 33 && rect.height <= 33) {
//            results.push_back(rect);
//        }
//    }
//    return results;
//}

std::vector<TriangleWithDescription> SmallTriangleDetector::FindSmallTriangles(const cv::Mat& binary) 
{
    std::vector<TriangleWithDescription> results{};

    if (binary.type() != CV_8UC1) 
        return results;


    cv::Mat labels, stats, centroids;

    // Run connected components with stats (8-way connectivity)
    // Note: CV_32S is required for the labels matrix
    int totalLabels = cv::connectedComponentsWithStats(binary, labels, stats, centroids, 8, CV_32S);

    int objectCount = totalLabels - 1;
    int countReal = 0;
    std::vector<TriangleWithDescription> resultRectangle;

    // Loop through each labeled object (skip index 0, which is background)
    for (int i = 1; i < totalLabels; ++i) {
        // Extract structural metrics from stats matrix
        int x = stats.at<int>(i, cv::CC_STAT_LEFT);
        int y = stats.at<int>(i, cv::CC_STAT_TOP);
        int w = stats.at<int>(i, cv::CC_STAT_WIDTH);
        int h = stats.at<int>(i, cv::CC_STAT_HEIGHT);
        int area = stats.at<int>(i, cv::CC_STAT_AREA);

        // Extract center coordinates from centroids matrix (always CV_64F / double)
        double centerX = centroids.at<double>(i, 0);
        double centerY = centroids.at<double>(i, 1);

        // Filter 1: Area limits
        if (!(area > 100 && area < 450)) continue;

        // Filter 2: Aspect ratio limits
        double aspectRatio = (w * 1.0) / h;
        if (!(aspectRatio > 0.8 && aspectRatio < 1.1)) continue;

        // Filter 3: Density limits
        double density = (area * 1.0) / w / h;
        if (!(density > 0.35 && density < 0.64)) continue;

        // Filter 4: Y center geometric constraint
        if (std::abs((h / 2.0) + y - centerY) > 2.0) continue;

        // Filter 5: X center geometric constraint
        if (std::abs((w / 3.0) + x - centerX) > 2.0) continue;

        // Profile the distribution of labeled pixels across columns
        std::vector<int> listOfCount;
        listOfCount.reserve(w); // Optimize allocation

        for (int xx = x; xx < x + w; ++xx) {
            int tempCount = 0;
            for (int yy = y; yy < y + h; ++yy) {
                // Access individual element inside the 32-bit signed labels matrix
                if (labels.at<int>(yy, xx) == i) {
                    tempCount++;
                }
            }
            listOfCount.push_back(tempCount);
        }

 
        // Filter 6: Index-based offset validation
        // Note: Python's list_of_count[-1] gets the last element
        int lastElement = listOfCount.back();

        // if first column has much more less point, we think it is some noise, make that triangle smaller
        if (listOfCount[0] < h - 5)
        {
            listOfCount.erase(listOfCount.begin());
            x += 1;
            w -= 1;
        }

        int prev_count= h; 
        int count_not_descending = 0;
        for (int current_count : listOfCount) {
            if (prev_count - current_count < 0)
                count_not_descending += 1;
            prev_count = current_count;
        }
        // triangle should be in descending order, if two many not, 
        if (count_not_descending >= 3)
            continue;
        
        // if the triangle is not sharp enough, it should be close to 1 
        if (prev_count > 3)
            continue;

        // If all conditions clear, track valid target
        countReal++;
 
        resultRectangle.push_back(TriangleWithDescription(Point(x, y), Point(x, y + h - 1), Point(x + w - 1, y + h / 2))); 
    }
    
    return resultRectangle;
}


} // namespace automationtest::opencvlib
