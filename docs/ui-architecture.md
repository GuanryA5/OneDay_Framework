# 无限蓝图游戏脚本开发框架 Frontend Architecture Document

## Template and Framework Selection

**框架决策：** 基于PRD和系统架构文档，本项目采用 **Qt6.6+ C++桌面应用架构**，而非Web前端技术栈。这是一个原生Windows桌面应用程序，使用Qt6框架提供用户界面。

**技术基础：**
- **Qt6.6+** - 成熟的跨平台GUI框架，特别适合复杂的桌面应用
- **C++20** - 现代C++标准，提供高性能和内存控制
- **Graphics View Framework** - Qt的高性能图形场景框架，专为节点编辑器优化
- **无Web技术依赖** - 纯桌面原生应用，不涉及HTML/CSS/JavaScript

### Change Log

| Date | Version | Description | Author |
|------|---------|-------------|--------|
| 2025-08-23 | 1.0 | 初始前端架构创建，基于Qt6桌面应用 | Frontend Architect |

## Frontend Tech Stack

基于主系统架构文档的技术选择，前端采用Qt6 C++技术栈：

### Technology Stack Table

| Category | Technology | Version | Purpose | Rationale |
|----------|------------|---------|---------|-----------|
| **GUI Framework** | Qt6 | 6.6+ | 核心用户界面框架 | 成熟稳定、高性能Graphics View、跨平台支持 |
| **Language** | C++ | 20 | 前端开发语言 | 与后端一致、高性能、内存控制 |
| **UI Layout** | Qt Widgets | 6.6+ | 传统界面组件 | 成熟稳定的桌面UI组件 |
| **2D Graphics** | Graphics View | 6.6+ | 蓝图编辑器渲染 | 专为复杂2D场景优化、支持大量图形对象 |
| **资源系统** | Qt Resource | 6.6+ | 静态资源管理 | 内置资源打包、主题切换支持 |
| **样式系统** | Qt StyleSheet | 6.6+ | 界面样式定制 | CSS-like语法、支持深色主题 |
| **动画系统** | Qt Animation | 6.6+ | UI动画效果 | 高性能属性动画、贝塞尔曲线支持 |
| **多线程** | Qt Concurrent | 6.6+ | 异步UI更新 | 线程安全的UI操作、信号槽机制 |
| **设置管理** | QSettings | 6.6+ | 用户配置持久化 | 跨平台设置存储 |
| **文件操作** | Qt File API | 6.6+ | 项目文件管理 | 完整的文件系统抽象 |
| **测试框架** | Qt Test | 6.6+ | 用户界面测试 | 专为Qt UI设计的测试框架 |

## Project Structure

基于Qt6 C++应用的最佳实践：

