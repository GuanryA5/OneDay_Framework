#include "blueprint_editor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QSplitter>
#include <QScrollArea>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QJsonDocument>
#include <QJsonObject>

namespace oneday::ui {

BlueprintEditor::BlueprintEditor(QWidget* parent)
    : QWidget(parent)
    , m_scene(new QGraphicsScene(this))
    , m_view(new QGraphicsView(m_scene, this))
    , m_nodePalette(new NodePalette(this))
    , m_isModified(false)
{
    setupUI();
    setupConnections();
    
    // 设置场景大小
    m_scene->setSceneRect(-5000, -5000, 10000, 10000);
    
    // 配置视图
    m_view->setDragMode(QGraphicsView::RubberBandDrag);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    // 启用拖放
    setAcceptDrops(true);
    m_view->setAcceptDrops(true);
}

BlueprintEditor::~BlueprintEditor() = default;

void BlueprintEditor::setupUI() {
    // 创建主布局
    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // 创建分割器
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    
    // 左侧：节点面板
    m_nodePalette->setMaximumWidth(250);
    m_nodePalette->setMinimumWidth(200);
    splitter->addWidget(m_nodePalette);
    
    // 右侧：编辑区域
    auto* editorWidget = new QWidget();
    auto* editorLayout = new QVBoxLayout(editorWidget);
    editorLayout->setContentsMargins(0, 0, 0, 0);
    
    // 工具栏
    setupToolBar(editorLayout);
    
    // 图形视图
    editorLayout->addWidget(m_view);
    
    splitter->addWidget(editorWidget);
    
    // 设置分割器比例
    splitter->setStretchFactor(0, 0);  // 节点面板固定宽度
    splitter->setStretchFactor(1, 1);  // 编辑区域可伸缩
    
    mainLayout->addWidget(splitter);
}

void BlueprintEditor::setupToolBar(QVBoxLayout* layout) {
    auto* toolBar = new QToolBar(this);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    // 新建动作
    auto* newAction = new QAction("新建", this);
    newAction->setIcon(QIcon(":/icons/new.png"));
    newAction->setShortcut(QKeySequence::New);
    toolBar->addAction(newAction);
    
    // 打开动作
    auto* openAction = new QAction("打开", this);
    openAction->setIcon(QIcon(":/icons/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    toolBar->addAction(openAction);
    
    // 保存动作
    auto* saveAction = new QAction("保存", this);
    saveAction->setIcon(QIcon(":/icons/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    toolBar->addAction(saveAction);
    
    toolBar->addSeparator();
    
    // 撤销动作
    auto* undoAction = new QAction("撤销", this);
    undoAction->setIcon(QIcon(":/icons/undo.png"));
    undoAction->setShortcut(QKeySequence::Undo);
    toolBar->addAction(undoAction);
    
    // 重做动作
    auto* redoAction = new QAction("重做", this);
    redoAction->setIcon(QIcon(":/icons/redo.png"));
    redoAction->setShortcut(QKeySequence::Redo);
    toolBar->addAction(redoAction);
    
    toolBar->addSeparator();
    
    // 复制动作
    auto* copyAction = new QAction("复制", this);
    copyAction->setIcon(QIcon(":/icons/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);
    toolBar->addAction(copyAction);
    
    // 粘贴动作
    auto* pasteAction = new QAction("粘贴", this);
    pasteAction->setIcon(QIcon(":/icons/paste.png"));
    pasteAction->setShortcut(QKeySequence::Paste);
    toolBar->addAction(pasteAction);
    
    // 删除动作
    auto* deleteAction = new QAction("删除", this);
    deleteAction->setIcon(QIcon(":/icons/delete.png"));
    deleteAction->setShortcut(QKeySequence::Delete);
    toolBar->addAction(deleteAction);
    
    toolBar->addSeparator();
    
    // 运行动作
    auto* runAction = new QAction("运行", this);
    runAction->setIcon(QIcon(":/icons/run.png"));
    runAction->setShortcut(QKeySequence("F5"));
    toolBar->addAction(runAction);
    
    // 停止动作
    auto* stopAction = new QAction("停止", this);
    stopAction->setIcon(QIcon(":/icons/stop.png"));
    stopAction->setShortcut(QKeySequence("Shift+F5"));
    toolBar->addAction(stopAction);
    
    layout->addWidget(toolBar);
    
    // 存储动作引用
    m_actions["new"] = newAction;
    m_actions["open"] = openAction;
    m_actions["save"] = saveAction;
    m_actions["undo"] = undoAction;
    m_actions["redo"] = redoAction;
    m_actions["copy"] = copyAction;
    m_actions["paste"] = pasteAction;
    m_actions["delete"] = deleteAction;
    m_actions["run"] = runAction;
    m_actions["stop"] = stopAction;
}

void BlueprintEditor::setupConnections() {
    // 连接动作信号
    connect(m_actions["new"], &QAction::triggered, this, &BlueprintEditor::newBlueprint);
    connect(m_actions["open"], &QAction::triggered, this, &BlueprintEditor::openBlueprint);
    connect(m_actions["save"], &QAction::triggered, this, &BlueprintEditor::saveBlueprint);
    connect(m_actions["undo"], &QAction::triggered, this, &BlueprintEditor::undo);
    connect(m_actions["redo"], &QAction::triggered, this, &BlueprintEditor::redo);
    connect(m_actions["copy"], &QAction::triggered, this, &BlueprintEditor::copySelected);
    connect(m_actions["paste"], &QAction::triggered, this, &BlueprintEditor::paste);
    connect(m_actions["delete"], &QAction::triggered, this, &BlueprintEditor::deleteSelected);
    connect(m_actions["run"], &QAction::triggered, this, &BlueprintEditor::runBlueprint);
    connect(m_actions["stop"], &QAction::triggered, this, &BlueprintEditor::stopBlueprint);
    
    // 连接场景信号
    connect(m_scene, &QGraphicsScene::selectionChanged, this, &BlueprintEditor::onSelectionChanged);
    connect(m_scene, &QGraphicsScene::changed, this, &BlueprintEditor::onSceneChanged);
    
    // 连接节点面板信号
    connect(m_nodePalette, &NodePalette::nodeRequested, this, &BlueprintEditor::addNode);
}

void BlueprintEditor::newBlueprint() {
    if (m_isModified) {
        // TODO: 询问是否保存当前蓝图
    }
    
    m_scene->clear();
    m_isModified = false;
    emit blueprintChanged();
}

void BlueprintEditor::openBlueprint() {
    // TODO: 实现打开蓝图文件
    emit blueprintChanged();
}

void BlueprintEditor::saveBlueprint() {
    // TODO: 实现保存蓝图文件
    m_isModified = false;
    emit blueprintChanged();
}

void BlueprintEditor::undo() {
    // TODO: 实现撤销功能
}

void BlueprintEditor::redo() {
    // TODO: 实现重做功能
}

void BlueprintEditor::copySelected() {
    auto selectedItems = m_scene->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }
    
    // TODO: 实现复制选中项目到剪贴板
    QJsonObject clipboardData;
    // ... 序列化选中的项目
    
    QJsonDocument doc(clipboardData);
    QApplication::clipboard()->setText(doc.toJson());
}

void BlueprintEditor::paste() {
    QString clipboardText = QApplication::clipboard()->text();
    if (clipboardText.isEmpty()) {
        return;
    }
    
    // TODO: 实现从剪贴板粘贴项目
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(clipboardText.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        return;
    }
    
    // ... 反序列化并创建项目
}

void BlueprintEditor::deleteSelected() {
    auto selectedItems = m_scene->selectedItems();
    for (auto* item : selectedItems) {
        m_scene->removeItem(item);
        delete item;
    }
    
    if (!selectedItems.isEmpty()) {
        m_isModified = true;
        emit blueprintChanged();
    }
}

void BlueprintEditor::runBlueprint() {
    // TODO: 实现运行蓝图
    emit blueprintExecutionRequested();
}

void BlueprintEditor::stopBlueprint() {
    // TODO: 实现停止蓝图执行
    emit blueprintExecutionStopped();
}

void BlueprintEditor::addNode(const QString& nodeType) {
    // TODO: 根据节点类型创建相应的图形项目
    // 这里先创建一个简单的矩形作为占位符
    auto* rect = m_scene->addRect(0, 0, 100, 60, QPen(Qt::black), QBrush(Qt::lightGray));
    rect->setFlag(QGraphicsItem::ItemIsMovable);
    rect->setFlag(QGraphicsItem::ItemIsSelectable);
    
    // 添加文本标签
    auto* text = m_scene->addText(nodeType);
    text->setParentItem(rect);
    text->setPos(10, 20);
    
    m_isModified = true;
    emit blueprintChanged();
}

void BlueprintEditor::onSelectionChanged() {
    auto selectedItems = m_scene->selectedItems();
    
    // 更新动作状态
    bool hasSelection = !selectedItems.isEmpty();
    m_actions["copy"]->setEnabled(hasSelection);
    m_actions["delete"]->setEnabled(hasSelection);
    
    emit selectionChanged(selectedItems);
}

void BlueprintEditor::onSceneChanged() {
    m_isModified = true;
    emit blueprintChanged();
}

bool BlueprintEditor::isModified() const {
    return m_isModified;
}

QGraphicsScene* BlueprintEditor::scene() const {
    return m_scene;
}

QGraphicsView* BlueprintEditor::view() const {
    return m_view;
}

} // namespace oneday::ui
