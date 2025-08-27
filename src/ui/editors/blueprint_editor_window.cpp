#include "blueprint_editor_window.h"

#include <QApplication>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include <QGroupBox>
#include <QKeySequence>
#include <QListWidget>
#include <QMessageBox>
#include <QSplitter>
#include <QStandardPaths>
#include <QTextEdit>
#include <QTreeWidget>
#include <QUrl>
#include <iostream>

namespace oneday {
namespace ui {
namespace editors {

BlueprintEditorWindow::BlueprintEditorWindow(QWidget *parent)
    : QMainWindow(parent),
      m_centralWidget(nullptr),
      m_centralLayout(nullptr),
      m_mainSplitter(nullptr),
      m_settings(nullptr),
      m_statusUpdateTimer(nullptr),
      m_isModified(false),
      m_currentZoomLevel(100),
      m_currentTool("Select") {
    std::cout << "Creating BlueprintEditorWindow..." << std::endl;

    setObjectName("BlueprintEditorWindow");
    setWindowTitle("OneDay Framework - 蓝图编辑器");
    setWindowIcon(QIcon(":/icons/blueprint_editor.png"));
    std::cout << "Basic window properties set" << std::endl;

    // 设置窗口最小尺寸
    setMinimumSize(1200, 800);
    resize(1600, 1000);
    std::cout << "Window size set" << std::endl;

    // 初始化设置
    std::cout << "Initializing QSettings..." << std::endl;
    m_settings = new QSettings("OneDay", "BlueprintEditor", this);
    std::cout << "QSettings initialized" << std::endl;

    // 设置UI
    std::cout << "Setting up shortcuts..." << std::endl;
    setupShortcuts();  // 先创建动作
    std::cout << "Shortcuts setup complete" << std::endl;

    std::cout << "Setting up UI..." << std::endl;
    setupUI();
    std::cout << "UI setup complete" << std::endl;

    std::cout << "Connecting signals..." << std::endl;
    connectSignals();
    std::cout << "Signals connected" << std::endl;

    std::cout << "Loading settings..." << std::endl;
    loadSettings();
    std::cout << "Settings loaded" << std::endl;

    // 应用默认主题
    std::cout << "Applying dark theme..." << std::endl;
    setDarkTheme();
    std::cout << "Dark theme applied" << std::endl;

    // 启动状态更新定时器
    m_statusUpdateTimer = new QTimer(this);
    connect(m_statusUpdateTimer, &QTimer::timeout, this, &BlueprintEditorWindow::updateStatusBar);
    m_statusUpdateTimer->start(100);  // 每100ms更新一次状态栏

    // 初始状态
    updateWindowTitle();
    updateStatusBar();
}

BlueprintEditorWindow::~BlueprintEditorWindow() {
    saveSettings();
}

void BlueprintEditorWindow::setupUI() {
    createMenuBar();
    createToolBars();
    createStatusBar();
    createCentralWidget();
    createDockWidgets();
}

void BlueprintEditorWindow::createMenuBar() {
    // 动作在setupShortcuts中创建

    // 文件菜单
    m_fileMenu = menuBar()->addMenu("文件(&F)");
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_importAction);
    m_fileMenu->addAction(m_exportAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    // 编辑菜单
    m_editMenu = menuBar()->addMenu("编辑(&E)");
    m_editMenu->addAction(m_undoAction);
    m_editMenu->addAction(m_redoAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_cutAction);
    m_editMenu->addAction(m_copyAction);
    m_editMenu->addAction(m_pasteAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_selectAllAction);
    m_editMenu->addAction(m_deleteAction);

    // 视图菜单
    m_viewMenu = menuBar()->addMenu("视图(&V)");
    m_viewMenu->addAction(m_zoomInAction);
    m_viewMenu->addAction(m_zoomOutAction);
    m_viewMenu->addAction(m_zoomFitAction);
    m_viewMenu->addAction(m_zoomResetAction);
    m_viewMenu->addSeparator();
    m_viewMenu->addAction(m_toggleGridAction);
    m_viewMenu->addAction(m_toggleSnapAction);

    // 工具菜单
    m_toolsMenu = menuBar()->addMenu("工具(&T)");
    m_toolsMenu->addAction(m_selectToolAction);
    m_toolsMenu->addAction(m_panToolAction);
    m_toolsMenu->addAction(m_zoomToolAction);
    m_toolsMenu->addAction(m_connectToolAction);

    // 窗口菜单
    m_windowMenu = menuBar()->addMenu("窗口(&W)");
    // 停靠面板的显示/隐藏动作将在createDockWidgets中添加

    // 帮助菜单
    m_helpMenu = menuBar()->addMenu("帮助(&H)");
    m_helpMenu->addAction(m_helpAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutAction);
}

void BlueprintEditorWindow::createToolBars() {
    // 主工具栏
    m_mainToolBar = addToolBar("主工具栏");
    m_mainToolBar->setObjectName("MainToolBar");
    m_mainToolBar->addAction(m_newAction);
    m_mainToolBar->addAction(m_openAction);
    m_mainToolBar->addAction(m_saveAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_undoAction);
    m_mainToolBar->addAction(m_redoAction);
    m_mainToolBar->addSeparator();
    m_mainToolBar->addAction(m_cutAction);
    m_mainToolBar->addAction(m_copyAction);
    m_mainToolBar->addAction(m_pasteAction);

    // 视图工具栏
    m_viewToolBar = addToolBar("视图工具栏");
    m_viewToolBar->setObjectName("ViewToolBar");
    m_viewToolBar->addAction(m_zoomInAction);
    m_viewToolBar->addAction(m_zoomOutAction);
    m_viewToolBar->addAction(m_zoomFitAction);
    m_viewToolBar->addAction(m_zoomResetAction);

    // 添加缩放下拉框
    m_zoomComboBox = new QComboBox();
    m_zoomComboBox->setEditable(true);
    m_zoomComboBox->addItems({"25%", "50%", "75%", "100%", "125%", "150%", "200%", "300%", "400%"});
    m_zoomComboBox->setCurrentText("100%");
    m_zoomComboBox->setMinimumWidth(80);
    m_viewToolBar->addWidget(m_zoomComboBox);

    // 工具栏
    m_toolsToolBar = addToolBar("工具栏");
    m_toolsToolBar->setObjectName("ToolsToolBar");

    // 创建工具动作组
    m_toolActionGroup = new QActionGroup(this);

    m_selectToolAction = new QAction(QIcon(":/icons/select.png"), "选择工具", this);
    m_selectToolAction->setCheckable(true);
    m_selectToolAction->setChecked(true);
    m_selectToolAction->setStatusTip("选择和移动节点");
    m_toolActionGroup->addAction(m_selectToolAction);
    connect(m_selectToolAction, &QAction::triggered, this, &BlueprintEditorWindow::setSelectTool);

    m_panToolAction = new QAction(QIcon(":/icons/pan.png"), "平移工具", this);
    m_panToolAction->setCheckable(true);
    m_panToolAction->setStatusTip("平移画布视图");
    m_toolActionGroup->addAction(m_panToolAction);
    connect(m_panToolAction, &QAction::triggered, this, &BlueprintEditorWindow::setPanTool);

    m_zoomToolAction = new QAction(QIcon(":/icons/zoom.png"), "缩放工具", this);
    m_zoomToolAction->setCheckable(true);
    m_zoomToolAction->setStatusTip("缩放画布视图");
    m_toolActionGroup->addAction(m_zoomToolAction);
    connect(m_zoomToolAction, &QAction::triggered, this, &BlueprintEditorWindow::setZoomTool);

    m_connectToolAction = new QAction(QIcon(":/icons/connect.png"), "连接工具", this);
    m_connectToolAction->setCheckable(true);
    m_connectToolAction->setStatusTip("连接节点端口");
    m_toolActionGroup->addAction(m_connectToolAction);
    connect(m_connectToolAction, &QAction::triggered, this, &BlueprintEditorWindow::setConnectTool);

    m_toolsToolBar->addActions(m_toolActionGroup->actions());
}

void BlueprintEditorWindow::createStatusBar() {
    // 状态标签
    m_statusLabel = new QLabel("就绪");
    statusBar()->addWidget(m_statusLabel, 1);

    // 坐标标签
    m_coordinatesLabel = new QLabel("X: 0, Y: 0");
    m_coordinatesLabel->setMinimumWidth(100);
    statusBar()->addPermanentWidget(m_coordinatesLabel);

    // 选择信息标签
    m_selectionLabel = new QLabel("未选择");
    m_selectionLabel->setMinimumWidth(120);
    statusBar()->addPermanentWidget(m_selectionLabel);

    // 缩放标签
    m_zoomLabel = new QLabel("100%");
    m_zoomLabel->setMinimumWidth(50);
    statusBar()->addPermanentWidget(m_zoomLabel);

    // 进度条（默认隐藏）
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_progressBar->setMaximumWidth(200);
    statusBar()->addPermanentWidget(m_progressBar);
}

void BlueprintEditorWindow::createCentralWidget() {
    m_centralWidget = new QWidget();
    setCentralWidget(m_centralWidget);

    m_centralLayout = new QVBoxLayout(m_centralWidget);
    m_centralLayout->setContentsMargins(0, 0, 0, 0);

    // 创建主分割器
    m_mainSplitter = new QSplitter(Qt::Horizontal);
    m_centralLayout->addWidget(m_mainSplitter);

    // 临时添加一个占位符（稍后会被画布替换）
    QLabel *placeholder = new QLabel("蓝图画布区域\n(将在下一个任务中实现)");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet(
        "QLabel { background-color: #2b2b2b; color: #ffffff; font-size: 16px; }");
    m_mainSplitter->addWidget(placeholder);
}

void BlueprintEditorWindow::createDockWidgets() {
    // 节点调色板停靠面板
    m_nodePaletteDock = new QDockWidget("节点调色板", this);
    m_nodePaletteDock->setObjectName("NodePaletteDock");
    m_nodePaletteDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // 创建节点调色板内容（临时占位符）
    m_nodePaletteWidget = new QWidget();
    QVBoxLayout *paletteLayout = new QVBoxLayout(m_nodePaletteWidget);
    QLabel *paletteLabel = new QLabel("节点调色板\n(将在任务1.3中实现)");
    paletteLabel->setAlignment(Qt::AlignCenter);
    paletteLayout->addWidget(paletteLabel);
    m_nodePaletteDock->setWidget(m_nodePaletteWidget);

    addDockWidget(Qt::LeftDockWidgetArea, m_nodePaletteDock);

    // 属性编辑面板
    m_propertyPanelDock = new QDockWidget("属性面板", this);
    m_propertyPanelDock->setObjectName("PropertyPanelDock");
    m_propertyPanelDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // 创建属性面板内容（临时占位符）
    m_propertyPanelWidget = new QWidget();
    QVBoxLayout *propertyLayout = new QVBoxLayout(m_propertyPanelWidget);
    QLabel *propertyLabel = new QLabel("属性编辑面板\n(将在任务1.4中实现)");
    propertyLabel->setAlignment(Qt::AlignCenter);
    propertyLayout->addWidget(propertyLabel);
    m_propertyPanelDock->setWidget(m_propertyPanelWidget);

    addDockWidget(Qt::RightDockWidgetArea, m_propertyPanelDock);

    // 大纲面板
    m_outlinePanelDock = new QDockWidget("大纲", this);
    m_outlinePanelDock->setObjectName("OutlinePanelDock");
    m_outlinePanelDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // 创建大纲面板内容
    m_outlinePanelWidget = new QTreeWidget();
    static_cast<QTreeWidget *>(m_outlinePanelWidget)->setHeaderLabel("蓝图结构");
    m_outlinePanelDock->setWidget(m_outlinePanelWidget);

    addDockWidget(Qt::RightDockWidgetArea, m_outlinePanelDock);

    // 控制台面板
    m_consolePanelDock = new QDockWidget("控制台", this);
    m_consolePanelDock->setObjectName("ConsolePanelDock");
    m_consolePanelDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);