```
src/ui/                          # Qt6 用户界面模块
├── main_window/                 # 主窗口组件
│   ├── main_window.h/cpp        # QMainWindow主类
│   ├── main_window.ui           # Qt Designer界面文件
│   └── main_window_controller.h/cpp # 业务逻辑控制器
├── editors/                     # 专用编辑器
│   ├── blueprint_editor/        # 蓝图编辑器
│   │   ├── blueprint_scene.h/cpp      # QGraphicsScene场景
│   │   ├── blueprint_view.h/cpp       # QGraphicsView视图
│   │   ├── blueprint_node_item.h/cpp  # 节点图形项
│   │   ├── blueprint_connection.h/cpp # 连接线图形项
│   │   └── node_palette.h/cpp         # 节点面板
│   └── map_editor/              # 寻路地图编辑器
│       ├── map_scene.h/cpp      # 地图编辑场景
│       ├── map_view.h/cpp       # 地图编辑视图
│       └── annotation_tools.h/cpp # 标注工具
├── widgets/                     # 自定义控件
│   ├── property_panel.h/cpp     # 属性面板控件
│   ├── debug_console.h/cpp      # 调试控制台
│   ├── game_preview.h/cpp       # 游戏预览窗口
│   └── project_tree.h/cpp       # 项目树控件
├── dialogs/                     # 对话框
│   ├── settings_dialog.h/cpp    # 设置对话框
│   ├── new_project_dialog.h/cpp # 新建项目
│   └── about_dialog.h/cpp       # 关于对话框
├── models/                      # 数据模型
│   ├── blueprint_model.h/cpp    # 蓝图数据模型
│   ├── project_model.h/cpp      # 项目树模型
│   └── node_library_model.h/cpp # 节点库模型
├── resources/                   # 资源文件
│   ├── icons/                   # 图标资源
│   ├── themes/                  # 主题样式
│   │   ├── dark.qss            # 深色主题样式表
│   │   └── light.qss           # 浅色主题样式表
│   ├── fonts/                   # 字体文件
│   └── resources.qrc           # Qt资源配置
├── utils/                       # UI工具类
│   ├── ui_utils.h/cpp          # 界面工具函数
│   ├── theme_manager.h/cpp     # 主题管理器
│   └── settings_manager.h/cpp  # 设置管理器
└── forms/                       # .ui文件
    ├── main_window.ui
    ├── settings_dialog.ui
    └── new_project_dialog.ui
```

## Component Standards

### Component Template

基于Qt6 C++的组件创建模板：

```cpp
// blueprint_node_item.h
#ifndef BLUEPRINT_NODE_ITEM_H
#define BLUEPRINT_NODE_ITEM_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include "core/blueprint/nodes/base_node.h"

class BlueprintNodeItem : public QGraphicsItem
{
public:
    explicit BlueprintNodeItem(std::shared_ptr<BaseNode> node, 
                              QGraphicsItem* parent = nullptr);
    
    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, 
              const QStyleOptionGraphicsItem* option, 
              QWidget* widget = nullptr) override;
    
    // Node operations
    void updateFromModel();
    void setSelected(bool selected);
    void setExecutionState(NodeExecutionState state);
    
    // Port management
    QPointF getInputPortPos(int index) const;
    QPointF getOutputPortPos(int index) const;
    
signals:
    void nodeClicked(BlueprintNodeItem* node);
    void nodeDoubleClicked(BlueprintNodeItem* node);
    void nodeMoved(BlueprintNodeItem* node);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
    std::shared_ptr<BaseNode> m_node;
    NodeExecutionState m_executionState;
    bool m_selected;
    bool m_hovered;
    
    void updateGeometry();
    void drawNodeBody(QPainter* painter);
    void drawNodePorts(QPainter* painter);
    void drawNodeText(QPainter* painter);
};

#endif // BLUEPRINT_NODE_ITEM_H
```

### Naming Conventions

基于Qt6 C++和项目规范的命名约定：

- **类名：** PascalCase (BlueprintNodeItem, MainWindow)
- **文件名：** snake_case (blueprint_node_item.h/cpp)
- **成员变量：** m_前缀 + camelCase (m_nodeList, m_isSelected)
- **私有成员：** m_前缀 (m_impl, m_data)
- **静态成员：** s_前缀 (s_instance, s_defaultSettings)
- **常量：** UPPER_SNAKE_CASE (MAX_NODE_COUNT)
- **信号：** camelCase动词形式 (nodeClicked, valueChanged)
- **槽函数：** on + 信号源 + 信号名 (onNodeClicked, onValueChanged)
- **UI对象：** ui->控件类型 + 描述 (ui->pushButtonSave)

## State Management

### Store Structure

Qt6应用的数据管理结构：

```
src/models/
├── application_state.h/cpp      # 全局应用状态
├── blueprint_state_manager.h/cpp # 蓝图状态管理
├── project_state_manager.h/cpp   # 项目状态管理
├── ui_state_manager.h/cpp         # 界面状态管理
└── undo_redo_manager.h/cpp        # 撤销重做管理
```

