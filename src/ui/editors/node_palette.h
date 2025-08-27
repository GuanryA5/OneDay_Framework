#pragma once

#include <QStringList>
#include <QWidget>

class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;
class QTextEdit;

namespace oneday::ui {

/**
 * @brief 节点面板类
 *
 * 提供可用节点类型的浏览和搜索界面
 */
class NodePalette : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit NodePalette(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~NodePalette();

    /**
     * @brief 获取所有可用的节点类型
     * @return 节点类型列表
     */
    QStringList getAvailableNodeTypes() const;

  public slots:
    /**
     * @brief 展开所有分类
     */
    void expandAll();

    /**
     * @brief 折叠所有分类
     */
    void collapseAll();

    /**
     * @brief 清空搜索
     */
    void clearSearch();

  signals:
    /**
     * @brief 请求创建节点
     * @param nodeType 节点类型
     */
    void nodeRequested(const QString& nodeType);

  private slots:
    /**
     * @brief 过滤节点
     * @param filter 过滤文本
     */
    void filterNodes(const QString& filter);

    /**
     * @brief 节点选择变化处理
     */
    void onNodeSelectionChanged();

    /**
     * @brief 节点双击处理
     * @param item 被双击的项目
     * @param column 列索引
     */
    void onNodeDoubleClicked(QTreeWidgetItem* item, int column);

  private:
    /**
     * @brief 设置用户界面
     */
    void setupUI();

    /**
     * @brief 设置信号连接
     */
    void setupConnections();

    /**
     * @brief 填充节点树
     */
    void populateNodeTree();

    /**
     * @brief 添加节点项目
     * @param parent 父项目
     * @param name 节点名称
     * @param description 节点描述
     * @param type 节点类型
     */
    void addNodeItem(QTreeWidgetItem* parent,
                     const QString& name,
                     const QString& description,
                     const QString& type);

    /**
     * @brief 显示所有项目
     * @param item 根项目
     */
    void showAllItems(QTreeWidgetItem* item);

    /**
     * @brief 过滤项目
     * @param item 当前项目
     * @param filter 过滤文本
     * @return 是否有可见的子项目
     */
    bool filterItems(QTreeWidgetItem* item, const QString& filter);

    /**
     * @brief 收集节点类型
     * @param item 当前项目
     * @param types 类型列表
     */
    void collectNodeTypes(QTreeWidgetItem* item, QStringList& types) const;

  private:
    QLineEdit* m_searchEdit;       ///< 搜索输入框
    QTreeWidget* m_nodeTree;       ///< 节点树
    QTextEdit* m_descriptionText;  ///< 描述文本
};

}  // namespace oneday::ui