    // 创建控制台内容
    m_consolePanelWidget = new QTextEdit();
    static_cast<QTextEdit *>(m_consolePanelWidget)->setReadOnly(true);
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("OneDay Framework 蓝图编辑器控制台");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("版本: 1.0.0");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("就绪...");
    m_consolePanelDock->setWidget(m_consolePanelWidget);

    addDockWidget(Qt::BottomDockWidgetArea, m_consolePanelDock);

    // 默认隐藏控制台
    m_consolePanelDock->hide();

    // 添加停靠面板切换动作到窗口菜单
    m_windowMenu->addAction(m_nodePaletteDock->toggleViewAction());
    m_windowMenu->addAction(m_propertyPanelDock->toggleViewAction());
    m_windowMenu->addAction(m_outlinePanelDock->toggleViewAction());
    m_windowMenu->addAction(m_consolePanelDock->toggleViewAction());
    m_windowMenu->addSeparator();

    QAction *resetLayoutAction = new QAction("重置布局", this);
    resetLayoutAction->setStatusTip("重置窗口布局到默认状态");
    connect(resetLayoutAction, &QAction::triggered, this, &BlueprintEditorWindow::resetLayout);
    m_windowMenu->addAction(resetLayoutAction);

    // 设置停靠面板的默认大小
    resizeDocks({m_nodePaletteDock}, {250}, Qt::Horizontal);
    resizeDocks({m_propertyPanelDock, m_outlinePanelDock}, {250, 200}, Qt::Horizontal);
    resizeDocks({m_consolePanelDock}, {150}, Qt::Vertical);
}

void BlueprintEditorWindow::connectSignals() {
    // 缩放下拉框信号连接
    connect(m_zoomComboBox,
            QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this,
            [this](const QString &text) {
                QString cleanText = text;
                cleanText.remove('%');
                bool ok;
                int zoomLevel = cleanText.toInt(&ok);
                if (ok && zoomLevel > 0) {
                    onZoomChanged(zoomLevel);
                }
            });
}

void BlueprintEditorWindow::setupShortcuts() {
    // 创建所有动作
    // 文件动作
    m_newAction = new QAction(QIcon(":/icons/new.png"), "新建(&N)", this);
    m_newAction->setShortcut(QKeySequence::New);
    m_newAction->setStatusTip("创建新的蓝图");
    connect(m_newAction, &QAction::triggered, this, &BlueprintEditorWindow::newBlueprint);

    m_openAction = new QAction(QIcon(":/icons/open.png"), "打开(&O)", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip("打开现有蓝图");
    connect(m_openAction, &QAction::triggered, this, &BlueprintEditorWindow::openBlueprint);

    m_saveAction = new QAction(QIcon(":/icons/save.png"), "保存(&S)", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip("保存当前蓝图");
    connect(m_saveAction, &QAction::triggered, this, &BlueprintEditorWindow::saveBlueprint);

    m_saveAsAction = new QAction("另存为(&A)...", this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setStatusTip("将蓝图保存到新文件");
    connect(m_saveAsAction, &QAction::triggered, this, &BlueprintEditorWindow::saveBlueprintAs);

    m_importAction = new QAction(QIcon(":/icons/import.png"), "导入(&I)...", this);
    m_importAction->setStatusTip("导入蓝图文件");
    connect(m_importAction, &QAction::triggered, this, &BlueprintEditorWindow::importBlueprint);

    m_exportAction = new QAction(QIcon(":/icons/export.png"), "导出(&E)...", this);
    m_exportAction->setStatusTip("导出蓝图文件");
    connect(m_exportAction, &QAction::triggered, this, &BlueprintEditorWindow::exportBlueprint);

    m_exitAction = new QAction("退出(&X)", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("退出应用程序");
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);

    // 编辑动作
    m_undoAction = new QAction(QIcon(":/icons/undo.png"), "撤销(&U)", this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    m_undoAction->setStatusTip("撤销上一个操作");
    m_undoAction->setEnabled(false);
    connect(m_undoAction, &QAction::triggered, this, &BlueprintEditorWindow::undo);

    m_redoAction = new QAction(QIcon(":/icons/redo.png"), "重做(&R)", this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    m_redoAction->setStatusTip("重做上一个操作");
    m_redoAction->setEnabled(false);
    connect(m_redoAction, &QAction::triggered, this, &BlueprintEditorWindow::redo);

    m_cutAction = new QAction(QIcon(":/icons/cut.png"), "剪切(&T)", this);
    m_cutAction->setShortcut(QKeySequence::Cut);
    m_cutAction->setStatusTip("剪切选中的项目");
    connect(m_cutAction, &QAction::triggered, this, &BlueprintEditorWindow::cut);

    m_copyAction = new QAction(QIcon(":/icons/copy.png"), "复制(&C)", this);
    m_copyAction->setShortcut(QKeySequence::Copy);
    m_copyAction->setStatusTip("复制选中的项目");
    connect(m_copyAction, &QAction::triggered, this, &BlueprintEditorWindow::copy);

    m_pasteAction = new QAction(QIcon(":/icons/paste.png"), "粘贴(&P)", this);
    m_pasteAction->setShortcut(QKeySequence::Paste);
    m_pasteAction->setStatusTip("粘贴剪贴板内容");
    connect(m_pasteAction, &QAction::triggered, this, &BlueprintEditorWindow::paste);

    m_selectAllAction = new QAction("全选(&A)", this);
    m_selectAllAction->setShortcut(QKeySequence::SelectAll);
    m_selectAllAction->setStatusTip("选择所有项目");
    connect(m_selectAllAction, &QAction::triggered, this, &BlueprintEditorWindow::selectAll);

    m_deleteAction = new QAction(QIcon(":/icons/delete.png"), "删除(&D)", this);
    m_deleteAction->setShortcut(QKeySequence::Delete);
    m_deleteAction->setStatusTip("删除选中的项目");
    connect(m_deleteAction, &QAction::triggered, this, &BlueprintEditorWindow::deleteSelected);

    // 视图动作快捷键
    m_zoomInAction = new QAction(QIcon(":/icons/zoom_in.png"), "放大(&I)", this);
    m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
    m_zoomInAction->setStatusTip("放大视图");
    connect(m_zoomInAction, &QAction::triggered, this, &BlueprintEditorWindow::zoomIn);

    m_zoomOutAction = new QAction(QIcon(":/icons/zoom_out.png"), "缩小(&O)", this);
    m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    m_zoomOutAction->setStatusTip("缩小视图");
    connect(m_zoomOutAction, &QAction::triggered, this, &BlueprintEditorWindow::zoomOut);

    m_zoomFitAction = new QAction(QIcon(":/icons/zoom_fit.png"), "适合窗口(&F)", this);
    m_zoomFitAction->setShortcut(QKeySequence("Ctrl+0"));
    m_zoomFitAction->setStatusTip("缩放以适合窗口");
    connect(m_zoomFitAction, &QAction::triggered, this, &BlueprintEditorWindow::zoomFit);

    m_zoomResetAction = new QAction(QIcon(":/icons/zoom_reset.png"), "实际大小(&1)", this);
    m_zoomResetAction->setShortcut(QKeySequence("Ctrl+1"));
    m_zoomResetAction->setStatusTip("重置为100%缩放");
    connect(m_zoomResetAction, &QAction::triggered, this, &BlueprintEditorWindow::zoomReset);

    m_toggleGridAction = new QAction("显示网格(&G)", this);
    m_toggleGridAction->setCheckable(true);
    m_toggleGridAction->setChecked(true);
    m_toggleGridAction->setShortcut(QKeySequence("Ctrl+G"));
    m_toggleGridAction->setStatusTip("显示/隐藏网格");
    connect(m_toggleGridAction, &QAction::triggered, this, &BlueprintEditorWindow::toggleGrid);

    m_toggleSnapAction = new QAction("对齐网格(&S)", this);
    m_toggleSnapAction->setCheckable(true);
    m_toggleSnapAction->setChecked(true);
    m_toggleSnapAction->setShortcut(QKeySequence("Ctrl+Shift+G"));
    m_toggleSnapAction->setStatusTip("启用/禁用网格对齐");
    connect(m_toggleSnapAction, &QAction::triggered, this, &BlueprintEditorWindow::toggleSnap);

    // 帮助动作
    m_helpAction = new QAction("帮助文档(&H)", this);
    m_helpAction->setShortcut(QKeySequence::HelpContents);
    m_helpAction->setStatusTip("打开帮助文档");
    connect(m_helpAction, &QAction::triggered, this, &BlueprintEditorWindow::showHelp);

    m_aboutAction = new QAction("关于(&A)", this);
    m_aboutAction->setStatusTip("关于OneDay Framework");
    connect(m_aboutAction, &QAction::triggered, this, &BlueprintEditorWindow::showAbout);
}

// 槽函数实现
void BlueprintEditorWindow::newBlueprint() {
    // TODO: 实现新建蓝图功能
    m_statusLabel->setText("新建蓝图");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("创建新蓝图...");
}

void BlueprintEditorWindow::openBlueprint() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "打开蓝图", "", "蓝图文件 (*.blueprint);;所有文件 (*.*)");
    if (!fileName.isEmpty()) {
        m_currentFilePath = fileName;
        m_statusLabel->setText("打开蓝图: " + QFileInfo(fileName).baseName());
        static_cast<QTextEdit *>(m_consolePanelWidget)->append("打开蓝图: " + fileName);
        updateWindowTitle();
    }
}

void BlueprintEditorWindow::saveBlueprint() {
    if (m_currentFilePath.isEmpty()) {
        saveBlueprintAs();
    } else {
        // TODO: 实现保存功能
        m_statusLabel->setText("保存蓝图: " + QFileInfo(m_currentFilePath).baseName());
        static_cast<QTextEdit *>(m_consolePanelWidget)->append("保存蓝图: " + m_currentFilePath);
        m_isModified = false;
        updateWindowTitle();
    }
}

void BlueprintEditorWindow::saveBlueprintAs() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "保存蓝图", "", "蓝图文件 (*.blueprint);;所有文件 (*.*)");
    if (!fileName.isEmpty()) {
        m_currentFilePath = fileName;
        saveBlueprint();
    }
}

void BlueprintEditorWindow::exportBlueprint() {
    QString fileName = QFileDialog::getSaveFileName(
        this, "导出蓝图", "", "JSON文件 (*.json);;XML文件 (*.xml);;所有文件 (*.*)");
    if (!fileName.isEmpty()) {
        m_statusLabel->setText("导出蓝图: " + QFileInfo(fileName).baseName());
        static_cast<QTextEdit *>(m_consolePanelWidget)->append("导出蓝图: " + fileName);
    }
}

void BlueprintEditorWindow::importBlueprint() {
    QString fileName = QFileDialog::getOpenFileName(
        this, "导入蓝图", "", "JSON文件 (*.json);;XML文件 (*.xml);;所有文件 (*.*)");
    if (!fileName.isEmpty()) {
        m_statusLabel->setText("导入蓝图: " + QFileInfo(fileName).baseName());
        static_cast<QTextEdit *>(m_consolePanelWidget)->append("导入蓝图: " + fileName);
    }
}

void BlueprintEditorWindow::undo() {
    // TODO: 实现撤销功能
    m_statusLabel->setText("撤销操作");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("撤销上一个操作");
}

void BlueprintEditorWindow::redo() {
    // TODO: 实现重做功能
    m_statusLabel->setText("重做操作");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("重做操作");
}

void BlueprintEditorWindow::cut() {
    // TODO: 实现剪切功能
    m_statusLabel->setText("剪切选中项");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("剪切选中的节点");
}

void BlueprintEditorWindow::copy() {
    // TODO: 实现复制功能
    m_statusLabel->setText("复制选中项");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("复制选中的节点");
}

void BlueprintEditorWindow::paste() {
    // TODO: 实现粘贴功能
    m_statusLabel->setText("粘贴项目");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("粘贴节点");
}

void BlueprintEditorWindow::selectAll() {
    // TODO: 实现全选功能
    m_statusLabel->setText("选择所有项目");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("选择所有节点");
}

void BlueprintEditorWindow::deleteSelected() {
    // TODO: 实现删除功能
    m_statusLabel->setText("删除选中项");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append("删除选中的节点");
}

// 视图操作槽函数
void BlueprintEditorWindow::zoomIn() {
    int newZoom = qMin(m_currentZoomLevel + 25, 400);
    onZoomChanged(newZoom);
}

void BlueprintEditorWindow::zoomOut() {
    int newZoom = qMax(m_currentZoomLevel - 25, 25);
    onZoomChanged(newZoom);
}

void BlueprintEditorWindow::zoomFit() {
    // TODO: 实现适合窗口缩放
    onZoomChanged(100);
    m_statusLabel->setText("缩放适合窗口");
}

void BlueprintEditorWindow::zoomReset() {
    onZoomChanged(100);
    m_statusLabel->setText("重置缩放到100%");
}

void BlueprintEditorWindow::toggleGrid() {
    bool showGrid = m_toggleGridAction->isChecked();
    m_statusLabel->setText(showGrid ? "显示网格" : "隐藏网格");
    static_cast<QTextEdit *>(m_consolePanelWidget)->append(showGrid ? "网格已显示" : "网格已隐藏");
}

void BlueprintEditorWindow::toggleSnap() {
    bool snapToGrid = m_toggleSnapAction->isChecked();
    m_statusLabel->setText(snapToGrid ? "启用网格对齐" : "禁用网格对齐");
    static_cast<QTextEdit *>(m_consolePanelWidget)
        ->append(snapToGrid ? "网格对齐已启用" : "网格对齐已禁用");
}

// 工具操作槽函数
void BlueprintEditorWindow::setSelectTool() {
    m_currentTool = "Select";
    m_statusLabel->setText("选择工具");
}

void BlueprintEditorWindow::setPanTool() {
    m_currentTool = "Pan";
    m_statusLabel->setText("平移工具");
}

void BlueprintEditorWindow::setZoomTool() {
    m_currentTool = "Zoom";
    m_statusLabel->setText("缩放工具");
}

void BlueprintEditorWindow::setConnectTool() {
    m_currentTool = "Connect";
    m_statusLabel->setText("连接工具");
}

// 窗口操作槽函数
void BlueprintEditorWindow::toggleNodePalette() {
    m_nodePaletteDock->setVisible(!m_nodePaletteDock->isVisible());
}

void BlueprintEditorWindow::togglePropertyPanel() {
    m_propertyPanelDock->setVisible(!m_propertyPanelDock->isVisible());
}

void BlueprintEditorWindow::toggleOutlinePanel() {
    m_outlinePanelDock->setVisible(!m_outlinePanelDock->isVisible());
}

void BlueprintEditorWindow::toggleConsolePanel() {
    m_consolePanelDock->setVisible(!m_consolePanelDock->isVisible());
}

void BlueprintEditorWindow::resetLayout() {
    // 重置所有停靠面板到默认位置
    addDockWidget(Qt::LeftDockWidgetArea, m_nodePaletteDock);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyPanelDock);
    addDockWidget(Qt::RightDockWidgetArea, m_outlinePanelDock);
    addDockWidget(Qt::BottomDockWidgetArea, m_consolePanelDock);

    // 显示主要面板，隐藏控制台
    m_nodePaletteDock->show();
    m_propertyPanelDock->show();
    m_outlinePanelDock->show();
    m_consolePanelDock->hide();

    // 重置大小
    resizeDocks({m_nodePaletteDock}, {250}, Qt::Horizontal);
    resizeDocks({m_propertyPanelDock, m_outlinePanelDock}, {250, 200}, Qt::Horizontal);
    resizeDocks({m_consolePanelDock}, {150}, Qt::Vertical);

    m_statusLabel->setText("布局已重置");
}

// 帮助操作槽函数
void BlueprintEditorWindow::showAbout() {
    QMessageBox::about(this,
                       "关于 OneDay Framework",
                       "<h2>OneDay Framework 蓝图编辑器</h2>"
                       "<p>版本: 1.0.0</p>"
                       "<p>一个强大的可视化编程工具，支持无限画布和节点编辑。</p>"
                       "<p>Copyright © 2024 OneDay Framework Team</p>");
}

void BlueprintEditorWindow::showHelp() {
    QDesktopServices::openUrl(QUrl("https://oneday-framework.com/help"));
}

// 事件处理函数
void BlueprintEditorWindow::closeEvent(QCloseEvent *event) {
    if (m_isModified) {
        QMessageBox::StandardButton ret =
            QMessageBox::warning(this,
                                 "OneDay Framework",
                                 "蓝图已被修改。\n是否要保存更改？",
                                 QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (ret == QMessageBox::Save) {
            saveBlueprint();
            event->accept();
        } else if (ret == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }

    saveSettings();
    event->accept();
}

void BlueprintEditorWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    updateStatusBar();
}

void BlueprintEditorWindow::changeEvent(QEvent *event) {
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        updateStatusBar();
    }
}

// 私有槽函数
void BlueprintEditorWindow::updateStatusBar() {
    // 更新坐标信息（暂时使用固定值）
    m_coordinatesLabel->setText("X: 0, Y: 0");

    // 更新缩放信息
    m_zoomLabel->setText(QString("%1%").arg(m_currentZoomLevel));

    // 更新选择信息（暂时使用固定值）
    m_selectionLabel->setText("未选择");
}

void BlueprintEditorWindow::updateWindowTitle() {
    QString title = "OneDay Framework - 蓝图编辑器";
    if (!m_currentFilePath.isEmpty()) {
        title += " - " + QFileInfo(m_currentFilePath).baseName();
    }
    if (m_isModified) {
        title += " *";
    }
    setWindowTitle(title);
}

void BlueprintEditorWindow::onZoomChanged(int zoomLevel) {
    m_currentZoomLevel = zoomLevel;
    m_zoomComboBox->setCurrentText(QString("%1%").arg(zoomLevel));
    m_zoomLabel->setText(QString("%1%").arg(zoomLevel));

    // TODO: 实际应用缩放到画布
    static_cast<QTextEdit *>(m_consolePanelWidget)->append(QString("缩放级别: %1%").arg(zoomLevel));
}

void BlueprintEditorWindow::onSelectionChanged() {
    // TODO: 根据实际选择更新状态
    updateStatusBar();
}

// 设置和主题函数
void BlueprintEditorWindow::loadSettings() {
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("windowState").toByteArray());