### State Management Template

Qt6状态管理实现模板：

```cpp
// application_state.h
#ifndef APPLICATION_STATE_H
#define APPLICATION_STATE_H

#include <QObject>
#include <QSettings>
#include <memory>

class ApplicationState : public QObject
{
    Q_OBJECT
    
public:
    static ApplicationState& instance();
    
    // Project state
    QString currentProjectPath() const;
    void setCurrentProjectPath(const QString& path);
    
    // UI state
    bool isDarkTheme() const;
    void setDarkTheme(bool dark);
    
    QByteArray mainWindowGeometry() const;
    void setMainWindowGeometry(const QByteArray& geometry);
    
    // Settings persistence
    void saveSettings();
    void loadSettings();

signals:
    void currentProjectChanged(const QString& path);
    void themeChanged(bool isDark);
    void settingsChanged();

private:
    explicit ApplicationState(QObject* parent = nullptr);
    
    QString m_currentProjectPath;
    bool m_isDarkTheme;
    QByteArray m_mainWindowGeometry;
    
    std::unique_ptr<QSettings> m_settings;
};

#endif // APPLICATION_STATE_H
```

## API Integration

### Service Template

Qt6网络通信模板（如果需要）：

```cpp
// api_service.h
#ifndef API_SERVICE_H
#define API_SERVICE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>

class ApiService : public QObject
{
    Q_OBJECT
    
public:
    explicit ApiService(QObject* parent = nullptr);
    
    // Async API calls
    void uploadBlueprintData(const QJsonObject& data);
    void downloadTemplates();
    void checkForUpdates();

signals:
    void uploadCompleted(bool success, const QString& message);
    void templatesDownloaded(const QJsonArray& templates);
    void updateAvailable(const QString& version, const QString& url);
    void networkError(const QString& error);

private slots:
    void onReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError error);

private:
    QNetworkAccessManager* m_networkManager;
    
    void handleReply(QNetworkReply* reply);
    QJsonObject parseResponse(const QByteArray& data);
};

#endif // API_SERVICE_H
```

### API Client Configuration

由于这是桌面应用，主要是本地文件操作，网络配置相对简单：

```cpp
// network_config.cpp
void ApiService::setupNetworkManager()
{
    m_networkManager = new QNetworkAccessManager(this);
    
    // Set timeouts
    m_networkManager->setTransferTimeout(30000); // 30 seconds
    
    // Connect signals
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &ApiService::onReplyFinished);
    
    // User agent
    m_networkManager->setNetworkAccessible(
        QNetworkAccessManager::Accessible);
}
```

## Routing

Qt6桌面应用使用QStackedWidget进行页面管理：

### Route Configuration

```cpp
// navigation_manager.h
#ifndef NAVIGATION_MANAGER_H
#define NAVIGATION_MANAGER_H

#include <QObject>
#include <QStackedWidget>
#include <QHash>

enum class AppPage {
    Welcome,
    BlueprintEditor,
    MapEditor,
    Settings,
    About
};

class NavigationManager : public QObject
{
    Q_OBJECT
    
public:
    explicit NavigationManager(QStackedWidget* stackedWidget, 
                             QObject* parent = nullptr);
    
    void navigateTo(AppPage page);
    void goBack();
    bool canGoBack() const;
    
    AppPage currentPage() const;

signals:
    void pageChanged(AppPage page);
    void navigationHistoryChanged();

private:
    QStackedWidget* m_stackedWidget;
    QHash<AppPage, int> m_pageIndexMap;
    QList<AppPage> m_navigationHistory;
    int m_currentHistoryIndex;
};

#endif // NAVIGATION_MANAGER_H
```

## Styling Guidelines

### Styling Approach

Qt6使用QSS (Qt Style Sheets) 进行界面样式定制，语法类似CSS：

