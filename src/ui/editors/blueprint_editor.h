#pragma once

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QAction>
#include <QMap>
#include <QList>
#include <QGraphicsItem>
#include "../editors/node_palette.h"

namespace oneday::ui {

/**
 * @brief 蓝图编辑器类
 * 
 * 提供可视化的节点编辑界面，用于创建和编辑游戏逻辑蓝图
 */
class BlueprintEditor : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit BlueprintEditor(QWidget* parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~BlueprintEditor();
    
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

public slots:
    /**
     * @brief 新建蓝图
     */
    void newBlueprint();
    
    /**
     * @brief 打开蓝图文件
     */
    void openBlueprint();
    
    /**
     * @brief 保存蓝图文件
     */
    void saveBlueprint();
    
    /**
     * @brief 撤销操作
     */
    void undo();
    
    /**
     * @brief 重做操作
     */
    void redo();
    
    /**
     * @brief 复制选中的项目
     */
    void copySelected();
    
    /**
     * @brief 粘贴项目
     */
    void paste();
    
    /**
     * @brief 删除选中的项目
     */
    void deleteSelected();
    
    /**
     * @brief 运行蓝图
     */
    void runBlueprint();
    
    /**
     * @brief 停止蓝图执行
     */
    void stopBlueprint();
    
    /**
     * @brief 添加节点
     * @param nodeType 节点类型
     */
    void addNode(const QString& nodeType);

signals:
    /**
     * @brief 蓝图内容发生变化
     */
    void blueprintChanged();
    
    /**
     * @brief 选择发生变化
     * @param selectedItems 选中的项目列表
     */
    void selectionChanged(const QList<QGraphicsItem*>& selectedItems);
    
    /**
     * @brief 请求执行蓝图
     */
    void blueprintExecutionRequested();
    
    /**
     * @brief 蓝图执行停止
     */
    void blueprintExecutionStopped();

private slots:
    /**
     * @brief 选择变化处理
     */
    void onSelectionChanged();
    
    /**
     * @brief 场景变化处理
     */
    void onSceneChanged();

private:
    /**
     * @brief 设置用户界面
     */
    void setupUI();
    
    /**
     * @brief 设置工具栏
     * @param layout 布局
     */
    void setupToolBar(QVBoxLayout* layout);
    
    /**
     * @brief 设置信号连接
     */
    void setupConnections();

private:
    QGraphicsScene* m_scene;           ///< 图形场景
    QGraphicsView* m_view;             ///< 图形视图
    NodePalette* m_nodePalette;        ///< 节点面板
    QMap<QString, QAction*> m_actions; ///< 动作映射
    bool m_isModified;                 ///< 是否已修改
};

} // namespace oneday::ui
