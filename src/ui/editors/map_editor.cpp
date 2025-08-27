#include "map_editor.h"

#include <QAction>
#include <QBrush>
#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPen>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QToolBar>
#include <QVBoxLayout>

namespace oneday::ui {

MapEditor::MapEditor(QWidget* parent)
    : QWidget(parent),
      m_scene(new QGraphicsScene(this)),
      m_view(new QGraphicsView(m_scene, this)),
      m_currentTool(Tool::Select),
      m_gridSize(20),
      m_showGrid(true),
      m_snapToGrid(true),
      m_isModified(false) {
    setupUI();
    setupConnections();

    // 设置场景
    m_scene->setSceneRect(-2000, -2000, 4000, 4000);

    // 配置视图
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // 绘制网格
    updateGrid();
}

MapEditor::~MapEditor() = default;

void MapEditor::setupUI() {
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 左侧工具面板
    setupToolPanel(mainLayout);

    // 右侧编辑区域
    auto* editorWidget = new QWidget();
    auto* editorLayout = new QVBoxLayout(editorWidget);
    editorLayout->setContentsMargins(0, 0, 0, 0);

    // 工具栏
    setupToolBar(editorLayout);

    // 图形视图
    editorLayout->addWidget(m_view);

    mainLayout->addWidget(editorWidget, 1);
}

void MapEditor::setupToolPanel(QHBoxLayout* mainLayout) {
    auto* toolPanel = new QWidget();
    toolPanel->setMaximumWidth(200);
    toolPanel->setMinimumWidth(180);

    auto* panelLayout = new QVBoxLayout(toolPanel);

    // 工具选择组
    auto* toolGroup = new QGroupBox("工具", toolPanel);
    auto* toolLayout = new QVBoxLayout(toolGroup);

    m_toolButtons["select"] = new QPushButton("选择", toolGroup);
    m_toolButtons["rect"] = new QPushButton("矩形", toolGroup);
    m_toolButtons["circle"] = new QPushButton("圆形", toolGroup);
    m_toolButtons["line"] = new QPushButton("直线", toolGroup);
    m_toolButtons["path"] = new QPushButton("路径", toolGroup);
    m_toolButtons["obstacle"] = new QPushButton("障碍物", toolGroup);

    for (auto* button : m_toolButtons.values()) {
        button->setCheckable(true);
        toolLayout->addWidget(button);
    }

    m_toolButtons["select"]->setChecked(true);
    panelLayout->addWidget(toolGroup);

    // 网格设置组
    auto* gridGroup = new QGroupBox("网格", toolPanel);
    auto* gridLayout = new QGridLayout(gridGroup);

    m_showGridCheck = new QCheckBox("显示网格", gridGroup);
    m_showGridCheck->setChecked(m_showGrid);
    gridLayout->addWidget(m_showGridCheck, 0, 0, 1, 2);

    m_snapToGridCheck = new QCheckBox("对齐网格", gridGroup);
    m_snapToGridCheck->setChecked(m_snapToGrid);
    gridLayout->addWidget(m_snapToGridCheck, 1, 0, 1, 2);

    gridLayout->addWidget(new QLabel("网格大小:"), 2, 0);
    m_gridSizeSpinBox = new QSpinBox(gridGroup);
    m_gridSizeSpinBox->setRange(5, 100);
    m_gridSizeSpinBox->setValue(m_gridSize);
    gridLayout->addWidget(m_gridSizeSpinBox, 2, 1);

    panelLayout->addWidget(gridGroup);

    // 属性设置组
    auto* propGroup = new QGroupBox("属性", toolPanel);
    auto* propLayout = new QGridLayout(propGroup);

    propLayout->addWidget(new QLabel("线条颜色:"), 0, 0);
    m_lineColorButton = new QPushButton(propGroup);
    m_lineColorButton->setStyleSheet("background-color: black; border: 1px solid gray;");
    m_lineColorButton->setMaximumHeight(25);
    propLayout->addWidget(m_lineColorButton, 0, 1);

    propLayout->addWidget(new QLabel("填充颜色:"), 1, 0);
    m_fillColorButton = new QPushButton(propGroup);
    m_fillColorButton->setStyleSheet("background-color: lightgray; border: 1px solid gray;");
    m_fillColorButton->setMaximumHeight(25);
    propLayout->addWidget(m_fillColorButton, 1, 1);

    propLayout->addWidget(new QLabel("线条宽度:"), 2, 0);
    m_lineWidthSpinBox = new QSpinBox(propGroup);
    m_lineWidthSpinBox->setRange(1, 20);
    m_lineWidthSpinBox->setValue(2);
    propLayout->addWidget(m_lineWidthSpinBox, 2, 1);

    panelLayout->addWidget(propGroup);

    // 图层组
    auto* layerGroup = new QGroupBox("图层", toolPanel);
    auto* layerLayout = new QVBoxLayout(layerGroup);

    m_layerComboBox = new QComboBox(layerGroup);
    m_layerComboBox->addItems({"背景", "障碍物", "路径", "标记"});
    layerLayout->addWidget(m_layerComboBox);

    panelLayout->addWidget(layerGroup);

    panelLayout->addStretch();

    mainLayout->addWidget(toolPanel);
}

void MapEditor::setupToolBar(QVBoxLayout* layout) {
    auto* toolBar = new QToolBar(this);

    // 文件操作
    auto* newAction = new QAction("新建", this);
    newAction->setIcon(QIcon(":/icons/new.png"));
    toolBar->addAction(newAction);

    auto* openAction = new QAction("打开", this);
    openAction->setIcon(QIcon(":/icons/open.png"));
    toolBar->addAction(openAction);

    auto* saveAction = new QAction("保存", this);
    saveAction->setIcon(QIcon(":/icons/save.png"));
    toolBar->addAction(saveAction);

    toolBar->addSeparator();

    // 编辑操作
    auto* undoAction = new QAction("撤销", this);
    undoAction->setIcon(QIcon(":/icons/undo.png"));
    toolBar->addAction(undoAction);

    auto* redoAction = new QAction("重做", this);
    redoAction->setIcon(QIcon(":/icons/redo.png"));
    toolBar->addAction(redoAction);

    toolBar->addSeparator();

    // 视图操作
    auto* zoomInAction = new QAction("放大", this);
    zoomInAction->setIcon(QIcon(":/icons/zoom_in.png"));
    toolBar->addAction(zoomInAction);

    auto* zoomOutAction = new QAction("缩小", this);
    zoomOutAction->setIcon(QIcon(":/icons/zoom_out.png"));
    toolBar->addAction(zoomOutAction);

    auto* fitAction = new QAction("适应窗口", this);
    fitAction->setIcon(QIcon(":/icons/fit.png"));
    toolBar->addAction(fitAction);

    layout->addWidget(toolBar);

    // 存储动作引用
    m_actions["new"] = newAction;
    m_actions["open"] = openAction;
    m_actions["save"] = saveAction;
    m_actions["undo"] = undoAction;
    m_actions["redo"] = redoAction;
    m_actions["zoomIn"] = zoomInAction;
    m_actions["zoomOut"] = zoomOutAction;
    m_actions["fit"] = fitAction;
}

void MapEditor::setupConnections() {
    // 工具按钮连接
    connect(
        m_toolButtons["select"], &QPushButton::clicked, [this]() { setCurrentTool(Tool::Select); });
    connect(m_toolButtons["rect"], &QPushButton::clicked, [this]() {
        setCurrentTool(Tool::Rectangle);
    });
    connect(
        m_toolButtons["circle"], &QPushButton::clicked, [this]() { setCurrentTool(Tool::Circle); });
    connect(m_toolButtons["line"], &QPushButton::clicked, [this]() { setCurrentTool(Tool::Line); });
    connect(m_toolButtons["path"], &QPushButton::clicked, [this]() { setCurrentTool(Tool::Path); });
    connect(m_toolButtons["obstacle"], &QPushButton::clicked, [this]() {
        setCurrentTool(Tool::Obstacle);
    });

    // 网格设置连接
    connect(m_showGridCheck, &QCheckBox::toggled, this, &MapEditor::setShowGrid);
    connect(m_snapToGridCheck, &QCheckBox::toggled, this, &MapEditor::setSnapToGrid);
    connect(m_gridSizeSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            &MapEditor::setGridSize);

    // 颜色按钮连接
    connect(m_lineColorButton, &QPushButton::clicked, this, &MapEditor::chooseLineColor);
    connect(m_fillColorButton, &QPushButton::clicked, this, &MapEditor::chooseFillColor);

    // 动作连接
    connect(m_actions["new"], &QAction::triggered, this, &MapEditor::newMap);
    connect(m_actions["open"], &QAction::triggered, this, &MapEditor::openMap);
    connect(m_actions["save"], &QAction::triggered, this, &MapEditor::saveMap);
    connect(m_actions["undo"], &QAction::triggered, this, &MapEditor::undo);
    connect(m_actions["redo"], &QAction::triggered, this, &MapEditor::redo);
    connect(m_actions["zoomIn"], &QAction::triggered, this, &MapEditor::zoomIn);
    connect(m_actions["zoomOut"], &QAction::triggered, this, &MapEditor::zoomOut);
    connect(m_actions["fit"], &QAction::triggered, this, &MapEditor::fitInView);

    // 场景连接
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &MapEditor::onSelectionChanged);
    connect(m_scene, &QGraphicsScene::changed, this, &MapEditor::onSceneChanged);
}

