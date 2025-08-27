#include "screen_capture.h"
#include "../common/logger.h"

using oneday::core::Logger;
#include <QApplication>
#include <QScreen>
#include <QPixmap>
#include <QWindow>
#include <QWidget>
#include <QDesktopWidget>
#include <QTimer>
#include <opencv2/opencv.hpp>

#ifdef _WIN32
#include <windows.h>
#include <wingdi.h>
#endif

namespace oneday::game {

ScreenCapture::ScreenCapture(QObject* parent)
    : QObject(parent)
    , m_captureTimer(new QTimer(this))
    , m_isCapturing(false)
    , m_captureInterval(100) // 默认10fps
{
    Logger::info("ScreenCapture initialized");
    
    // 连接定时�?    connect(m_captureTimer, &QTimer::timeout, this, &ScreenCapture::captureFrame);
}

ScreenCapture::~ScreenCapture() {
    stopCapture();
    Logger::info("ScreenCapture destroyed");
}

QPixmap ScreenCapture::captureScreen() {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) {
        Logger::error("No primary screen found");
        return QPixmap();
    }
    
    QPixmap screenshot = screen->grabWindow(0);
    
    if (screenshot.isNull()) {
        Logger::error("Failed to capture screen");
    } else {
        Logger::debug("Screen captured: " + std::to_string(screenshot.width()) + "x" + 
                     std::to_string(screenshot.height()));
    }
    
    return screenshot;
}

QPixmap ScreenCapture::captureWindow(QWidget* window) {
    if (!window) {
        Logger::error("Window is null");
        return QPixmap();
    }
    
    QPixmap screenshot = window->grab();
    
    if (screenshot.isNull()) {
        Logger::error("Failed to capture window");
    } else {
        Logger::debug("Window captured: " + std::to_string(screenshot.width()) + "x" + 
                     std::to_string(screenshot.height()));
    }
    
    return screenshot;
}

QPixmap ScreenCapture::captureRegion(const QRect& region) {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) {
        Logger::error("No primary screen found");
        return QPixmap();
    }
    
    QPixmap screenshot = screen->grabWindow(0, region.x(), region.y(), 
                                          region.width(), region.height());
    
    if (screenshot.isNull()) {
        Logger::error("Failed to capture region");
    } else {
        Logger::debug("Region captured: " + std::to_string(region.x()) + "," + 
                     std::to_string(region.y()) + " " + 
                     std::to_string(region.width()) + "x" + std::to_string(region.height()));
    }
    
    return screenshot;
}

cv::Mat ScreenCapture::captureScreenToMat() {
    QPixmap pixmap = captureScreen();
    return pixmapToMat(pixmap);
}

cv::Mat ScreenCapture::captureWindowToMat(QWidget* window) {
    QPixmap pixmap = captureWindow(window);
    return pixmapToMat(pixmap);
}

cv::Mat ScreenCapture::captureRegionToMat(const QRect& region) {
    QPixmap pixmap = captureRegion(region);
    return pixmapToMat(pixmap);
}

cv::Mat ScreenCapture::pixmapToMat(const QPixmap& pixmap) {
    if (pixmap.isNull()) {
        return cv::Mat();
    }
    
    QImage image = pixmap.toImage();
    return qImageToMat(image);
}

