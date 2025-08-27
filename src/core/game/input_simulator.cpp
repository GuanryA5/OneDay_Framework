#include "input_simulator.h"
#include "../common/logger.h"

using oneday::core::Logger;
#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QTimer>
#include <QCursor>

#ifdef _WIN32
#include <windows.h>
#endif

namespace oneday::game {

InputSimulator::InputSimulator(QObject* parent)
    : QObject(parent)
    , m_delayTimer(new QTimer(this))
{
    Logger::info("InputSimulator initialized");
    
    m_delayTimer->setSingleShot(true);
    connect(m_delayTimer, &QTimer::timeout, this, &InputSimulator::executeDelayedAction);
}

InputSimulator::~InputSimulator() {
    Logger::info("InputSimulator destroyed");
}

void InputSimulator::simulateMouseClick(const QPoint& position, Qt::MouseButton button, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, position, button]() {
            performMouseClick(position, button);
        });
        m_delayTimer->start(delayMs);
    } else {
        performMouseClick(position, button);
    }
}

void InputSimulator::simulateMouseDoubleClick(const QPoint& position, Qt::MouseButton button, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, position, button]() {
            performMouseDoubleClick(position, button);
        });
        m_delayTimer->start(delayMs);
    } else {
        performMouseDoubleClick(position, button);
    }
}

void InputSimulator::simulateMouseMove(const QPoint& position, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, position]() {
            performMouseMove(position);
        });
        m_delayTimer->start(delayMs);
    } else {
        performMouseMove(position);
    }
}

void InputSimulator::simulateMouseDrag(const QPoint& startPos, const QPoint& endPos, 
                                      Qt::MouseButton button, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, startPos, endPos, button]() {
            performMouseDrag(startPos, endPos, button);
        });
        m_delayTimer->start(delayMs);
    } else {
        performMouseDrag(startPos, endPos, button);
    }
}

void InputSimulator::simulateMouseWheel(const QPoint& position, int delta, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, position, delta]() {
            performMouseWheel(position, delta);
        });
        m_delayTimer->start(delayMs);
    } else {
        performMouseWheel(position, delta);
    }
}

void InputSimulator::simulateKeyPress(Qt::Key key, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, key]() {
            performKeyPress(key);
        });
        m_delayTimer->start(delayMs);
    } else {
        performKeyPress(key);
    }
}

void InputSimulator::simulateKeyRelease(Qt::Key key, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, key]() {
            performKeyRelease(key);
        });
        m_delayTimer->start(delayMs);
    } else {
        performKeyRelease(key);
    }
}

void InputSimulator::simulateKeySequence(const QString& text, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, text]() {
            performKeySequence(text);
        });
        m_delayTimer->start(delayMs);
    } else {
        performKeySequence(text);
    }
}

void InputSimulator::simulateKeyCombo(const QList<Qt::Key>& keys, int delayMs) {
    if (delayMs > 0) {
        m_delayedActions.enqueue([this, keys]() {
            performKeyCombo(keys);
        });
        m_delayTimer->start(delayMs);
    } else {
        performKeyCombo(keys);
    }
}

void InputSimulator::performMouseClick(const QPoint& position, Qt::MouseButton button) {
#ifdef _WIN32
    // 转换坐标
    POINT pt = {position.x(), position.y()};
    
    // 移动鼠标到目标位�?    SetCursorPos(pt.x, pt.y);
    
    // 模拟鼠标按下和释�?    DWORD downFlag = 0, upFlag = 0;
    switch (button) {
        case Qt::LeftButton:
            downFlag = MOUSEEVENTF_LEFTDOWN;
            upFlag = MOUSEEVENTF_LEFTUP;
            break;
        case Qt::RightButton:
            downFlag = MOUSEEVENTF_RIGHTDOWN;
            upFlag = MOUSEEVENTF_RIGHTUP;
            break;
        case Qt::MiddleButton:
            downFlag = MOUSEEVENTF_MIDDLEDOWN;
            upFlag = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            Logger::warning("Unsupported mouse button");
            return;
    }
    
    mouse_event(downFlag, 0, 0, 0, 0);
    Sleep(10); // 短暂延迟
    mouse_event(upFlag, 0, 0, 0, 0);
    
    Logger::debug("Mouse click simulated at (" + std::to_string(position.x()) + 
                 "," + std::to_string(position.y()) + ")");
#else
    // 对于非Windows平台，使用Qt的事件系�?    QWidget* widget = QApplication::widgetAt(position);
    if (widget) {
        QPoint localPos = widget->mapFromGlobal(position);
        
        QMouseEvent pressEvent(QEvent::MouseButtonPress, localPos, position, button, button, Qt::NoModifier);
        QApplication::sendEvent(widget, &pressEvent);
        
        QMouseEvent releaseEvent(QEvent::MouseButtonRelease, localPos, position, button, Qt::NoButton, Qt::NoModifier);
        QApplication::sendEvent(widget, &releaseEvent);
        
        Logger::debug("Mouse click simulated via Qt events");
    }
#endif
    
    emit mouseClicked(position, button);
}

