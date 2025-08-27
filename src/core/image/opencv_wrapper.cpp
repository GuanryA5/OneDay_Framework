#include "opencv_wrapper.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "../common/logger.h"

using oneday::core::Logger;

namespace oneday::image {

OpenCVWrapper::OpenCVWrapper() {
    Logger::info("OpenCV Wrapper initialized");
    Logger::info("OpenCV Version: " + std::string(CV_VERSION));
}

OpenCVWrapper::~OpenCVWrapper() {
    Logger::info("OpenCV Wrapper destroyed");
}

cv::Mat OpenCVWrapper::loadImage(const std::string& filename, int flags) {
    cv::Mat image = cv::imread(filename, flags);

    if (image.empty()) {
        Logger::error("Failed to load image: " + filename);
    } else {
        Logger::info("Image loaded successfully: " + filename + " (" + std::to_string(image.cols) +
                     "x" + std::to_string(image.rows) + ")");
    }

    return image;
}

bool OpenCVWrapper::saveImage(const cv::Mat& image, const std::string& filename) {
    if (image.empty()) {
        Logger::error("Cannot save empty image");
        return false;
    }

    bool success = cv::imwrite(filename, image);
    if (success) {
        Logger::info("Image saved successfully: " + filename);
    } else {
        Logger::error("Failed to save image: " + filename);
    }

    return success;
}

cv::Mat OpenCVWrapper::resize(const cv::Mat& src, const cv::Size& dsize, int interpolation) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;
    cv::resize(src, dst, dsize, 0, 0, interpolation);

    Logger::debug("Image resized from " + std::to_string(src.cols) + "x" +
                  std::to_string(src.rows) + " to " + std::to_string(dst.cols) + "x" +
                  std::to_string(dst.rows));

    return dst;
}

cv::Mat OpenCVWrapper::convertColor(const cv::Mat& src, int code) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;
    cv::cvtColor(src, dst, code);

    Logger::debug("Color conversion applied, code: " + std::to_string(code));
    return dst;
}

cv::Mat OpenCVWrapper::gaussianBlur(const cv::Mat& src,
                                    const cv::Size& ksize,
                                    double sigmaX,
                                    double sigmaY) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;
    cv::GaussianBlur(src, dst, ksize, sigmaX, sigmaY);

    Logger::debug("Gaussian blur applied");
    return dst;
}

cv::Mat OpenCVWrapper::cannyEdgeDetection(const cv::Mat& src,
                                          double threshold1,
                                          double threshold2,
                                          int apertureSize) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat gray, edges;

    // Convert to grayscale if needed
    if (src.channels() > 1) {
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = src;
    }

    cv::Canny(gray, edges, threshold1, threshold2, apertureSize);

    Logger::debug("Canny edge detection completed");
    return edges;
}

std::vector<cv::Vec3f> OpenCVWrapper::houghCircles(const cv::Mat& src,
                                                   int method,
                                                   double dp,
                                                   double minDist,
                                                   double param1,
                                                   double param2,
                                                   int minRadius,
                                                   int maxRadius) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return std::vector<cv::Vec3f>();
    }

    cv::Mat gray;
    if (src.channels() > 1) {
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = src;
    }

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(gray, circles, method, dp, minDist, param1, param2, minRadius, maxRadius);

    Logger::info("Detected " + std::to_string(circles.size()) + " circles");
    return circles;
}

std::vector<cv::Vec4i> OpenCVWrapper::houghLines(const cv::Mat& src,
                                                 double rho,
                                                 double theta,
                                                 int threshold,
                                                 double srn,
                                                 double stn,
                                                 double min_theta,
                                                 double max_theta) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return std::vector<cv::Vec4i>();
    }

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(src, lines, rho, theta, threshold, 50, 10);

    Logger::info("Detected " + std::to_string(lines.size()) + " lines");
    return lines;
}

std::vector<std::vector<cv::Point>> OpenCVWrapper::findContours(const cv::Mat& src,
                                                                int mode,
                                                                int method) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return std::vector<std::vector<cv::Point>>();
    }

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(src, contours, hierarchy, mode, method);

    Logger::info("Found " + std::to_string(contours.size()) + " contours");
    return contours;
}

cv::Mat OpenCVWrapper::morphologyEx(const cv::Mat& src,
                                    int op,
                                    const cv::Mat& kernel,
                                    int iterations) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;
    cv::morphologyEx(src, dst, op, kernel, cv::Point(-1, -1), iterations);

    Logger::debug("Morphology operation applied");
    return dst;
}

