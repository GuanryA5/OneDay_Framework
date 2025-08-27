#include "infinite_canvas.h"

#include <QApplication>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QMimeData>
#include <QPainter>
#include <QScrollBar>
#include <QtMath>
#include <iostream>

#include "blueprint_node.h"

namespace oneday {
namespace ui {
namespace canvas {

InfiniteCanvas::InfiniteCanvas(QWidget* parent)
    : QGraphicsView(parent),
      m_scene(nullptr),
      m_zoomLevel(1.0),
      m_minZoom(0.1),
      m_maxZoom(10.0),
      m_zoomStep(1.2),
      m_gridVisible(true),
      m_gridSize(20),
      m_gridColor(QColor(100, 100, 100, 100)),
      m_gridColorMajor(QColor(150, 150, 150, 150)),
      m_snapToGrid(false),
      m_isPanning(false),
      m_panButton(Qt::MiddleButton),
      m_zoomTimer(nullptr),
      m_targetZoom(1.0),
      m_fastRender(false),
      m_updateTimer(nullptr) {
    std::cout << "Creating InfiniteCanvas..." << std::endl;

    try {
        setupCanvas();
        setupScene();
        std::cout << "InfiniteCanvas created successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error creating InfiniteCanvas: " << e.what() << std::endl;
        throw;
    }
}

void InfiniteCanvas::setupCanvas() {
    std::cout << "Setting up canvas properties..." << std::endl;

    // 设置视图属性
    setDragMode(QGraphicsView::NoDrag);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    // 设置背景
    setBackgroundBrush(QBrush(QColor(45, 45, 45)));

    // 启用拖拽接收
    setAcceptDrops(true);

    // 初始化定时器
    m_zoomTimer = new QTimer(this);
    m_zoomTimer->setSingleShot(true);
    m_zoomTimer->setInterval(16);  // ~60 FPS

    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(16);

    std::cout << "Canvas properties set" << std::endl;
}

void InfiniteCanvas::setupScene() {
    std::cout << "Setting up scene..." << std::endl;

    // 创建场景
    m_scene = new QGraphicsScene(this);

    // 设置场景大小为一个很大的矩形，模拟无限画布
    const qreal sceneSize = 1000000.0;  // 100万像素的场景
    m_scene->setSceneRect(-sceneSize / 2, -sceneSize / 2, sceneSize, sceneSize);

    // 连接场景信号
    connect(m_scene, &QGraphicsScene::changed, this, &InfiniteCanvas::onSceneChanged);

    // 设置场景到视图
    setScene(m_scene);

    std::cout << "Scene setup complete" << std::endl;
}

void InfiniteCanvas::setZoomLevel(qreal zoomLevel) {
    // 限制缩放范围
    zoomLevel = qBound(m_minZoom, zoomLevel, m_maxZoom);

    if (qAbs(zoomLevel - m_zoomLevel) < 0.001) {
        return;  // 避免微小变化
    }

    qreal scaleFactor = zoomLevel / m_zoomLevel;
    m_zoomLevel = zoomLevel;

    // 应用缩放
    scale(scaleFactor, scaleFactor);

    // 发出信号
    emit zoomLevelChanged(m_zoomLevel);

    std::cout << "Zoom level set to: " << m_zoomLevel << std::endl;
}

void InfiniteCanvas::zoomIn() {
    setZoomLevel(m_zoomLevel * m_zoomStep);
}

void InfiniteCanvas::zoomOut() {
    setZoomLevel(m_zoomLevel / m_zoomStep);
}

void InfiniteCanvas::zoomToFit() {
    // 缩放到适合所有内容
    if (m_scene->items().isEmpty()) {
        setZoomLevel(1.0);
        return;
    }

    QRectF itemsRect = m_scene->itemsBoundingRect();
    fitInView(itemsRect, Qt::KeepAspectRatio);

    // 更新缩放级别
    QTransform transform = this->transform();
    m_zoomLevel = transform.m11();  // 获取X轴缩放因子

    emit zoomLevelChanged(m_zoomLevel);
}

void InfiniteCanvas::zoomToActualSize() {
    setZoomLevel(1.0);
}

void InfiniteCanvas::setGridVisible(bool visible) {
    if (m_gridVisible != visible) {
        m_gridVisible = visible;
        viewport()->update();
        std::cout << "Grid visibility set to: " << (visible ? "true" : "false") << std::endl;
    }
}

void InfiniteCanvas::setGridSize(int size) {
    if (size > 0 && m_gridSize != size) {
        m_gridSize = size;
        if (m_gridVisible) {
            viewport()->update();
        }
        std::cout << "Grid size set to: " << size << std::endl;
    }
}

void InfiniteCanvas::setSnapToGrid(bool snap) {
    if (m_snapToGrid != snap) {
        m_snapToGrid = snap;
        std::cout << "Snap to grid set to: " << (snap ? "true" : "false") << std::endl;
    }
}

QPointF InfiniteCanvas::mapToCanvas(const QPoint& viewPoint) const {
    return mapToScene(viewPoint);
}

QPoint InfiniteCanvas::mapFromCanvas(const QPointF& canvasPoint) const {
    return mapFromScene(canvasPoint);
}

void InfiniteCanvas::createNode(const QString& nodeType, const QPointF& position) {
    std::cout << "Creating node: " << nodeType.toStdString() << " at (" << position.x() << ", "
              << position.y() << ")" << std::endl;

    // 使用节点工厂创建节点
    BlueprintNode* node = NodeFactory::createNode(nodeType, position);

    // 添加到场景
    m_scene->addItem(node);

    std::cout << "Node created and added to scene" << std::endl;
}

// 事件处理
void InfiniteCanvas::wheelEvent(QWheelEvent* event) {
    // 鼠标滚轮缩放
    if (event->modifiers() & Qt::ControlModifier) {
        // Ctrl + 滚轮：缩放
        const qreal scaleFactor = event->angleDelta().y() > 0 ? m_zoomStep : 1.0 / m_zoomStep;
        setZoomLevel(m_zoomLevel * scaleFactor);
        event->accept();
    } else {
        // 普通滚轮：滚动
        QGraphicsView::wheelEvent(event);
    }
}

void InfiniteCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() == m_panButton ||
        (event->button() == Qt::LeftButton && event->modifiers() & Qt::AltModifier)) {
        // 开始平移
        m_isPanning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);