void InputSimulator::performMouseDoubleClick(const QPoint& position, Qt::MouseButton button) {
    performMouseClick(position, button);
    QThread::msleep(50); // 双击间隔
    performMouseClick(position, button);
    
    Logger::debug("Mouse double click simulated");
    emit mouseDoubleClicked(position, button);
}

void InputSimulator::performMouseMove(const QPoint& position) {
#ifdef _WIN32
    SetCursorPos(position.x(), position.y());
#else
    QCursor::setPos(position);
#endif
    
    Logger::debug("Mouse moved to (" + std::to_string(position.x()) + 
                 "," + std::to_string(position.y()) + ")");
    emit mouseMoved(position);
}

void InputSimulator::performMouseDrag(const QPoint& startPos, const QPoint& endPos, Qt::MouseButton button) {
    // 移动到起始位�?    performMouseMove(startPos);
    QThread::msleep(10);
    
#ifdef _WIN32
    // 按下鼠标
    DWORD downFlag = 0, upFlag = 0;
    switch (button) {
        case Qt::LeftButton:
            downFlag = MOUSEEVENTF_LEFTDOWN;
            upFlag = MOUSEEVENTF_LEFTUP;
            break;
        case Qt::RightButton:
            downFlag = MOUSEEVENTF_RIGHTDOWN;
            upFlag = MOUSEEVENTF_RIGHTUP;
            break;
        case Qt::MiddleButton:
            downFlag = MOUSEEVENTF_MIDDLEDOWN;
            upFlag = MOUSEEVENTF_MIDDLEUP;
            break;
        default:
            return;
    }
    
    mouse_event(downFlag, 0, 0, 0, 0);
    
    // 拖拽到目标位�?    performMouseMove(endPos);
    QThread::msleep(10);
    
    // 释放鼠标
    mouse_event(upFlag, 0, 0, 0, 0);
#endif
    
    Logger::debug("Mouse drag simulated from (" + std::to_string(startPos.x()) + 
                 "," + std::to_string(startPos.y()) + ") to (" + 
                 std::to_string(endPos.x()) + "," + std::to_string(endPos.y()) + ")");
    emit mouseDragged(startPos, endPos, button);
}

void InputSimulator::performMouseWheel(const QPoint& position, int delta) {
#ifdef _WIN32
    SetCursorPos(position.x(), position.y());
    mouse_event(MOUSEEVENTF_WHEEL, 0, 0, delta, 0);
#endif
    
    Logger::debug("Mouse wheel simulated at (" + std::to_string(position.x()) + 
                 "," + std::to_string(position.y()) + ") delta: " + std::to_string(delta));
    emit mouseWheelScrolled(position, delta);
}

void InputSimulator::performKeyPress(Qt::Key key) {
#ifdef _WIN32
    BYTE vkCode = qtKeyToVirtualKey(key);
    if (vkCode != 0) {
        keybd_event(vkCode, 0, 0, 0); // 按下
        keybd_event(vkCode, 0, KEYEVENTF_KEYUP, 0); // 释放
    }
#endif
    
    Logger::debug("Key press simulated: " + std::to_string(static_cast<int>(key)));
    emit keyPressed(key);
}

void InputSimulator::performKeyRelease(Qt::Key key) {
#ifdef _WIN32
    BYTE vkCode = qtKeyToVirtualKey(key);
    if (vkCode != 0) {
        keybd_event(vkCode, 0, KEYEVENTF_KEYUP, 0);
    }
#endif
    
    Logger::debug("Key release simulated: " + std::to_string(static_cast<int>(key)));
    emit keyReleased(key);
}

void InputSimulator::performKeySequence(const QString& text) {
    for (const QChar& ch : text) {
        Qt::Key key = static_cast<Qt::Key>(ch.toUpper().unicode());
        performKeyPress(key);
        QThread::msleep(10); // 短暂延迟
    }
    
    Logger::debug("Key sequence simulated: " + text.toStdString());
    emit keySequenceTyped(text);
}

void InputSimulator::performKeyCombo(const QList<Qt::Key>& keys) {
#ifdef _WIN32
    // 按下所有键
    for (Qt::Key key : keys) {
        BYTE vkCode = qtKeyToVirtualKey(key);
        if (vkCode != 0) {
            keybd_event(vkCode, 0, 0, 0);
        }
    }
    
    QThread::msleep(10);
    
    // 释放所有键（逆序�?    for (auto it = keys.rbegin(); it != keys.rend(); ++it) {
        BYTE vkCode = qtKeyToVirtualKey(*it);
        if (vkCode != 0) {
            keybd_event(vkCode, 0, KEYEVENTF_KEYUP, 0);
        }
    }