void MapEditor::setCurrentTool(Tool tool) {
    m_currentTool = tool;

    // 更新按钮状态
    for (auto* button : m_toolButtons.values()) {
        button->setChecked(false);
    }

    switch (tool) {
        case Tool::Select:
            m_toolButtons["select"]->setChecked(true);
            m_view->setDragMode(QGraphicsView::RubberBandDrag);
            break;
        case Tool::Rectangle:
            m_toolButtons["rect"]->setChecked(true);
            m_view->setDragMode(QGraphicsView::NoDrag);
            break;
        case Tool::Circle:
            m_toolButtons["circle"]->setChecked(true);
            m_view->setDragMode(QGraphicsView::NoDrag);
            break;
        case Tool::Line:
            m_toolButtons["line"]->setChecked(true);
            m_view->setDragMode(QGraphicsView::NoDrag);
            break;
        case Tool::Path:
            m_toolButtons["path"]->setChecked(true);
            m_view->setDragMode(QGraphicsView::NoDrag);
            break;
        case Tool::Obstacle:
            m_toolButtons["obstacle"]->setChecked(true);
            m_view->setDragMode(QGraphicsView::NoDrag);
            break;
    }

    emit toolChanged(tool);
}

void MapEditor::setShowGrid(bool show) {
    m_showGrid = show;
    updateGrid();
}

