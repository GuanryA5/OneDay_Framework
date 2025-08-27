#pragma once

#include <QObject>
#include <QPixmap>
#include <QImage>
#include <QRect>
#include <QSize>
#include <QTimer>
#include <opencv2/opencv.hpp>

#ifdef _WIN32
#include <windows.h>
#include <vector>
#endif

class QWidget;

namespace oneday::game {

/**
 * @brief 屏幕截图类
 * 
 * 提供屏幕截图、窗口截图和区域截图功能
 */
class ScreenCapture : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit ScreenCapture(QObject* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ScreenCapture();
    
    /**
     * @brief 截取整个屏幕
     * @return 屏幕截图
     */
    QPixmap captureScreen();
    
    /**
     * @brief 截取指定窗口
     * @param window 目标窗口
     * @return 窗口截图
     */
    QPixmap captureWindow(QWidget* window);
    
    /**
     * @brief 截取屏幕区域
     * @param region 目标区域
     * @return 区域截图
     */
    QPixmap captureRegion(const QRect& region);
    
    /**
     * @brief 截取屏幕并转换为OpenCV Mat
     * @return OpenCV Mat格式的截图
     */
    cv::Mat captureScreenToMat();
    
    /**
     * @brief 截取窗口并转换为OpenCV Mat
     * @param window 目标窗口
     * @return OpenCV Mat格式的截图
     */
    cv::Mat captureWindowToMat(QWidget* window);
    
    /**
     * @brief 截取区域并转换为OpenCV Mat
     * @param region 目标区域
     * @return OpenCV Mat格式的截图
     */
    cv::Mat captureRegionToMat(const QRect& region);
    
    /**
     * @brief 开始连续截图
     */
    void startCapture();
    
    /**
     * @brief 停止连续截图
     */
    void stopCapture();
    
    /**
     * @brief 设置截图间隔
     * @param intervalMs 间隔时间（毫秒）
     */
    void setCaptureInterval(int intervalMs);
    
    /**
     * @brief 获取截图间隔
     * @return 间隔时间（毫秒）
     */
    int getCaptureInterval() const;
    
    /**
     * @brief 检查是否正在截图
     * @return 是否正在截图
     */
    bool isCapturing() const;
    
    /**
     * @brief 设置截图区域
     * @param region 截图区域
     */
    void setCaptureRegion(const QRect& region);
    
    /**
     * @brief 获取截图区域
     * @return 截图区域
     */
    QRect getCaptureRegion() const;
    
    /**
     * @brief 清除截图区域（截取整个屏幕）
     */
    void clearCaptureRegion();
    
    /**
     * @brief 保存屏幕截图到文件
     * @param filename 文件名
     * @return 是否保存成功
     */
    bool saveScreenshot(const QString& filename);
    
    /**
     * @brief 保存窗口截图到文件
     * @param window 目标窗口
     * @param filename 文件名
     * @return 是否保存成功
     */
    bool saveWindowScreenshot(QWidget* window, const QString& filename);
    
    /**
     * @brief 保存区域截图到文件
     * @param region 目标区域
     * @param filename 文件名
     * @return 是否保存成功
     */
    bool saveRegionScreenshot(const QRect& region, const QString& filename);
    
    /**
     * @brief 获取屏幕尺寸
     * @return 屏幕尺寸
     */
    static QSize getScreenSize();
    
    /**
     * @brief 获取屏幕几何信息
     * @return 屏幕矩形
     */
    static QRect getScreenGeometry();
    
    // === 格式转换工具函数 ===
    
    /**
     * @brief QPixmap转换为OpenCV Mat
     * @param pixmap QPixmap对象
     * @return OpenCV Mat对象
     */
    static cv::Mat pixmapToMat(const QPixmap& pixmap);
    
    /**
     * @brief QImage转换为OpenCV Mat
     * @param qImage QImage对象
     * @return OpenCV Mat对象
     */
    static cv::Mat qImageToMat(const QImage& qImage);
    
    /**
     * @brief OpenCV Mat转换为QPixmap
     * @param mat OpenCV Mat对象
     * @return QPixmap对象
     */
    static QPixmap matToPixmap(const cv::Mat& mat);
    
    /**
     * @brief OpenCV Mat转换为QImage
     * @param mat OpenCV Mat对象
     * @return QImage对象
     */
    static QImage matToQImage(const cv::Mat& mat);

#ifdef _WIN32
    /**
     * @brief 通过窗口句柄截取窗口
     * @param hwnd 窗口句柄
     * @return 窗口截图
     */
    QPixmap captureWindowByHandle(HWND hwnd);
    
    /**
     * @brief 根据标题查找窗口
     * @param title 窗口标题（支持部分匹配）
     * @return 窗口句柄列表
     */
    std::vector<HWND> findWindowsByTitle(const QString& title);
#endif

signals:
    /**
     * @brief 截图开始信号
     */
    void captureStarted();
    
    /**
     * @brief 截图停止信号
     */
    void captureStopped();
    
    /**
     * @brief 帧截图信号（QPixmap格式）
     * @param frame 截图帧
     */
    void frameCapture(const QPixmap& frame);
    
    /**
     * @brief 帧截图信号（OpenCV Mat格式）
     * @param frame 截图帧
     */
    void frameCaptureMat(const cv::Mat& frame);

private slots:
    /**
     * @brief 截取一帧
     */
    void captureFrame();

private:
    QTimer* m_captureTimer;     ///< 截图定时器
    bool m_isCapturing;         ///< 是否正在截图
    int m_captureInterval;      ///< 截图间隔（毫秒）
    QRect m_captureRegion;      ///< 截图区域
};

} // namespace oneday::game
