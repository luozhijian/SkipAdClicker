#ifndef AUTOMATIOTEST_OPENCVLIB_SMALLTRIANGLEDETECTOR_HPP
#define AUTOMATIOTEST_OPENCVLIB_SMALLTRIANGLEDETECTOR_HPP

#include "../Utilities/CommonTypes.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <vector>

#include "../Utilities/TriangleWithDescription.hpp"


namespace automationtest::opencvlib {

class SmallTriangleDetector {
public:
	static std::vector<automationtest::utilities::TriangleWithDescription> FindSmallTriangles(const cv::Mat& binary);

private:
 
};

} // namespace automationtest::opencvlib

#endif // AUTOMATIOTEST_OPENCVLIB_SMALLTRIANGLEDETECTOR_HPP