void MapEditor::setSnapToGrid(bool snap) {
    m_snapToGrid = snap;
}

void MapEditor::setGridSize(int size) {
    m_gridSize = size;
    updateGrid();
}

void MapEditor::updateGrid() {
    // 清除现有网格
    for (auto* item : m_gridItems) {
        m_scene->removeItem(item);
        delete item;
    }
    m_gridItems.clear();

    if (!m_showGrid) {
        return;
    }

    // 绘制新网格
    QRectF sceneRect = m_scene->sceneRect();
    QPen gridPen(QColor(200, 200, 200), 1, Qt::DotLine);

    // 垂直线
    for (int x = static_cast<int>(sceneRect.left()); x <= sceneRect.right(); x += m_gridSize) {
        auto* line = m_scene->addLine(x, sceneRect.top(), x, sceneRect.bottom(), gridPen);
        line->setZValue(-1000);  // 确保网格在最底层
        m_gridItems.append(line);
    }

    // 水平线
    for (int y = static_cast<int>(sceneRect.top()); y <= sceneRect.bottom(); y += m_gridSize) {
        auto* line = m_scene->addLine(sceneRect.left(), y, sceneRect.right(), y, gridPen);
        line->setZValue(-1000);
        m_gridItems.append(line);
    }
}

QPointF MapEditor::snapToGrid(const QPointF& point) {
    if (!m_snapToGrid) {
        return point;
    }

    int x = static_cast<int>(point.x() / m_gridSize) * m_gridSize;
    int y = static_cast<int>(point.y() / m_gridSize) * m_gridSize;

    return QPointF(x, y);
}

void MapEditor::chooseLineColor() {
    QColor color = QColorDialog::getColor(Qt::black, this, "选择线条颜色");
    if (color.isValid()) {
        m_lineColor = color;
        m_lineColorButton->setStyleSheet(
            QString("background-color: %1; border: 1px solid gray;").arg(color.name()));
    }
}

void MapEditor::chooseFillColor() {
    QColor color = QColorDialog::getColor(Qt::lightGray, this, "选择填充颜色");
    if (color.isValid()) {
        m_fillColor = color;
        m_fillColorButton->setStyleSheet(
            QString("background-color: %1; border: 1px solid gray;").arg(color.name()));
    }
}

void MapEditor::newMap() {
    m_scene->clear();
    updateGrid();
    m_isModified = false;
    emit mapChanged();
}

void MapEditor::openMap() {
    // TODO: 实现打开地图文件
    emit mapChanged();
}

void MapEditor::saveMap() {
    // TODO: 实现保存地图文件
    m_isModified = false;
    emit mapChanged();
}

void MapEditor::undo() {
    // TODO: 实现撤销功能
}

void MapEditor::redo() {
    // TODO: 实现重做功能
}

void MapEditor::zoomIn() {
    m_view->scale(1.2, 1.2);
}

void MapEditor::zoomOut() {
    m_view->scale(0.8, 0.8);
}

void MapEditor::fitInView() {
    m_view->fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void MapEditor::onSelectionChanged() {
    auto selectedItems = m_scene->selectedItems();
    emit selectionChanged(selectedItems);
}

void MapEditor::onSceneChanged() {
    m_isModified = true;
    emit mapChanged();
}

bool MapEditor::isModified() const {
    return m_isModified;
}

QGraphicsScene* MapEditor::scene() const {
    return m_scene;
}

QGraphicsView* MapEditor::view() const {
    return m_view;
}

MapEditor::Tool MapEditor::currentTool() const {
    return m_currentTool;
}

}  // namespace oneday::ui
