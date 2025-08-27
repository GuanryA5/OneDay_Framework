#include "node_palette.h"

#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QMouseEvent>
#include <QSplitter>
#include <QTextEdit>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <iostream>

// 自定义QTreeWidget来处理拖拽
class DraggableTreeWidget : public QTreeWidget {
  public:
    explicit DraggableTreeWidget(QWidget* parent = nullptr)
        : QTreeWidget(parent), m_dragging(false) {}

  protected:
    void mousePressEvent(QMouseEvent* event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragStartPosition = event->pos();
            m_dragging = false;
        }
        QTreeWidget::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        if (!(event->buttons() & Qt::LeftButton)) {
            return;
        }

        if (m_dragging) {
            return;  // 已经在拖拽中
        }

        if ((event->pos() - m_dragStartPosition).manhattanLength() <
            QApplication::startDragDistance()) {
            return;
        }

        QTreeWidgetItem* item = itemAt(m_dragStartPosition);
        if (!item) {
            return;
        }

        QString nodeType = item->data(0, Qt::UserRole).toString();
        if (nodeType.isEmpty()) {
            return;  // 不能拖拽分类节点
        }

        m_dragging = true;

        QDrag* drag = new QDrag(this);
        QMimeData* mimeData = new QMimeData;

        mimeData->setText(nodeType);
        mimeData->setData("application/x-node-type", nodeType.toUtf8());
        drag->setMimeData(mimeData);

        std::cout << "Starting drag for node: " << nodeType.toStdString() << std::endl;
        drag->exec(Qt::CopyAction);

        m_dragging = false;
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        m_dragging = false;
        QTreeWidget::mouseReleaseEvent(event);
    }

  private:
    QPoint m_dragStartPosition;
    bool m_dragging;
};

namespace oneday::ui {

NodePalette::NodePalette(QWidget* parent)
    : QWidget(parent),
      m_searchEdit(new QLineEdit(this)),
      m_nodeTree(new DraggableTreeWidget(this)),
      m_descriptionText(new QTextEdit(this)) {
    setupUI();
    setupConnections();
    populateNodeTree();
}

NodePalette::~NodePalette() = default;

void NodePalette::setupUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    // 搜索框
    auto* searchGroup = new QGroupBox("搜索", this);
    auto* searchLayout = new QVBoxLayout(searchGroup);

    m_searchEdit->setPlaceholderText("搜索节点...");
    searchLayout->addWidget(m_searchEdit);

    layout->addWidget(searchGroup);

    // 分割器
    auto* splitter = new QSplitter(Qt::Vertical, this);

    // 节点树
    auto* treeGroup = new QGroupBox("节点类型", this);
    auto* treeLayout = new QVBoxLayout(treeGroup);

    m_nodeTree->setHeaderHidden(true);
    m_nodeTree->setSelectionMode(QAbstractItemView::SingleSelection);

    treeLayout->addWidget(m_nodeTree);
    splitter->addWidget(treeGroup);

    // 描述文本
    auto* descGroup = new QGroupBox("描述", this);
    auto* descLayout = new QVBoxLayout(descGroup);

    m_descriptionText->setReadOnly(true);
    m_descriptionText->setMaximumHeight(100);
    m_descriptionText->setPlainText("选择一个节点查看详细描述");

    descLayout->addWidget(m_descriptionText);
    splitter->addWidget(descGroup);

    // 设置分割器比例
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter);
}

void NodePalette::setupConnections() {
    // 搜索框连接
    connect(m_searchEdit, &QLineEdit::textChanged, this, &NodePalette::filterNodes);

    // 节点树连接
    connect(
        m_nodeTree, &QTreeWidget::itemSelectionChanged, this, &NodePalette::onNodeSelectionChanged);
    connect(m_nodeTree, &QTreeWidget::itemDoubleClicked, this, &NodePalette::onNodeDoubleClicked);
}