    // 加载其他设置
    m_currentZoomLevel = m_settings->value("zoomLevel", 100).toInt();
    bool showGrid = m_settings->value("showGrid", true).toBool();
    bool snapToGrid = m_settings->value("snapToGrid", true).toBool();

    m_toggleGridAction->setChecked(showGrid);
    m_toggleSnapAction->setChecked(snapToGrid);
    m_zoomComboBox->setCurrentText(QString("%1%").arg(m_currentZoomLevel));
}

void BlueprintEditorWindow::saveSettings() {
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
    m_settings->setValue("zoomLevel", m_currentZoomLevel);
    m_settings->setValue("showGrid", m_toggleGridAction->isChecked());
    m_settings->setValue("snapToGrid", m_toggleSnapAction->isChecked());
}

void BlueprintEditorWindow::setDarkTheme() {
    applyCustomStyleSheet();
}

void BlueprintEditorWindow::setLightTheme() {
    setStyleSheet("");
}

void BlueprintEditorWindow::applyCustomStyleSheet() {
    QString darkStyle = R"(
        QMainWindow {
            background-color: #2b2b2b;
            color: #ffffff;
        }
        QMenuBar {
            background-color: #3c3c3c;
            color: #ffffff;
            border-bottom: 1px solid #555555;
        }
        QMenuBar::item {
            background-color: transparent;
            padding: 4px 8px;
        }
        QMenuBar::item:selected {
            background-color: #555555;
        }
        QMenu {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
        }
        QMenu::item:selected {
            background-color: #555555;
        }
        QToolBar {
            background-color: #3c3c3c;
            border: 1px solid #555555;
            spacing: 2px;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            padding: 4px;
        }
        QToolButton:hover {
            background-color: #555555;
        }
        QToolButton:checked {
            background-color: #0078d4;
        }
        QStatusBar {
            background-color: #3c3c3c;
            color: #ffffff;
            border-top: 1px solid #555555;
        }
        QDockWidget {
            background-color: #2b2b2b;
            color: #ffffff;
            titlebar-close-icon: url(:/icons/close.png);
            titlebar-normal-icon: url(:/icons/undock.png);
        }
        QDockWidget::title {
            background-color: #3c3c3c;
            padding: 4px;
            border-bottom: 1px solid #555555;
        }
        QComboBox {
            background-color: #3c3c3c;
            color: #ffffff;
            border: 1px solid #555555;
            padding: 2px 4px;
        }
        QComboBox::drop-down {
            border: none;
        }
        QComboBox::down-arrow {
            image: url(:/icons/arrow_down.png);
        }
        QTextEdit {
            background-color: #2b2b2b;
            color: #ffffff;
            border: 1px solid #555555;
        }
        QTreeWidget {
            background-color: #2b2b2b;
            color: #ffffff;
            border: 1px solid #555555;
        }
        QLabel {
            color: #ffffff;
        }
    )";

    setStyleSheet(darkStyle);
}

}  // namespace editors
}  // namespace ui
}  // namespace oneday