cv::Mat OpenCVWrapper::threshold(const cv::Mat& src, double thresh, double maxval, int type) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;
    cv::threshold(src, dst, thresh, maxval, type);

    Logger::debug("Threshold applied: " + std::to_string(thresh));
    return dst;
}

cv::Mat OpenCVWrapper::adaptiveThreshold(const cv::Mat& src,
                                         double maxValue,
                                         int adaptiveMethod,
                                         int thresholdType,
                                         int blockSize,
                                         double C) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;
    cv::adaptiveThreshold(src, dst, maxValue, adaptiveMethod, thresholdType, blockSize, C);

    Logger::debug("Adaptive threshold applied");
    return dst;
}

cv::Mat OpenCVWrapper::equalizeHist(const cv::Mat& src) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;

    if (src.channels() == 1) {
        cv::equalizeHist(src, dst);
    } else {
        // For color images, convert to YUV and equalize Y channel
        cv::Mat yuv;
        cv::cvtColor(src, yuv, cv::COLOR_BGR2YUV);

        std::vector<cv::Mat> channels;
        cv::split(yuv, channels);

        cv::equalizeHist(channels[0], channels[0]);

        cv::merge(channels, yuv);
        cv::cvtColor(yuv, dst, cv::COLOR_YUV2BGR);
    }

    Logger::debug("Histogram equalization applied");
    return dst;
}

cv::Mat OpenCVWrapper::bilateralFilter(const cv::Mat& src,
                                       int d,
                                       double sigmaColor,
                                       double sigmaSpace) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;
    cv::bilateralFilter(src, dst, d, sigmaColor, sigmaSpace);

    Logger::debug("Bilateral filter applied");
    return dst;
}

cv::Mat OpenCVWrapper::medianBlur(const cv::Mat& src, int ksize) {
    if (src.empty()) {
        Logger::error("Source image is empty");
        return cv::Mat();
    }

    cv::Mat dst;
    cv::medianBlur(src, dst, ksize);

    Logger::debug("Median blur applied with kernel size: " + std::to_string(ksize));
    return dst;
}

cv::Rect OpenCVWrapper::boundingRect(const std::vector<cv::Point>& points) {
    if (points.empty()) {
        Logger::error("Points vector is empty");
        return cv::Rect();
    }

    cv::Rect rect = cv::boundingRect(points);
    Logger::debug("Bounding rectangle calculated");
    return rect;
}

double OpenCVWrapper::contourArea(const std::vector<cv::Point>& contour) {
    if (contour.empty()) {
        Logger::error("Contour is empty");
        return 0.0;
    }

    double area = cv::contourArea(contour);
    Logger::debug("Contour area calculated: " + std::to_string(area));
    return area;
}

double OpenCVWrapper::arcLength(const std::vector<cv::Point>& curve, bool closed) {
    if (curve.empty()) {
        Logger::error("Curve is empty");
        return 0.0;
    }

    double length = cv::arcLength(curve, closed);
    Logger::debug("Arc length calculated: " + std::to_string(length));
    return length;
}

std::vector<cv::Point> OpenCVWrapper::approxPolyDP(const std::vector<cv::Point>& curve,
                                                   double epsilon,
                                                   bool closed) {
    if (curve.empty()) {
        Logger::error("Curve is empty");
        return std::vector<cv::Point>();
    }

    std::vector<cv::Point> approx;
    cv::approxPolyDP(curve, approx, epsilon, closed);

    Logger::debug("Polygon approximation completed, points: " + std::to_string(approx.size()));
    return approx;
}

cv::Mat OpenCVWrapper::getStructuringElement(int shape,
                                             const cv::Size& ksize,
                                             const cv::Point& anchor) {
    cv::Mat element = cv::getStructuringElement(shape, ksize, anchor);
    Logger::debug("Structuring element created");
    return element;
}

std::string OpenCVWrapper::getOpenCVVersion() {
    return std::string(CV_VERSION);
}

cv::Size OpenCVWrapper::getImageSize(const cv::Mat& image) {
    if (image.empty()) {
        Logger::error("Image is empty");
        return cv::Size(0, 0);
    }

    return cv::Size(image.cols, image.rows);
}

int OpenCVWrapper::getImageChannels(const cv::Mat& image) {
    if (image.empty()) {
        Logger::error("Image is empty");
        return 0;
    }

    return image.channels();
}

int OpenCVWrapper::getImageDepth(const cv::Mat& image) {
    if (image.empty()) {
        Logger::error("Image is empty");
        return -1;
    }

    return image.depth();
}

}  // namespace oneday::image
