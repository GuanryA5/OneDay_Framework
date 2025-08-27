#pragma once

#include <QAction>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

namespace oneday::ui {

/**
 * @brief 地图编辑器类
 *
 * 提供可视化的地图编辑界面，用于创建和编辑游戏地图
 */
class MapEditor : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief 编辑工具枚举
     */
    enum class Tool {
        Select,     ///< 选择工具
        Rectangle,  ///< 矩形工具
        Circle,     ///< 圆形工具
        Line,       ///< 直线工具
        Path,       ///< 路径工具
        Obstacle    ///< 障碍物工具
    };

    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit MapEditor(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~MapEditor();

    /**
     * @brief 检查是否有未保存的修改
     * @return 是否已修改
     */
    bool isModified() const;

    /**
     * @brief 获取图形场景
     * @return 图形场景指针
     */
    QGraphicsScene* scene() const;

    /**
     * @brief 获取图形视图
     * @return 图形视图指针
     */
    QGraphicsView* view() const;

    /**
     * @brief 获取当前工具
     * @return 当前工具
     */
    Tool currentTool() const;

  public slots:
    /**
     * @brief 设置当前工具
     * @param tool 工具类型
     */
    void setCurrentTool(Tool tool);

    /**
     * @brief 设置是否显示网格
     * @param show 是否显示
     */
    void setShowGrid(bool show);

    /**
     * @brief 设置是否对齐网格
     * @param snap 是否对齐
     */
    void setSnapToGrid(bool snap);

    /**
     * @brief 设置网格大小
     * @param size 网格大小
     */
    void setGridSize(int size);

    /**
     * @brief 新建地图
     */
    void newMap();

    /**
     * @brief 打开地图文件
     */
    void openMap();

    /**
     * @brief 保存地图文件
     */
    void saveMap();

    /**
     * @brief 撤销操作
     */
    void undo();

    /**
     * @brief 重做操作
     */
    void redo();

    /**
     * @brief 放大视图
     */
    void zoomIn();

    /**
     * @brief 缩小视图
     */
    void zoomOut();

    /**
     * @brief 适应窗口
     */
    void fitInView();

  signals:
    /**
     * @brief 地图内容发生变化
     */
    void mapChanged();

    /**
     * @brief 工具发生变化
     * @param tool 新工具
     */
    void toolChanged(Tool tool);

    /**
     * @brief 选择发生变化
     * @param selectedItems 选中的项目列表
     */
    void selectionChanged(const QList<QGraphicsItem*>& selectedItems);

  private slots:
    /**
     * @brief 选择变化处理
     */
    void onSelectionChanged();

    /**
     * @brief 场景变化处理
     */
    void onSceneChanged();

    /**
     * @brief 选择线条颜色
     */
    void chooseLineColor();

    /**
     * @brief 选择填充颜色
     */
    void chooseFillColor();

  private:
    /**
     * @brief 设置用户界面
     */
    void setupUI();

    /**
     * @brief 设置工具面板
     * @param mainLayout 主布局
     */
    void setupToolPanel(QHBoxLayout* mainLayout);

    /**
     * @brief 设置工具栏
     * @param layout 布局
     */
    void setupToolBar(QVBoxLayout* layout);

    /**
     * @brief 设置信号连接
     */
    void setupConnections();

    /**
     * @brief 更新网格显示
     */
    void updateGrid();

    /**
     * @brief 对齐到网格
     * @param point 原始点
     * @return 对齐后的点
     */
    QPointF snapToGrid(const QPointF& point);

  private:
    // 核心组件
    QGraphicsScene* m_scene;  ///< 图形场景
    QGraphicsView* m_view;    ///< 图形视图

    // 工具和状态
    Tool m_currentTool;  ///< 当前工具
    bool m_isModified;   ///< 是否已修改

    // 网格设置
    int m_gridSize;                     ///< 网格大小
    bool m_showGrid;                    ///< 是否显示网格
    bool m_snapToGrid;                  ///< 是否对齐网格
    QList<QGraphicsItem*> m_gridItems;  ///< 网格图形项目

    // 绘制属性
    QColor m_lineColor = Qt::black;      ///< 线条颜色
    QColor m_fillColor = Qt::lightGray;  ///< 填充颜色

    // UI组件
    QMap<QString, QPushButton*> m_toolButtons;  ///< 工具按钮
    QMap<QString, QAction*> m_actions;          ///< 动作映射

    // 控件
    QCheckBox* m_showGridCheck;      ///< 显示网格复选框
    QCheckBox* m_snapToGridCheck;    ///< 对齐网格复选框
    QSpinBox* m_gridSizeSpinBox;     ///< 网格大小输入框
    QSpinBox* m_lineWidthSpinBox;    ///< 线条宽度输入框
    QPushButton* m_lineColorButton;  ///< 线条颜色按钮
    QPushButton* m_fillColorButton;  ///< 填充颜色按钮
    QComboBox* m_layerComboBox;      ///< 图层选择框
};

}  // namespace oneday::ui
