#pragma once

#include <QLabel>
#include <QMainWindow>

namespace oneday {
namespace ui {

// 前向声明
namespace canvas {
class InfiniteCanvas;
}

class NodePalette;
class PropertyPanel;

namespace editors {

/**
 * @brief 最小化的蓝图编辑器窗口（用于调试）
 */
class BlueprintEditorWindowMinimal : public QMainWindow {
    Q_OBJECT

  public:
    explicit BlueprintEditorWindowMinimal(QWidget* parent = nullptr);
    ~BlueprintEditorWindowMinimal() = default;

  private:
    void setupUI();

    // UI组件
    canvas::InfiniteCanvas* m_canvas;
    ui::NodePalette* m_nodePalette;
    ui::PropertyPanel* m_propertyPanel;
    QLabel* m_statusLabel;
};

}  // namespace editors
}  // namespace ui
}  // namespace oneday
