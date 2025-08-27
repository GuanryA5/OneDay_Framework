#pragma once

#include <QColor>
#include <QFont>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include "node_property.h"

namespace oneday {
namespace ui {
namespace canvas {

/**
 * @brief 蓝图节点图形项
 *
 * 在画布上显示的可视化节点，支持：
 * - 节点外观渲染
 * - 文本显示
 * - 选择状态
 * - 鼠标交互
 */
class BlueprintNode : public QGraphicsItem {
  public:
    explicit BlueprintNode(const QString& nodeType,
                           const QString& displayName,
                           QGraphicsItem* parent = nullptr);
    ~BlueprintNode() = default;

    // QGraphicsItem接口
    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    // 节点属性
    QString getNodeType() const {
        return m_nodeType;
    }
    QString getDisplayName() const {
        return m_displayName;
    }
    QString getNodeId() const {
        return m_nodeId;
    }

    void setDisplayName(const QString& name);
    void setSelected(bool selected);
    bool isSelected() const {
        return m_selected;
    }

    // 属性系统
    NodeProperties& getProperties() {
        return m_properties;
    }
    const NodeProperties& getProperties() const {
        return m_properties;
    }
    void setPropertyValue(const QString& name, const QVariant& value);
    QVariant getPropertyValue(const QString& name) const;

    // 节点尺寸
    static constexpr qreal DEFAULT_WIDTH = 120.0;
    static constexpr qreal DEFAULT_HEIGHT = 60.0;
    static constexpr qreal CORNER_RADIUS = 8.0;
    static constexpr qreal TEXT_MARGIN = 10.0;

  protected:
    // 事件处理
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  private:
    void updateColors();
    QColor getNodeColor() const;

    // 节点数据
    QString m_nodeType;
    QString m_displayName;
    QString m_nodeId;
    NodeProperties m_properties;

    // 状态
    bool m_selected;
    bool m_hovered;
    bool m_dragging;
    QPointF m_dragStartPos;

    // 外观
    QColor m_backgroundColor;
    QColor m_borderColor;
    QColor m_textColor;
    QFont m_font;

    // 尺寸
    qreal m_width;
    qreal m_height;
};

/**
 * @brief 节点工厂类
 *
 * 根据节点类型创建对应的节点实例
 */
class NodeFactory {
  public:
    static BlueprintNode* createNode(const QString& nodeType, const QPointF& position);
    static QString getDisplayName(const QString& nodeType);
    static QColor getNodeColor(const QString& nodeType);
};

}  // namespace canvas
}  // namespace ui
}  // namespace oneday
