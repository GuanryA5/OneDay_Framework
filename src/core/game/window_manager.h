#pragma once

#include <QObject>
#include <QWidget>
#include <QRect>
#include <QSize>
#include <QPoint>
#include <QIcon>
#include <QString>

namespace oneday::game {

/**
 * @brief 窗口管理器类
 * 
 * 管理游戏主窗口的显示、大小、位置等属性
 */
class WindowManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit WindowManager(QObject* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~WindowManager();
    
    /**
     * @brief 设置主窗口
     * @param window 主窗口指针
     */
    void setPrimaryWindow(QWidget* window);
    
    /**
     * @brief 获取主窗口
     * @return 主窗口指针
     */
    QWidget* getPrimaryWindow() const;
    
    /**
     * @brief 显示窗口
     */
    void showWindow();
    
    /**
     * @brief 隐藏窗口
     */
    void hideWindow();
    
    /**
     * @brief 最小化窗口
     */
    void minimizeWindow();
    
    /**
     * @brief 最大化/还原窗口
     */
    void maximizeWindow();
    
    /**
     * @brief 切换全屏模式
     */
    void toggleFullscreen();
    
    /**
     * @brief 进入全屏模式
     */
    void enterFullscreen();
    
    /**
     * @brief 退出全屏模式
     */
    void exitFullscreen();
    
    /**
     * @brief 居中显示窗口
     */
    void centerWindow();
    
    /**
     * @brief 设置窗口大小
     * @param width 宽度
     * @param height 高度
     */
    void setWindowSize(int width, int height);
    
    /**
     * @brief 设置窗口位置
     * @param x X坐标
     * @param y Y坐标
     */
    void setWindowPosition(int x, int y);
    
    /**
     * @brief 设置窗口标题
     * @param title 标题
     */
    void setWindowTitle(const QString& title);
    
    /**
     * @brief 获取窗口标题
     * @return 标题
     */
    QString getWindowTitle() const;
    
    /**
     * @brief 获取窗口大小
     * @return 窗口大小
     */
    QSize getWindowSize() const;
    
    /**
     * @brief 获取窗口位置
     * @return 窗口位置
     */
    QPoint getWindowPosition() const;
    
    /**
     * @brief 检查是否全屏
     * @return 是否全屏
     */
    bool isFullscreen() const;
    
    /**
     * @brief 检查是否最大化
     * @return 是否最大化
     */
    bool isMaximized() const;
    
    /**
     * @brief 检查是否最小化
     * @return 是否最小化
     */
    bool isMinimized() const;
    
    /**
     * @brief 检查是否可见
     * @return 是否可见
     */
    bool isVisible() const;
    
    /**
     * @brief 获取屏幕几何信息
     * @return 屏幕矩形
     */
    QRect getScreenGeometry() const;
    
    /**
     * @brief 设置窗口图标
     * @param icon 图标
     */
    void setWindowIcon(const QIcon& icon);
    
    /**
     * @brief 获取窗口图标
     * @return 图标
     */
    QIcon getWindowIcon() const;
    
    /**
     * @brief 设置窗口透明度
     * @param opacity 透明度 (0.0-1.0)
     */
    void setWindowOpacity(double opacity);
    
    /**
     * @brief 获取窗口透明度
     * @return 透明度
     */
    double getWindowOpacity() const;
    
    /**
     * @brief 设置窗口总在最前
     * @param onTop 是否总在最前
     */
    void setAlwaysOnTop(bool onTop);
    
    /**
     * @brief 检查窗口是否总在最前
     * @return 是否总在最前
     */
    bool isAlwaysOnTop() const;
    
    /**
     * @brief 保存窗口状态
     */
    void saveWindowState();
    
    /**
     * @brief 恢复窗口状态
     */
    void restoreWindowState();
    
    /**
     * @brief 更新屏幕几何信息
     */
    void updateScreenGeometry();

signals:
    /**
     * @brief 主窗口改变信号
     * @param window 新的主窗口
     */
    void primaryWindowChanged(QWidget* window);
    
    /**
     * @brief 窗口显示信号
     */
    void windowShown();
    
    /**
     * @brief 窗口隐藏信号
     */
    void windowHidden();
    
    /**
     * @brief 窗口最小化信号
     */
    void windowMinimized();
    
    /**
     * @brief 窗口最大化信号
     */
    void windowMaximized();
    
    /**
     * @brief 窗口还原信号
     */
    void windowRestored();
    
    /**
     * @brief 进入全屏信号
     */
    void fullscreenEntered();
    
    /**
     * @brief 退出全屏信号
     */
    void fullscreenExited();
    
    /**
     * @brief 窗口移动信号
     * @param position 新位置
     */
    void windowMoved(const QPoint& position);
    
    /**
     * @brief 窗口大小改变信号
     * @param size 新大小
     */
    void windowResized(const QSize& size);
    
    /**
     * @brief 窗口标题改变信号
     * @param title 新标题
     */
    void windowTitleChanged(const QString& title);
    
    /**
     * @brief 窗口图标改变信号
     * @param icon 新图标
     */
    void windowIconChanged(const QIcon& icon);
    
    /**
     * @brief 窗口透明度改变信号
     * @param opacity 新透明度
     */
    void windowOpacityChanged(double opacity);
    
    /**
     * @brief 总在最前状态改变信号
     * @param onTop 新状态
     */
    void alwaysOnTopChanged(bool onTop);
    
    /**
     * @brief 屏幕几何信息改变信号
     * @param geometry 新几何信息
     */
    void screenGeometryChanged(const QRect& geometry);

private:
    QWidget* m_primaryWindow;           ///< 主窗口指针
    bool m_isFullscreen;                ///< 是否全屏
    bool m_isMaximized;                 ///< 是否最大化
    QRect m_screenGeometry;             ///< 屏幕几何信息
    QRect m_savedGeometry;              ///< 保存的窗口几何信息
    Qt::WindowStates m_savedWindowState; ///< 保存的窗口状态
};

} // namespace oneday::game
