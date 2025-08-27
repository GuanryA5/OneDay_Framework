#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QTimer>
#include <QWheelEvent>

namespace oneday {
namespace ui {
namespace canvas {

/**
 * @brief 无限画布组件
 *
 * 基于QGraphicsView实现的无限画布，支持：
 * - 无限大小的绘图区域
 * - 鼠标滚轮缩放
 * - 鼠标拖拽平移
 * - 网格背景显示
 * - 平滑的缩放和平移动画
 */
class InfiniteCanvas : public QGraphicsView {
    Q_OBJECT

  public:
    explicit InfiniteCanvas(QWidget* parent = nullptr);
    ~InfiniteCanvas() = default;

    // 缩放控制
    void setZoomLevel(qreal zoomLevel);
    qreal getZoomLevel() const {
        return m_zoomLevel;
    }
    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void zoomToActualSize();

    // 网格控制
    void setGridVisible(bool visible);
    bool isGridVisible() const {
        return m_gridVisible;
    }
    void setGridSize(int size);
    int getGridSize() const {
        return m_gridSize;
    }

    // 对齐控制
    void setSnapToGrid(bool snap);
    bool isSnapToGrid() const {
        return m_snapToGrid;
    }

    // 坐标转换
    QPointF mapToCanvas(const QPoint& viewPoint) const;
    QPoint mapFromCanvas(const QPointF& canvasPoint) const;

    // 节点管理
    void createNode(const QString& nodeType, const QPointF& position);

  signals:
    void zoomLevelChanged(qreal zoomLevel);
    void viewportChanged(const QRectF& viewport);
    void canvasClicked(const QPointF& position);
    void canvasDoubleClicked(const QPointF& position);
    void nodeDropped(const QString& nodeType, const QPointF& position);

  protected:
    // 事件处理
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // 拖拽事件
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

    // 绘制函数
    void drawBackground(QPainter* painter, const QRectF& rect) override;

  private slots:
    void onSceneChanged(const QList<QRectF>& region);

  private:
    void setupCanvas();
    void setupScene();
    void updateViewport();
    void drawGrid(QPainter* painter, const QRectF& rect);
    void smoothZoom(qreal targetZoom);
    QPointF snapToGrid(const QPointF& point) const;

    // 画布属性
    QGraphicsScene* m_scene;
    qreal m_zoomLevel;
    qreal m_minZoom;
    qreal m_maxZoom;
    qreal m_zoomStep;

    // 网格属性
    bool m_gridVisible;
    int m_gridSize;
    QColor m_gridColor;
    QColor m_gridColorMajor;

    // 交互属性
    bool m_snapToGrid;
    bool m_isPanning;
    QPoint m_lastPanPoint;
    Qt::MouseButton m_panButton;

    // 动画
    QTimer* m_zoomTimer;
    qreal m_targetZoom;

    // 性能优化
    bool m_fastRender;
    QTimer* m_updateTimer;
};

}  // namespace canvas
}  // namespace ui
}  // namespace oneday