void NodePalette::populateNodeTree() {
    m_nodeTree->clear();

    // 事件节点
    auto* eventCategory = new QTreeWidgetItem(m_nodeTree, QStringList("事件"));
    eventCategory->setExpanded(true);

    addNodeItem(eventCategory, "开始", "游戏开始事件", "event_start");
    addNodeItem(eventCategory, "更新", "每帧更新事件", "event_update");
    addNodeItem(eventCategory, "键盘输入", "键盘按键事件", "event_keyboard");
    addNodeItem(eventCategory, "鼠标输入", "鼠标点击事件", "event_mouse");
    addNodeItem(eventCategory, "碰撞", "物体碰撞事件", "event_collision");

    // 动作节点
    auto* actionCategory = new QTreeWidgetItem(m_nodeTree, QStringList("动作"));
    actionCategory->setExpanded(true);

    addNodeItem(actionCategory, "移动", "移动物体到指定位置", "action_move");
    addNodeItem(actionCategory, "旋转", "旋转物体", "action_rotate");
    addNodeItem(actionCategory, "缩放", "缩放物体", "action_scale");
    addNodeItem(actionCategory, "播放声音", "播放音效", "action_play_sound");
    addNodeItem(actionCategory, "显示文本", "显示文本信息", "action_show_text");
    addNodeItem(actionCategory, "创建对象", "创建新的游戏对象", "action_create_object");
    addNodeItem(actionCategory, "销毁对象", "销毁游戏对象", "action_destroy_object");

    // 控制流节点
    auto* controlCategory = new QTreeWidgetItem(m_nodeTree, QStringList("控制流"));
    controlCategory->setExpanded(true);

    addNodeItem(controlCategory, "条件判断", "if-else条件分支", "control_if");
    addNodeItem(controlCategory, "循环", "for/while循环", "control_loop");
    addNodeItem(controlCategory, "延迟", "延迟执行", "control_delay");
    addNodeItem(controlCategory, "序列", "按顺序执行多个动作", "control_sequence");
    addNodeItem(controlCategory, "并行", "同时执行多个动作", "control_parallel");
    addNodeItem(controlCategory, "选择", "随机或条件选择", "control_select");

    // 数据节点
    auto* dataCategory = new QTreeWidgetItem(m_nodeTree, QStringList("数据"));
    dataCategory->setExpanded(true);

    addNodeItem(dataCategory, "变量", "存储和获取变量值", "data_variable");
    addNodeItem(dataCategory, "常量", "常量值", "data_constant");
    addNodeItem(dataCategory, "数学运算", "加减乘除等运算", "data_math");
    addNodeItem(dataCategory, "比较", "大小比较", "data_compare");
    addNodeItem(dataCategory, "逻辑运算", "与或非逻辑运算", "data_logic");
    addNodeItem(dataCategory, "类型转换", "数据类型转换", "data_convert");

    // 游戏对象节点
    auto* gameObjectCategory = new QTreeWidgetItem(m_nodeTree, QStringList("游戏对象"));
    gameObjectCategory->setExpanded(true);

    addNodeItem(gameObjectCategory, "获取位置", "获取对象位置", "gameobject_get_position");
    addNodeItem(gameObjectCategory, "设置位置", "设置对象位置", "gameobject_set_position");
    addNodeItem(gameObjectCategory, "获取属性", "获取对象属性", "gameobject_get_property");
    addNodeItem(gameObjectCategory, "设置属性", "设置对象属性", "gameobject_set_property");
    addNodeItem(gameObjectCategory, "查找对象", "按名称或标签查找对象", "gameobject_find");

    // AI节点
    auto* aiCategory = new QTreeWidgetItem(m_nodeTree, QStringList("AI"));
    aiCategory->setExpanded(true);

    addNodeItem(aiCategory, "路径查找", "A*路径查找", "ai_pathfinding");
    addNodeItem(aiCategory, "状态机", "有限状态机", "ai_state_machine");
    addNodeItem(aiCategory, "行为树", "行为树节点", "ai_behavior_tree");
    addNodeItem(aiCategory, "决策", "AI决策节点", "ai_decision");
    addNodeItem(aiCategory, "感知", "AI感知系统", "ai_perception");

    // 图像处理节点
    auto* imageCategory = new QTreeWidgetItem(m_nodeTree, QStringList("图像处理"));
    imageCategory->setExpanded(true);

    addNodeItem(imageCategory, "截图", "屏幕截图", "image_screenshot");
    addNodeItem(imageCategory, "模板匹配", "图像模板匹配", "image_template_match");
    addNodeItem(imageCategory, "颜色检测", "颜色区域检测", "image_color_detect");
    addNodeItem(imageCategory, "边缘检测", "图像边缘检测", "image_edge_detect");
    addNodeItem(imageCategory, "图像滤波", "图像滤波处理", "image_filter");
}

