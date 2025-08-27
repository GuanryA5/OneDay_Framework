#include "blueprint_node.h"

#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainterPath>
#include <iostream>

namespace oneday {
namespace ui {
namespace canvas {

BlueprintNode::BlueprintNode(const QString& nodeType,
                             const QString& displayName,
                             QGraphicsItem* parent)
    : QGraphicsItem(parent),
      m_nodeType(nodeType),
      m_displayName(displayName),
      m_nodeId(QString("node_%1_%2").arg(nodeType).arg(reinterpret_cast<quintptr>(this))),
      m_properties(NodePropertyFactory::createPropertiesForNodeType(nodeType)),
      m_selected(false),
      m_hovered(false),
      m_dragging(false),
      m_width(DEFAULT_WIDTH),
      m_height(DEFAULT_HEIGHT) {
    // 设置标志
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);

    // 设置字体
    m_font = QFont("Arial", 10);
    m_font.setBold(true);

    // 更新颜色
    updateColors();

    std::cout << "Created BlueprintNode: " << nodeType.toStdString() << " ("
              << displayName.toStdString() << ")" << std::endl;
}

QRectF BlueprintNode::boundingRect() const {
    return QRectF(0, 0, m_width, m_height);
}

void BlueprintNode::paint(QPainter* painter,
                          const QStyleOptionGraphicsItem* option,
                          QWidget* widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    // 创建圆角矩形路径
    QPainterPath path;
    path.addRoundedRect(boundingRect(), CORNER_RADIUS, CORNER_RADIUS);

    // 绘制背景
    painter->fillPath(path, m_backgroundColor);

    // 绘制边框
    QPen borderPen(m_borderColor);
    borderPen.setWidth(m_selected ? 3 : (m_hovered ? 2 : 1));
    painter->setPen(borderPen);
    painter->drawPath(path);

    // 绘制文本
    painter->setPen(m_textColor);
    painter->setFont(m_font);

    QRectF textRect = boundingRect().adjusted(TEXT_MARGIN, TEXT_MARGIN, -TEXT_MARGIN, -TEXT_MARGIN);
    painter->drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, m_displayName);
}

void BlueprintNode::setDisplayName(const QString& name) {
    if (m_displayName != name) {
        m_displayName = name;
        update();
    }
}

void BlueprintNode::setSelected(bool selected) {
    if (m_selected != selected) {
        m_selected = selected;
        updateColors();
        update();
    }
}

void BlueprintNode::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStartPos = event->pos();
        setSelected(true);
        std::cout << "Node selected: " << m_nodeType.toStdString() << std::endl;
    }
    QGraphicsItem::mousePressEvent(event);
}

void BlueprintNode::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        // 移动节点
        QPointF newPos = event->scenePos() - m_dragStartPos;
        setPos(newPos);
        std::cout << "Node moved to: (" << newPos.x() << ", " << newPos.y() << ")" << std::endl;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void BlueprintNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void BlueprintNode::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    Q_UNUSED(event)
    m_hovered = true;
    updateColors();
    update();
}

void BlueprintNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    Q_UNUSED(event)
    m_hovered = false;
    updateColors();
    update();
}

void BlueprintNode::updateColors() {
    m_backgroundColor = getNodeColor();

    if (m_selected) {
        m_borderColor = QColor(255, 165, 0);  // 橙色选中边框
        m_backgroundColor = m_backgroundColor.lighter(120);
    } else if (m_hovered) {
        m_borderColor = QColor(100, 149, 237);  // 蓝色悬停边框
        m_backgroundColor = m_backgroundColor.lighter(110);
    } else {
        m_borderColor = QColor(128, 128, 128);  // 灰色默认边框
    }

    m_textColor = QColor(255, 255, 255);  // 白色文本
}

QColor BlueprintNode::getNodeColor() const {
    return NodeFactory::getNodeColor(m_nodeType);
}

void BlueprintNode::setPropertyValue(const QString& name, const QVariant& value) {
    m_properties.setPropertyValue(name, value);

    // 如果是影响外观的属性，更新显示
    if (name == "name") {
        setDisplayName(value.toString());
    } else if (name == "node_color") {
        updateColors();
        update();
    } else if (name == "show_label") {
        update();
    }
}