cv::Mat ScreenCapture::qImageToMat(const QImage& qImage) {
    if (qImage.isNull()) {
        return cv::Mat();
    }
    
    QImage swapped = qImage.rgbSwapped();
    
    cv::Mat mat(swapped.height(), swapped.width(), CV_8UC4, 
                (void*)swapped.constBits(), swapped.bytesPerLine());
    
    cv::Mat result;
    cv::cvtColor(mat, result, cv::COLOR_BGRA2BGR);
    
    return result.clone(); // 返回副本以避免数据共享问�?}

QPixmap ScreenCapture::matToPixmap(const cv::Mat& mat) {
    if (mat.empty()) {
        return QPixmap();
    }
    
    QImage qImage = matToQImage(mat);
    return QPixmap::fromImage(qImage);
}

QImage ScreenCapture::matToQImage(const cv::Mat& mat) {
    if (mat.empty()) {
        return QImage();
    }
    
    switch (mat.type()) {
        case CV_8UC4: {
            QImage qImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
            return qImage.rgbSwapped();
        }
        case CV_8UC3: {
            QImage qImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
            return qImage.rgbSwapped();
        }
        case CV_8UC1: {
            QImage qImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
            return qImage;
        }
        default: {
            Logger::warning("Unsupported Mat type for conversion to QImage");
            return QImage();
        }
    }
}

void ScreenCapture::startCapture() {
    if (m_isCapturing) {
        Logger::warning("Screen capture already running");
        return;
    }
    
    m_isCapturing = true;
    m_captureTimer->start(m_captureInterval);
    
    Logger::info("Screen capture started with interval " + std::to_string(m_captureInterval) + "ms");
    emit captureStarted();
}

void ScreenCapture::stopCapture() {
    if (!m_isCapturing) {
        return;
    }
    
    m_isCapturing = false;
    m_captureTimer->stop();
    
    Logger::info("Screen capture stopped");
    emit captureStopped();
}

void ScreenCapture::setCaptureInterval(int intervalMs) {
    m_captureInterval = std::max(10, intervalMs); // 最�?0ms间隔
    
    if (m_isCapturing) {
        m_captureTimer->setInterval(m_captureInterval);
    }
    
    Logger::info("Capture interval set to " + std::to_string(m_captureInterval) + "ms");
}

int ScreenCapture::getCaptureInterval() const {
    return m_captureInterval;
}

bool ScreenCapture::isCapturing() const {
    return m_isCapturing;
}

void ScreenCapture::setCaptureRegion(const QRect& region) {
    m_captureRegion = region;
    Logger::info("Capture region set to " + std::to_string(region.x()) + "," + 
                std::to_string(region.y()) + " " + 
                std::to_string(region.width()) + "x" + std::to_string(region.height()));
}

QRect ScreenCapture::getCaptureRegion() const {
    return m_captureRegion;
}

void ScreenCapture::clearCaptureRegion() {
    m_captureRegion = QRect();
    Logger::info("Capture region cleared");
}

bool ScreenCapture::saveScreenshot(const QString& filename) {
    QPixmap screenshot = captureScreen();
    if (screenshot.isNull()) {
        return false;
    }
    
    bool success = screenshot.save(filename);
    if (success) {
        Logger::info("Screenshot saved to: " + filename.toStdString());
    } else {
        Logger::error("Failed to save screenshot to: " + filename.toStdString());
    }
    
    return success;
}

bool ScreenCapture::saveWindowScreenshot(QWidget* window, const QString& filename) {
    QPixmap screenshot = captureWindow(window);
    if (screenshot.isNull()) {
        return false;
    }
    
    bool success = screenshot.save(filename);
    if (success) {
        Logger::info("Window screenshot saved to: " + filename.toStdString());
    } else {
        Logger::error("Failed to save window screenshot to: " + filename.toStdString());
    }
    
    return success;
}

bool ScreenCapture::saveRegionScreenshot(const QRect& region, const QString& filename) {
    QPixmap screenshot = captureRegion(region);
    if (screenshot.isNull()) {
        return false;
    }
    
    bool success = screenshot.save(filename);
    if (success) {
        Logger::info("Region screenshot saved to: " + filename.toStdString());
    } else {
        Logger::error("Failed to save region screenshot to: " + filename.toStdString());
    }
    
    return success;
}

QSize ScreenCapture::getScreenSize() {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) {
        Logger::error("No primary screen found");
        return QSize();
    }
    
    return screen->size();
}

QRect ScreenCapture::getScreenGeometry() {
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) {
        Logger::error("No primary screen found");
        return QRect();
    }
    
    return screen->geometry();
}

void ScreenCapture::captureFrame() {
    if (!m_isCapturing) {
        return;
    }
    
    QPixmap frame;
    
    if (m_captureRegion.isValid()) {
        frame = captureRegion(m_captureRegion);
    } else {
        frame = captureScreen();
    }
    
    if (!frame.isNull()) {
        emit frameCapture(frame);
        
        // 如果需要OpenCV格式
        cv::Mat matFrame = pixmapToMat(frame);
        if (!matFrame.empty()) {
            emit frameCaptureMat(matFrame);
        }
    }
}

#ifdef _WIN32
QPixmap ScreenCapture::captureWindowByHandle(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        Logger::error("Invalid window handle");
        return QPixmap();
    }
    
    RECT rect;
    if (!GetWindowRect(hwnd, &rect)) {
        Logger::error("Failed to get window rectangle");
        return QPixmap();
    }
    
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    
    HDC hdcWindow = GetDC(hwnd);
    HDC hdcMemDC = CreateCompatibleDC(hdcWindow);
    HBITMAP hbmScreen = CreateCompatibleBitmap(hdcWindow, width, height);
    
    SelectObject(hdcMemDC, hbmScreen);
    
    if (!BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY)) {
        Logger::error("BitBlt failed");
        DeleteObject(hbmScreen);
        DeleteDC(hdcMemDC);
        ReleaseDC(hwnd, hdcWindow);
        return QPixmap();
    }
    
    // 转换为QPixmap
    QPixmap pixmap = QPixmap::fromWinHBITMAP(hbmScreen);
    
    // 清理资源
    DeleteObject(hbmScreen);
    DeleteDC(hdcMemDC);
    ReleaseDC(hwnd, hdcWindow);
    
    Logger::debug("Window captured by handle: " + std::to_string(width) + "x" + std::to_string(height));
    return pixmap;
}

std::vector<HWND> ScreenCapture::findWindowsByTitle(const QString& title) {
    std::vector<HWND> windows;
    
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        auto* data = reinterpret_cast<std::pair<QString, std::vector<HWND>*>*>(lParam);
        
        wchar_t windowTitle[256];
        if (GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(wchar_t))) {
            QString currentTitle = QString::fromWCharArray(windowTitle);
            if (currentTitle.contains(data->first, Qt::CaseInsensitive)) {
                data->second->push_back(hwnd);
            }
        }
        
        return TRUE;
    }, reinterpret_cast<LPARAM>(&std::make_pair(title, &windows)));
    
    Logger::info("Found " + std::to_string(windows.size()) + " windows with title containing: " + title.toStdString());
    return windows;
}
#endif

} // namespace oneday::game
