#include "window_manager.h"
#include "../common/logger.h"

using oneday::core::Logger;
#include <QApplication>
#include <QScreen>
#include <QWindow>
#include <QWidget>

namespace oneday::game {

WindowManager::WindowManager(QObject* parent)
    : QObject(parent)
    , m_primaryWindow(nullptr)
    , m_isFullscreen(false)
    , m_isMaximized(false)
{
    Logger::info("WindowManager initialized");
    
    // 获取屏幕信息
    if (QApplication::primaryScreen()) {
        m_screenGeometry = QApplication::primaryScreen()->geometry();
        Logger::info("Primary screen geometry: " + 
                    std::to_string(m_screenGeometry.width()) + "x" + 
                    std::to_string(m_screenGeometry.height()));
    }
}

WindowManager::~WindowManager() {
    Logger::info("WindowManager destroyed");
}

void WindowManager::setPrimaryWindow(QWidget* window) {
    if (m_primaryWindow == window) {
        return;
    }
    
    if (m_primaryWindow) {
        // 断开之前窗口的连�?        disconnect(m_primaryWindow, nullptr, this, nullptr);
    }
    
    m_primaryWindow = window;
    
    if (m_primaryWindow) {
        // 连接窗口信号
        connect(m_primaryWindow, &QWidget::destroyed, this, [this]() {
            m_primaryWindow = nullptr;
        });
        
        Logger::info("Primary window set");
        emit primaryWindowChanged(m_primaryWindow);
    }
}

QWidget* WindowManager::getPrimaryWindow() const {
    return m_primaryWindow;
}

void WindowManager::showWindow() {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    m_primaryWindow->show();
    Logger::info("Window shown");
    emit windowShown();
}

void WindowManager::hideWindow() {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    m_primaryWindow->hide();
    Logger::info("Window hidden");
    emit windowHidden();
}

void WindowManager::minimizeWindow() {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    m_primaryWindow->showMinimized();
    Logger::info("Window minimized");
    emit windowMinimized();
}

void WindowManager::maximizeWindow() {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    if (m_isMaximized) {
        m_primaryWindow->showNormal();
        m_isMaximized = false;
        Logger::info("Window restored from maximized");
        emit windowRestored();
    } else {
        m_primaryWindow->showMaximized();
        m_isMaximized = true;
        Logger::info("Window maximized");
        emit windowMaximized();
    }
}

void WindowManager::toggleFullscreen() {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    if (m_isFullscreen) {
        exitFullscreen();
    } else {
        enterFullscreen();
    }
}

void WindowManager::enterFullscreen() {
    if (!m_primaryWindow || m_isFullscreen) {
        return;
    }
    
    // 保存当前窗口状�?    m_savedGeometry = m_primaryWindow->geometry();
    m_savedWindowState = m_primaryWindow->windowState();
    
    m_primaryWindow->showFullScreen();
    m_isFullscreen = true;
    
    Logger::info("Entered fullscreen mode");
    emit fullscreenEntered();
}

void WindowManager::exitFullscreen() {
    if (!m_primaryWindow || !m_isFullscreen) {
        return;
    }
    
    m_primaryWindow->setWindowState(m_savedWindowState);
    m_primaryWindow->setGeometry(m_savedGeometry);
    
    m_isFullscreen = false;
    
    Logger::info("Exited fullscreen mode");
    emit fullscreenExited();
}

void WindowManager::centerWindow() {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    QRect windowGeometry = m_primaryWindow->geometry();
    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    
    int x = (screenGeometry.width() - windowGeometry.width()) / 2;
    int y = (screenGeometry.height() - windowGeometry.height()) / 2;
    
    m_primaryWindow->move(x, y);
    
    Logger::info("Window centered");
    emit windowMoved(QPoint(x, y));
}

void WindowManager::setWindowSize(int width, int height) {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    m_primaryWindow->resize(width, height);
    
    Logger::info("Window size set to " + std::to_string(width) + "x" + std::to_string(height));
    emit windowResized(QSize(width, height));
}

void WindowManager::setWindowPosition(int x, int y) {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    m_primaryWindow->move(x, y);
    
    Logger::info("Window position set to (" + std::to_string(x) + "," + std::to_string(y) + ")");
    emit windowMoved(QPoint(x, y));
}

void WindowManager::setWindowTitle(const QString& title) {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    m_primaryWindow->setWindowTitle(title);
    
    Logger::info("Window title set to: " + title.toStdString());
    emit windowTitleChanged(title);
}

QString WindowManager::getWindowTitle() const {
    if (!m_primaryWindow) {
        return QString();
    }
    
    return m_primaryWindow->windowTitle();
}

QSize WindowManager::getWindowSize() const {
    if (!m_primaryWindow) {
        return QSize();
    }
    
    return m_primaryWindow->size();
}

QPoint WindowManager::getWindowPosition() const {
    if (!m_primaryWindow) {
        return QPoint();
    }
    
    return m_primaryWindow->pos();
}

bool WindowManager::isFullscreen() const {
    return m_isFullscreen;
}

bool WindowManager::isMaximized() const {
    return m_isMaximized;
}

bool WindowManager::isMinimized() const {
    if (!m_primaryWindow) {
        return false;
    }
    
    return m_primaryWindow->isMinimized();
}

bool WindowManager::isVisible() const {
    if (!m_primaryWindow) {
        return false;
    }
    
    return m_primaryWindow->isVisible();
}

QRect WindowManager::getScreenGeometry() const {
    return m_screenGeometry;
}

void WindowManager::setWindowIcon(const QIcon& icon) {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    m_primaryWindow->setWindowIcon(icon);
    Logger::info("Window icon set");
    emit windowIconChanged(icon);
}

QIcon WindowManager::getWindowIcon() const {
    if (!m_primaryWindow) {
        return QIcon();
    }
    
    return m_primaryWindow->windowIcon();
}

void WindowManager::setWindowOpacity(double opacity) {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    opacity = std::max(0.0, std::min(1.0, opacity)); // 限制�?-1范围�?    m_primaryWindow->setWindowOpacity(opacity);
    
    Logger::info("Window opacity set to " + std::to_string(opacity));
    emit windowOpacityChanged(opacity);
}

double WindowManager::getWindowOpacity() const {
    if (!m_primaryWindow) {
        return 1.0;
    }
    
    return m_primaryWindow->windowOpacity();
}

void WindowManager::setAlwaysOnTop(bool onTop) {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    Qt::WindowFlags flags = m_primaryWindow->windowFlags();
    
    if (onTop) {
        flags |= Qt::WindowStaysOnTopHint;
    } else {
        flags &= ~Qt::WindowStaysOnTopHint;
    }
    
    m_primaryWindow->setWindowFlags(flags);
    m_primaryWindow->show(); // 需要重新显示窗口以应用标志
    
    Logger::info("Window always on top: " + std::string(onTop ? "enabled" : "disabled"));
    emit alwaysOnTopChanged(onTop);
}

bool WindowManager::isAlwaysOnTop() const {
    if (!m_primaryWindow) {
        return false;
    }
    
    return m_primaryWindow->windowFlags() & Qt::WindowStaysOnTopHint;
}

void WindowManager::saveWindowState() {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    m_savedGeometry = m_primaryWindow->geometry();
    m_savedWindowState = m_primaryWindow->windowState();
    
    Logger::info("Window state saved");
}

void WindowManager::restoreWindowState() {
    if (!m_primaryWindow) {
        Logger::error("No primary window set");
        return;
    }
    
    if (!m_savedGeometry.isNull()) {
        m_primaryWindow->setGeometry(m_savedGeometry);
        m_primaryWindow->setWindowState(m_savedWindowState);
        
        Logger::info("Window state restored");
        emit windowRestored();
    }
}

void WindowManager::updateScreenGeometry() {
    if (QApplication::primaryScreen()) {
        m_screenGeometry = QApplication::primaryScreen()->geometry();
        Logger::info("Screen geometry updated: " + 
                    std::to_string(m_screenGeometry.width()) + "x" + 
                    std::to_string(m_screenGeometry.height()));
        emit screenGeometryChanged(m_screenGeometry);
    }
}

} // namespace oneday::game