void NodePalette::addNodeItem(QTreeWidgetItem* parent,
                              const QString& name,
                              const QString& description,
                              const QString& type) {
    auto* item = new QTreeWidgetItem(parent, QStringList(name));
    item->setData(0, Qt::UserRole, type);
    item->setData(0, Qt::UserRole + 1, description);
    item->setToolTip(0, description);

    // 设置图标（如果有的话）
    // item->setIcon(0, QIcon(":/icons/" + type + ".png"));
}

void NodePalette::filterNodes(const QString& filter) {
    if (filter.isEmpty()) {
        // 显示所有节点
        showAllItems(m_nodeTree->invisibleRootItem());
    } else {
        // 隐藏不匹配的节点
        filterItems(m_nodeTree->invisibleRootItem(), filter.toLower());
    }
}

void NodePalette::showAllItems(QTreeWidgetItem* item) {
    item->setHidden(false);

    for (int i = 0; i < item->childCount(); ++i) {
        showAllItems(item->child(i));
    }
}

bool NodePalette::filterItems(QTreeWidgetItem* item, const QString& filter) {
    bool hasVisibleChild = false;

    // 递归处理子项
    for (int i = 0; i < item->childCount(); ++i) {
        if (filterItems(item->child(i), filter)) {
            hasVisibleChild = true;
        }
    }

    // 检查当前项是否匹配
    bool matches = item->text(0).toLower().contains(filter) ||
                   item->data(0, Qt::UserRole + 1).toString().toLower().contains(filter);

    // 如果是叶子节点，根据匹配结果决定是否显示
    if (item->childCount() == 0) {
        item->setHidden(!matches);
        return matches;
    }

    // 如果是分类节点，只要有可见子项或自身匹配就显示
    bool shouldShow = hasVisibleChild || matches;
    item->setHidden(!shouldShow);

    return shouldShow;
}

void NodePalette::onNodeSelectionChanged() {
    auto selectedItems = m_nodeTree->selectedItems();
    if (selectedItems.isEmpty()) {
        m_descriptionText->setPlainText("选择一个节点查看详细描述");
        return;
    }

    auto* item = selectedItems.first();
    QString description = item->data(0, Qt::UserRole + 1).toString();
    QString type = item->data(0, Qt::UserRole).toString();

    if (!description.isEmpty()) {
        QString fullDescription =
            QString("节点类型: %1\n\n描述: %2").arg(item->text(0)).arg(description);

        if (!type.isEmpty()) {
            fullDescription += QString("\n\n内部类型: %1").arg(type);
        }

        m_descriptionText->setPlainText(fullDescription);
    } else {
        m_descriptionText->setPlainText("这是一个节点分类");
    }
}

void NodePalette::onNodeDoubleClicked(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column)

    std::cout << "Double click detected on item: " << item->text(0).toStdString() << std::endl;

    QString type = item->data(0, Qt::UserRole).toString();
    std::cout << "Node type: " << type.toStdString() << std::endl;

    if (!type.isEmpty()) {
        std::cout << "Emitting nodeRequested signal for: " << type.toStdString() << std::endl;
        emit nodeRequested(type);
    } else {
        std::cout << "Node type is empty, not emitting signal" << std::endl;
    }
}

QStringList NodePalette::getAvailableNodeTypes() const {
    QStringList types;
    collectNodeTypes(m_nodeTree->invisibleRootItem(), types);
    return types;
}

void NodePalette::collectNodeTypes(QTreeWidgetItem* item, QStringList& types) const {
    QString type = item->data(0, Qt::UserRole).toString();
    if (!type.isEmpty()) {
        types.append(type);
    }

    for (int i = 0; i < item->childCount(); ++i) {
        collectNodeTypes(item->child(i), types);
    }
}

void NodePalette::expandAll() {
    m_nodeTree->expandAll();
}

void NodePalette::collapseAll() {
    m_nodeTree->collapseAll();
}

void NodePalette::clearSearch() {
    m_searchEdit->clear();
}

}  // namespace oneday::ui