QVariant BlueprintNode::getPropertyValue(const QString& name) const {
    return m_properties.getPropertyValue(name);
}

// NodeFactory 实现
BlueprintNode* NodeFactory::createNode(const QString& nodeType, const QPointF& position) {
    QString displayName = getDisplayName(nodeType);
    BlueprintNode* node = new BlueprintNode(nodeType, displayName);
    node->setPos(position);
    return node;
}

QString NodeFactory::getDisplayName(const QString& nodeType) {
    // 根据节点类型返回显示名称
    static QHash<QString, QString> displayNames = {// 事件节点
                                                   {"event_start", "开始"},
                                                   {"event_update", "更新"},
                                                   {"event_keyboard", "键盘输入"},
                                                   {"event_mouse", "鼠标输入"},
                                                   {"event_collision", "碰撞"},

                                                   // 动作节点
                                                   {"action_move", "移动"},
                                                   {"action_rotate", "旋转"},
                                                   {"action_scale", "缩放"},
                                                   {"action_play_sound", "播放声音"},
                                                   {"action_show_text", "显示文本"},
                                                   {"action_create_object", "创建对象"},
                                                   {"action_destroy_object", "销毁对象"},

                                                   // 控制流节点
                                                   {"control_if", "条件判断"},
                                                   {"control_loop", "循环"},
                                                   {"control_delay", "延迟"},
                                                   {"control_sequence", "序列"},
                                                   {"control_parallel", "并行"},
                                                   {"control_select", "选择"},

                                                   // 数据节点
                                                   {"data_variable", "变量"},
                                                   {"data_constant", "常量"},
                                                   {"data_math", "数学运算"},
                                                   {"data_compare", "比较"},
                                                   {"data_logic", "逻辑运算"},
                                                   {"data_convert", "类型转换"},

                                                   // 游戏对象节点
                                                   {"gameobject_get_position", "获取位置"},
                                                   {"gameobject_set_position", "设置位置"},
                                                   {"gameobject_get_property", "获取属性"},
                                                   {"gameobject_set_property", "设置属性"},
                                                   {"gameobject_find", "查找对象"},

                                                   // AI节点
                                                   {"ai_pathfinding", "路径查找"},
                                                   {"ai_state_machine", "状态机"},
                                                   {"ai_behavior_tree", "行为树"},
                                                   {"ai_decision", "决策"},
                                                   {"ai_perception", "感知"},

                                                   // 图像处理节点
                                                   {"image_screenshot", "截图"},
                                                   {"image_template_match", "模板匹配"},
                                                   {"image_color_detect", "颜色检测"},
                                                   {"image_edge_detect", "边缘检测"},
                                                   {"image_filter", "图像滤波"}};

    return displayNames.value(nodeType, nodeType);
}

QColor NodeFactory::getNodeColor(const QString& nodeType) {
    // 根据节点类型返回颜色
    if (nodeType.startsWith("event_")) {
        return QColor(220, 20, 60);  // 深红色 - 事件
    } else if (nodeType.startsWith("action_")) {
        return QColor(30, 144, 255);  // 蓝色 - 动作
    } else if (nodeType.startsWith("control_")) {
        return QColor(50, 205, 50);  // 绿色 - 控制流
    } else if (nodeType.startsWith("data_")) {
        return QColor(255, 140, 0);  // 橙色 - 数据
    } else if (nodeType.startsWith("gameobject_")) {
        return QColor(138, 43, 226);  // 紫色 - 游戏对象
    } else if (nodeType.startsWith("ai_")) {
        return QColor(255, 20, 147);  // 深粉色 - AI
    } else if (nodeType.startsWith("image_")) {
        return QColor(0, 191, 255);  // 天蓝色 - 图像处理
    } else {
        return QColor(128, 128, 128);  // 灰色 - 默认
    }
}

}  // namespace canvas
}  // namespace ui
}  // namespace oneday