**主要特点：**
- CSS-like语法，易于理解和维护
- 支持选择器、属性继承、伪状态
- 完整的主题切换支持
- 高性能的样式缓存机制

### Global Theme Variables

深色主题样式表实现：

```css
/* themes/dark.qss */
/* 全局色彩变量 */
QWidget {
    background-color: #2d2d2d;
    color: #ffffff;
    font-family: "Segoe UI", "Microsoft YaHei";
    font-size: 14px;
}

/* 主窗口样式 */
QMainWindow {
    background-color: #2d2d2d;
    border: none;
}

/* 蓝图编辑器样式 */
QGraphicsView {
    background-color: #1e1e1e;
    border: 1px solid #404040;
    border-radius: 4px;
}

/* 节点样式 */
.BlueprintNodeItem {
    background-color: #404040;
    border: 2px solid #007acc;
    border-radius: 8px;
}

.BlueprintNodeItem:selected {
    border-color: #4a90e2;
    box-shadow: 0px 0px 10px rgba(74, 144, 226, 0.5);
}

.BlueprintNodeItem:hover {
    background-color: #4a4a4a;
}

/* 面板样式 */
QDockWidget {
    background-color: #333333;
    border: 1px solid #404040;
    titlebar-close-icon: url(:/icons/close_white.png);
    titlebar-normal-icon: url(:/icons/undock_white.png);
}

QDockWidget::title {
    background-color: #404040;
    color: #ffffff;
    padding: 4px;
    border-bottom: 1px solid #555555;
}

/* 按钮样式 */
QPushButton {
    background-color: #007acc;
    color: white;
    border: none;
    border-radius: 4px;
    padding: 8px 16px;
    font-weight: 500;
}

QPushButton:hover {
    background-color: #4a90e2;
}

QPushButton:pressed {
    background-color: #005a9e;
}

QPushButton:disabled {
    background-color: #666666;
    color: #999999;
}

/* 输入框样式 */
QLineEdit, QTextEdit {
    background-color: #1e1e1e;
    border: 1px solid #404040;
    border-radius: 4px;
    padding: 6px;
    color: white;
    selection-background-color: #007acc;
}

QLineEdit:focus, QTextEdit:focus {
    border-color: #007acc;
}

/* 菜单样式 */
QMenuBar {
    background-color: #2d2d2d;
    border-bottom: 1px solid #404040;
}

QMenuBar::item {
    background-color: transparent;
    padding: 6px 12px;
}

QMenuBar::item:selected {
    background-color: #404040;
}

QMenu {
    background-color: #333333;
    border: 1px solid #404040;
    border-radius: 4px;
}

QMenu::item {
    padding: 6px 24px;
    border: none;
}

QMenu::item:selected {
    background-color: #007acc;
}
```

## Testing Requirements

### Component Test Template

Qt6组件测试模板：

```cpp
// test_blueprint_node_item.cpp
#include <QtTest>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QApplication>
#include "ui/editors/blueprint_editor/blueprint_node_item.h"
#include "core/blueprint/nodes/base_node.h"

class TestBlueprintNodeItem : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testNodeCreation();
    void testNodeSelection();
    void testNodeMovement();
    void testPortPositions();
    void testExecutionStateUpdate();

private:
    QApplication* m_app;
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    BlueprintNodeItem* m_nodeItem;
    std::shared_ptr<BaseNode> m_node;
};

void TestBlueprintNodeItem::initTestCase()
{
    // Setup test environment
    int argc = 0;
    char* argv[] = {nullptr};
    m_app = new QApplication(argc, argv);
    
    m_scene = new QGraphicsScene(this);
    m_view = new QGraphicsView(m_scene);
    
    // Create test node
    m_node = std::make_shared<TestNode>();
    m_nodeItem = new BlueprintNodeItem(m_node);
    m_scene->addItem(m_nodeItem);
}

void TestBlueprintNodeItem::testNodeCreation()
{
    QVERIFY(m_nodeItem != nullptr);
    QVERIFY(m_nodeItem->boundingRect().isValid());
    QCOMPARE(m_nodeItem->scene(), m_scene);
}

void TestBlueprintNodeItem::testNodeSelection()
{
    // Test selection
    m_nodeItem->setSelected(true);
    QVERIFY(m_nodeItem->isSelected());
    
    // Test visual feedback
    QVERIFY(m_nodeItem->boundingRect().isValid());
}

void TestBlueprintNodeItem::cleanupTestCase()
{
    delete m_view;
    delete m_app;
}

QTEST_APPLESS_MAIN(TestBlueprintNodeItem)
#include "test_blueprint_node_item.moc"
```

