#include "main_window.h"

#include <QApplication>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

#include "core/common/logger.h"

namespace oneday {
namespace ui {

class MainWindow::Impl {
  public:
    QWidget* centralWidget = nullptr;
    QMenuBar* menuBar = nullptr;
    QToolBar* mainToolBar = nullptr;
    QStatusBar* statusBar = nullptr;
};

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), pImpl(std::make_unique<Impl>()) {
    core::Logger::info("Initializing main window");

    setupUI();
    setupMenus();
    setupToolbars();
    setupStatusBar();

    setWindowTitle("OneDay Framework v1.0.0");
    setMinimumSize(1200, 800);
    resize(1400, 900);

    core::Logger::info("Main window initialized successfully");
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    pImpl->centralWidget = new QWidget(this);
    setCentralWidget(pImpl->centralWidget);

    auto* layout = new QVBoxLayout(pImpl->centralWidget);

    // 临时占位标签
    auto* welcomeLabel = new QLabel("欢迎使用 OneDay Framework", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #333;");

    layout->addWidget(welcomeLabel);

    core::Logger::debug("Main window UI setup completed");
}

void MainWindow::setupMenus() {
    pImpl->menuBar = menuBar();

    // 文件菜单
    auto* fileMenu = pImpl->menuBar->addMenu("文件(&F)");

    auto* newAction = fileMenu->addAction("新建项目(&N)");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewProject);

    auto* openAction = fileMenu->addAction("打开项目(&O)");
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenProject);

    auto* saveAction = fileMenu->addAction("保存项目(&S)");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveProject);

    fileMenu->addSeparator();

    auto* exitAction = fileMenu->addAction("退出(&X)");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &MainWindow::onExit);

    // 帮助菜单
    auto* helpMenu = pImpl->menuBar->addMenu("帮助(&H)");
    helpMenu->addAction("关于(&A)");

    core::Logger::debug("Menu bar setup completed");
}

void MainWindow::setupToolbars() {
    pImpl->mainToolBar = addToolBar("主工具栏");
    pImpl->mainToolBar->setObjectName("MainToolBar");

    // 添加基础工具栏按钮占位符
    pImpl->mainToolBar->addAction("新建");
    pImpl->mainToolBar->addAction("打开");
    pImpl->mainToolBar->addAction("保存");

    core::Logger::debug("Toolbar setup completed");
}

void MainWindow::setupStatusBar() {
    pImpl->statusBar = statusBar();
    pImpl->statusBar->showMessage("就绪", 2000);

    core::Logger::debug("Status bar setup completed");
}

void MainWindow::onNewProject() {
    core::Logger::info("New project requested");
    pImpl->statusBar->showMessage("创建新项目...", 2000);
    // 占位符实现
}

void MainWindow::onOpenProject() {
    core::Logger::info("Open project requested");
    pImpl->statusBar->showMessage("打开项目...", 2000);
    // 占位符实现
}

void MainWindow::onSaveProject() {
    core::Logger::info("Save project requested");
    pImpl->statusBar->showMessage("保存项目...", 2000);
    // 占位符实现
}

void MainWindow::onExit() {
    core::Logger::info("Application exit requested");
    QApplication::quit();
}

}  // namespace ui
}  // namespace oneday