        if (event->button() == Qt::LeftButton) {
            QPointF scenePos = mapToScene(event->pos());
            emit canvasClicked(scenePos);
        }
    }
}

void InfiniteCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (m_isPanning) {
        // 执行平移
        QPoint delta = event->pos() - m_lastPanPoint;
        m_lastPanPoint = event->pos();

        // 移动视图
        QScrollBar* hBar = horizontalScrollBar();
        QScrollBar* vBar = verticalScrollBar();
        hBar->setValue(hBar->value() - delta.x());
        vBar->setValue(vBar->value() - delta.y());

        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void InfiniteCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (m_isPanning &&
        (event->button() == m_panButton || (event->button() == Qt::LeftButton && m_isPanning))) {
        // 结束平移
        m_isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void InfiniteCanvas::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        QPointF scenePos = mapToScene(event->pos());
        emit canvasDoubleClicked(scenePos);
        event->accept();
    } else {
        QGraphicsView::mouseDoubleClickEvent(event);
    }
}

void InfiniteCanvas::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Plus:
        case Qt::Key_Equal:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomIn();
                event->accept();
                return;
            }
            break;

        case Qt::Key_Minus:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomOut();
                event->accept();
                return;
            }
            break;

        case Qt::Key_0:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomToActualSize();
                event->accept();
                return;
            }
            break;

        case Qt::Key_F:
            if (event->modifiers() & Qt::ControlModifier) {
                zoomToFit();
                event->accept();
                return;
            }
            break;
    }

    QGraphicsView::keyPressEvent(event);
}

void InfiniteCanvas::paintEvent(QPaintEvent* event) {
    QGraphicsView::paintEvent(event);
}

void InfiniteCanvas::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    updateViewport();
}

