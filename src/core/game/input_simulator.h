#pragma once

#include <QObject>
#include <QPoint>
#include <QList>
#include <QQueue>
#include <QTimer>
#include <QThread>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#endif

namespace oneday::game {

/**
 * @brief 输入模拟器类
 * 
 * 提供鼠标和键盘输入的模拟功能
 */
class InputSimulator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit InputSimulator(QObject* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~InputSimulator();
    
    // === 鼠标模拟 ===
    
    /**
     * @brief 模拟鼠标点击
     * @param position 点击位置
     * @param button 鼠标按钮
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateMouseClick(const QPoint& position, Qt::MouseButton button = Qt::LeftButton, int delayMs = 0);
    
    /**
     * @brief 模拟鼠标双击
     * @param position 点击位置
     * @param button 鼠标按钮
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateMouseDoubleClick(const QPoint& position, Qt::MouseButton button = Qt::LeftButton, int delayMs = 0);
    
    /**
     * @brief 模拟鼠标移动
     * @param position 目标位置
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateMouseMove(const QPoint& position, int delayMs = 0);
    
    /**
     * @brief 模拟鼠标拖拽
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @param button 鼠标按钮
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateMouseDrag(const QPoint& startPos, const QPoint& endPos, 
                          Qt::MouseButton button = Qt::LeftButton, int delayMs = 0);
    
    /**
     * @brief 模拟鼠标滚轮
     * @param position 滚轮位置
     * @param delta 滚动量
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateMouseWheel(const QPoint& position, int delta, int delayMs = 0);
    
    // === 键盘模拟 ===
    
    /**
     * @brief 模拟按键按下
     * @param key 按键
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateKeyPress(Qt::Key key, int delayMs = 0);
    
    /**
     * @brief 模拟按键释放
     * @param key 按键
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateKeyRelease(Qt::Key key, int delayMs = 0);
    
    /**
     * @brief 模拟输入文本序列
     * @param text 文本内容
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateKeySequence(const QString& text, int delayMs = 0);
    
    /**
     * @brief 模拟组合键
     * @param keys 按键列表
     * @param delayMs 延迟时间（毫秒）
     */
    void simulateKeyCombo(const QList<Qt::Key>& keys, int delayMs = 0);
    
    // === 工具函数 ===
    
    /**
     * @brief 获取当前鼠标位置
     * @return 鼠标位置
     */
    static QPoint getCurrentMousePosition();
    
    /**
     * @brief 设置鼠标位置
     * @param position 目标位置
     */
    void setMousePosition(const QPoint& position);
    
    /**
     * @brief 检查按键是否被按下
     * @param key 按键
     * @return 是否被按下
     */
    static bool isKeyPressed(Qt::Key key);
    
    /**
     * @brief 清除所有延迟动作
     */
    void clearDelayedActions();

signals:
    /**
     * @brief 鼠标点击信号
     * @param position 点击位置
     * @param button 鼠标按钮
     */
    void mouseClicked(const QPoint& position, Qt::MouseButton button);
    
    /**
     * @brief 鼠标双击信号
     * @param position 点击位置
     * @param button 鼠标按钮
     */
    void mouseDoubleClicked(const QPoint& position, Qt::MouseButton button);
    
    /**
     * @brief 鼠标移动信号
     * @param position 新位置
     */
    void mouseMoved(const QPoint& position);
    
    /**
     * @brief 鼠标拖拽信号
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @param button 鼠标按钮
     */
    void mouseDragged(const QPoint& startPos, const QPoint& endPos, Qt::MouseButton button);
    
    /**
     * @brief 鼠标滚轮信号
     * @param position 滚轮位置
     * @param delta 滚动量
     */
    void mouseWheelScrolled(const QPoint& position, int delta);
    
    /**
     * @brief 按键按下信号
     * @param key 按键
     */
    void keyPressed(Qt::Key key);
    
    /**
     * @brief 按键释放信号
     * @param key 按键
     */
    void keyReleased(Qt::Key key);
    
    /**
     * @brief 文本输入信号
     * @param text 输入的文本
     */
    void keySequenceTyped(const QString& text);
    
    /**
     * @brief 组合键按下信号
     * @param keys 按键列表
     */
    void keyComboPressed(const QList<Qt::Key>& keys);

private slots:
    /**
     * @brief 执行延迟动作
     */
    void executeDelayedAction();

private:
    /**
     * @brief 执行鼠标点击
     * @param position 点击位置
     * @param button 鼠标按钮
     */
    void performMouseClick(const QPoint& position, Qt::MouseButton button);
    
    /**
     * @brief 执行鼠标双击
     * @param position 点击位置
     * @param button 鼠标按钮
     */
    void performMouseDoubleClick(const QPoint& position, Qt::MouseButton button);
    
    /**
     * @brief 执行鼠标移动
     * @param position 目标位置
     */
    void performMouseMove(const QPoint& position);
    
    /**
     * @brief 执行鼠标拖拽
     * @param startPos 起始位置
     * @param endPos 结束位置
     * @param button 鼠标按钮
     */
    void performMouseDrag(const QPoint& startPos, const QPoint& endPos, Qt::MouseButton button);
    
    /**
     * @brief 执行鼠标滚轮
     * @param position 滚轮位置
     * @param delta 滚动量
     */
    void performMouseWheel(const QPoint& position, int delta);
    
    /**
     * @brief 执行按键按下
     * @param key 按键
     */
    void performKeyPress(Qt::Key key);
    
    /**
     * @brief 执行按键释放
     * @param key 按键
     */
    void performKeyRelease(Qt::Key key);
    
    /**
     * @brief 执行文本序列输入
     * @param text 文本内容
     */
    void performKeySequence(const QString& text);
    
    /**
     * @brief 执行组合键
     * @param keys 按键列表
     */
    void performKeyCombo(const QList<Qt::Key>& keys);

#ifdef _WIN32
    /**
     * @brief Qt按键转换为Windows虚拟键码
     * @param key Qt按键
     * @return Windows虚拟键码
     */
    static BYTE qtKeyToVirtualKey(Qt::Key key);
#endif

private:
    QTimer* m_delayTimer;                           ///< 延迟定时器
    QQueue<std::function<void()>> m_delayedActions; ///< 延迟动作队列
};

} // namespace oneday::game