#endif
    
    Logger::debug("Key combo simulated with " + std::to_string(keys.size()) + " keys");
    emit keyComboPressed(keys);
}

void InputSimulator::executeDelayedAction() {
    if (!m_delayedActions.isEmpty()) {
        auto action = m_delayedActions.dequeue();
        action();
        
        // 如果还有待执行的动作，继续执�?        if (!m_delayedActions.isEmpty()) {
            m_delayTimer->start(50); // 默认50ms间隔
        }
    }
}

#ifdef _WIN32
BYTE InputSimulator::qtKeyToVirtualKey(Qt::Key key) {
    switch (key) {
        case Qt::Key_A: return 'A';
        case Qt::Key_B: return 'B';
        case Qt::Key_C: return 'C';
        case Qt::Key_D: return 'D';
        case Qt::Key_E: return 'E';
        case Qt::Key_F: return 'F';
        case Qt::Key_G: return 'G';
        case Qt::Key_H: return 'H';
        case Qt::Key_I: return 'I';
        case Qt::Key_J: return 'J';
        case Qt::Key_K: return 'K';
        case Qt::Key_L: return 'L';
        case Qt::Key_M: return 'M';
        case Qt::Key_N: return 'N';
        case Qt::Key_O: return 'O';
        case Qt::Key_P: return 'P';
        case Qt::Key_Q: return 'Q';
        case Qt::Key_R: return 'R';
        case Qt::Key_S: return 'S';
        case Qt::Key_T: return 'T';
        case Qt::Key_U: return 'U';
        case Qt::Key_V: return 'V';
        case Qt::Key_W: return 'W';
        case Qt::Key_X: return 'X';
        case Qt::Key_Y: return 'Y';
        case Qt::Key_Z: return 'Z';
        
        case Qt::Key_0: return '0';
        case Qt::Key_1: return '1';
        case Qt::Key_2: return '2';
        case Qt::Key_3: return '3';
        case Qt::Key_4: return '4';
        case Qt::Key_5: return '5';
        case Qt::Key_6: return '6';
        case Qt::Key_7: return '7';
        case Qt::Key_8: return '8';
        case Qt::Key_9: return '9';
        
        case Qt::Key_Space: return VK_SPACE;
        case Qt::Key_Return: return VK_RETURN;
        case Qt::Key_Enter: return VK_RETURN;
        case Qt::Key_Escape: return VK_ESCAPE;
        case Qt::Key_Tab: return VK_TAB;
        case Qt::Key_Backspace: return VK_BACK;
        case Qt::Key_Delete: return VK_DELETE;
        
        case Qt::Key_Left: return VK_LEFT;
        case Qt::Key_Right: return VK_RIGHT;
        case Qt::Key_Up: return VK_UP;
        case Qt::Key_Down: return VK_DOWN;
        
        case Qt::Key_Control: return VK_CONTROL;
        case Qt::Key_Alt: return VK_MENU;
        case Qt::Key_Shift: return VK_SHIFT;
        
        case Qt::Key_F1: return VK_F1;
        case Qt::Key_F2: return VK_F2;
        case Qt::Key_F3: return VK_F3;
        case Qt::Key_F4: return VK_F4;
        case Qt::Key_F5: return VK_F5;
        case Qt::Key_F6: return VK_F6;
        case Qt::Key_F7: return VK_F7;
        case Qt::Key_F8: return VK_F8;
        case Qt::Key_F9: return VK_F9;
        case Qt::Key_F10: return VK_F10;
        case Qt::Key_F11: return VK_F11;
        case Qt::Key_F12: return VK_F12;
        
        default: return 0;
    }
}
#endif

QPoint InputSimulator::getCurrentMousePosition() {
#ifdef _WIN32
    POINT pt;
    GetCursorPos(&pt);
    return QPoint(pt.x, pt.y);
#else
    return QCursor::pos();
#endif
}

void InputSimulator::setMousePosition(const QPoint& position) {
    performMouseMove(position);
}

bool InputSimulator::isKeyPressed(Qt::Key key) {
#ifdef _WIN32
    BYTE vkCode = qtKeyToVirtualKey(key);
    if (vkCode != 0) {
        return (GetAsyncKeyState(vkCode) & 0x8000) != 0;
    }
#endif
    return false;
}

void InputSimulator::clearDelayedActions() {
    m_delayedActions.clear();
    m_delayTimer->stop();
    Logger::info("Delayed actions cleared");
}

} // namespace oneday::game