void InfiniteCanvas::drawBackground(QPainter* painter, const QRectF& rect) {
    // 绘制背景色
    painter->fillRect(rect, backgroundBrush());

    // 绘制网格
    if (m_gridVisible) {
        drawGrid(painter, rect);
    }
}

void InfiniteCanvas::drawGrid(QPainter* painter, const QRectF& rect) {
    painter->save();

    // 设置网格画笔
    QPen gridPen(m_gridColor);
    QPen majorGridPen(m_gridColorMajor);
    gridPen.setWidth(0);  // 0宽度表示1像素
    majorGridPen.setWidth(0);

    const qreal gridSize = m_gridSize;
    const qreal majorGridSize = gridSize * 5;  // 主网格线间距是普通网格的5倍

    // 计算网格线的起始和结束位置
    const qreal left = rect.left();
    const qreal right = rect.right();
    const qreal top = rect.top();
    const qreal bottom = rect.bottom();

    // 绘制垂直网格线
    const qreal startX = qFloor(left / gridSize) * gridSize;
    for (qreal x = startX; x <= right; x += gridSize) {
        bool isMajor = qAbs(qRound(x / majorGridSize) * majorGridSize - x) < 0.1;
        painter->setPen(isMajor ? majorGridPen : gridPen);
        painter->drawLine(QPointF(x, top), QPointF(x, bottom));
    }

    // 绘制水平网格线
    const qreal startY = qFloor(top / gridSize) * gridSize;
    for (qreal y = startY; y <= bottom; y += gridSize) {
        bool isMajor = qAbs(qRound(y / majorGridSize) * majorGridSize - y) < 0.1;
        painter->setPen(isMajor ? majorGridPen : gridPen);
        painter->drawLine(QPointF(left, y), QPointF(right, y));
    }

    painter->restore();
}

void InfiniteCanvas::onSceneChanged(const QList<QRectF>& region) {
    Q_UNUSED(region)
    updateViewport();
}

void InfiniteCanvas::updateViewport() {
    QRectF viewportRect = mapToScene(viewport()->rect()).boundingRect();
    emit viewportChanged(viewportRect);
}

QPointF InfiniteCanvas::snapToGrid(const QPointF& point) const {
    if (!m_snapToGrid) {
        return point;
    }

    qreal x = qRound(point.x() / m_gridSize) * m_gridSize;
    qreal y = qRound(point.y() / m_gridSize) * m_gridSize;
    return QPointF(x, y);
}

// 拖拽事件处理
void InfiniteCanvas::dragEnterEvent(QDragEnterEvent* event) {
    // 检查是否是节点类型的拖拽
    if (event->mimeData()->hasFormat("application/x-node-type") || event->mimeData()->hasText()) {
        event->acceptProposedAction();
        std::cout << "Drag enter accepted" << std::endl;
    } else {
        event->ignore();
    }
}

void InfiniteCanvas::dragMoveEvent(QDragMoveEvent* event) {
    // 检查是否是节点类型的拖拽
    if (event->mimeData()->hasFormat("application/x-node-type") || event->mimeData()->hasText()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void InfiniteCanvas::dropEvent(QDropEvent* event) {
    QString nodeType;

    // 尝试获取节点类型
    if (event->mimeData()->hasFormat("application/x-node-type")) {
        nodeType = QString::fromUtf8(event->mimeData()->data("application/x-node-type"));
    } else if (event->mimeData()->hasText()) {
        nodeType = event->mimeData()->text();
    }

    if (!nodeType.isEmpty()) {
        // 将视图坐标转换为场景坐标
        QPointF scenePos = mapToScene(event->position().toPoint());

        // 如果启用了网格对齐，则对齐到网格
        if (m_snapToGrid) {
            scenePos = snapToGrid(scenePos);
        }

        std::cout << "Node dropped: " << nodeType.toStdString() << " at (" << scenePos.x() << ", "
                  << scenePos.y() << ")" << std::endl;

        // 发出节点放置信号
        emit nodeDropped(nodeType, scenePos);

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

}  // namespace canvas
}  // namespace ui
}  // namespace oneday