### Testing Best Practices

1. **单元测试**: 测试独立组件的功能逻辑
2. **集成测试**: 测试组件间的交互和数据流
3. **UI测试**: 使用Qt Test框架测试用户交互
4. **覆盖率目标**: 争取80%代码覆盖率
5. **测试结构**: 准备-执行-断言模式
6. **模拟外部依赖**: 模拟后端模块、文件系统操作

## Environment Configuration

Qt6桌面应用的环境配置：

```cpp
// config/app_config.h
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <QString>
#include <QStandardPaths>

class AppConfig
{
public:
    // 应用程序路径
    static QString applicationDirPath();
    static QString documentsPath();
    static QString configPath();
    static QString logsPath();
    static QString tempPath();
    
    // 默认设置
    static constexpr int DEFAULT_MAX_NODES = 500;
    static constexpr int DEFAULT_CANVAS_SIZE = 10000;
    static constexpr int DEFAULT_ZOOM_MIN = 10;
    static constexpr int DEFAULT_ZOOM_MAX = 500;
    
    // 性能设置
    static constexpr int UI_UPDATE_INTERVAL = 16; // 60 FPS
    static constexpr int AUTOSAVE_INTERVAL = 300000; // 5 minutes
    
    // 主题设置
    static const QString DARK_THEME_PATH;
    static const QString LIGHT_THEME_PATH;
};

#endif // APP_CONFIG_H
```

## Frontend Developer Standards

### Critical Coding Rules

**Qt6桌面应用关键开发规则：**

1. **内存管理**: 所有QObject派生类使用父子关系管理内存，避免内存泄露
2. **信号槽连接**: 使用新语法连接信号槽，避免字符串拼写错误
3. **线程安全**: UI更新必须在主线程进行，使用QMetaObject::invokeMethod跨线程调用
4. **事件处理**: 重写事件处理函数时必须调用父类实现
5. **Graphics View优化**: 大量图形对象时使用QGraphicsItem::ItemIgnoresTransformations优化
6. **样式表**: 避免在代码中硬编码样式，使用QSS文件统一管理
7. **国际化**: 所有用户可见字符串使用tr()函数包装
8. **资源管理**: 图标、字体等资源使用Qt Resource System管理
9. **设置持久化**: 使用QSettings管理用户配置，支持跨平台
10. **错误处理**: Qt函数调用后检查返回值，记录错误日志

### Quick Reference

**Qt6桌面应用快速参考：**

```bash
# 开发命令
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt6
make -j4

# 测试命令
ctest --verbose

# 调试工具
gdb ./OneDay_Framework
valgrind --leak-check=full ./OneDay_Framework

# Qt Designer
designer forms/main_window.ui

# 资源编译
rcc resources/resources.qrc -o qrc_resources.cpp
```

**关键导入模式：**
```cpp
#include <QApplication>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
```

**项目特定工具：**
- **主题管理器**: `ThemeManager::instance().loadTheme("dark")`
- **设置管理**: `SettingsManager::instance().getValue("key")`
- **日志系统**: `LOG_INFO("message")`, `LOG_ERROR("error")`
- **撤销重做**: `UndoRedoManager::instance().executeCommand(cmd)`