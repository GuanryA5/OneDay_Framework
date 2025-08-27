#include "processor.h"

#include <opencv2/opencv.hpp>

#include "../common/logger.h"

using oneday::core::Logger;

namespace oneday::image {

ImageProcessor::ImageProcessor() {
    Logger::info("ImageProcessor initialized");
}

ImageProcessor::~ImageProcessor() {
    Logger::info("ImageProcessor destroyed");
}

cv::Mat ImageProcessor::processImage(const cv::Mat& input) {
    if (input.empty()) {
        Logger::error("Input image is empty");
        return cv::Mat();
    }

    cv::Mat result;
    input.copyTo(result);

    Logger::debug("Image processed successfully");
    return result;
}

cv::Mat ImageProcessor::resize(const cv::Mat& input, const cv::Size& size) {
    if (input.empty()) {
        Logger::error("Input image is empty");
        return cv::Mat();
    }

    cv::Mat result;
    cv::resize(input, result, size);

    Logger::debug("Image resized to " + std::to_string(size.width) + "x" +
                  std::to_string(size.height));
    return result;
}

cv::Mat ImageProcessor::convertColorSpace(const cv::Mat& input, int code) {
    if (input.empty()) {
        Logger::error("Input image is empty");
        return cv::Mat();
    }

    cv::Mat result;
    cv::cvtColor(input, result, code);

    Logger::debug("Color space converted");
    return result;
}

cv::Mat ImageProcessor::applyGaussianBlur(const cv::Mat& input,
                                          const cv::Size& kernelSize,
                                          double sigmaX,
                                          double sigmaY) {
    if (input.empty()) {
        Logger::error("Input image is empty");
        return cv::Mat();
    }

    cv::Mat result;
    cv::GaussianBlur(input, result, kernelSize, sigmaX, sigmaY);

    Logger::debug("Gaussian blur applied");
    return result;
}

cv::Mat ImageProcessor::detectEdges(const cv::Mat& input, double threshold1, double threshold2) {
    if (input.empty()) {
        Logger::error("Input image is empty");
        return cv::Mat();
    }

    cv::Mat gray, edges;

    // Convert to grayscale if needed
    if (input.channels() > 1) {
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = input;
    }

    // Apply Canny edge detection
    cv::Canny(gray, edges, threshold1, threshold2);

    Logger::debug("Edge detection completed");
    return edges;
}

std::vector<cv::Rect> ImageProcessor::detectObjects(const cv::Mat& input,
                                                    const std::string& cascadePath) {
    std::vector<cv::Rect> objects;

    if (input.empty()) {
        Logger::error("Input image is empty");
        return objects;
    }

    cv::CascadeClassifier cascade;
    if (!cascade.load(cascadePath)) {
        Logger::error("Failed to load cascade classifier: " + cascadePath);
        return objects;
    }

    cv::Mat gray;
    if (input.channels() > 1) {
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = input;
    }

    cascade.detectMultiScale(gray, objects, 1.1, 3, 0, cv::Size(30, 30));

    Logger::info("Detected " + std::to_string(objects.size()) + " objects");
    return objects;
}

cv::Mat ImageProcessor::enhanceContrast(const cv::Mat& input, double alpha, int beta) {
    if (input.empty()) {
        Logger::error("Input image is empty");
        return cv::Mat();
    }

    cv::Mat result;
    input.convertTo(result, -1, alpha, beta);

    Logger::debug("Contrast enhanced");
    return result;
}

cv::Mat ImageProcessor::applyHistogramEqualization(const cv::Mat& input) {
    if (input.empty()) {
        Logger::error("Input image is empty");
        return cv::Mat();
    }

    cv::Mat result;

    if (input.channels() == 1) {
        // Grayscale image
        cv::equalizeHist(input, result);
    } else {
        // Color image - convert to YUV and equalize Y channel
        cv::Mat yuv;
        cv::cvtColor(input, yuv, cv::COLOR_BGR2YUV);

        std::vector<cv::Mat> channels;
        cv::split(yuv, channels);

        cv::equalizeHist(channels[0], channels[0]);

        cv::merge(channels, yuv);
        cv::cvtColor(yuv, result, cv::COLOR_YUV2BGR);
    }

    Logger::debug("Histogram equalization applied");
    return result;
}

bool ImageProcessor::saveImage(const cv::Mat& image, const std::string& filename) {
    if (image.empty()) {
        Logger::error("Cannot save empty image");
        return false;
    }

    bool success = cv::imwrite(filename, image);
    if (success) {
        Logger::info("Image saved to: " + filename);
    } else {
        Logger::error("Failed to save image to: " + filename);
    }

    return success;
}

cv::Mat ImageProcessor::loadImage(const std::string& filename, int flags) {
    cv::Mat image = cv::imread(filename, flags);

    if (image.empty()) {
        Logger::error("Failed to load image: " + filename);
    } else {
        Logger::info("Image loaded: " + filename + " (" + std::to_string(image.cols) + "x" +
                     std::to_string(image.rows) + ")");
    }

    return image;
}

}  // namespace oneday::image
