#pragma once

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QLabel>
#include <QAction>
#include <QActionGroup>
#include <QProgressBar>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QTimer>
#include <QSettings>

QT_BEGIN_NAMESPACE
class QGraphicsView;
class QGraphicsScene;
class QTreeWidget;
class QPropertyBrowser;
QT_END_NAMESPACE

namespace oneday {
namespace ui {
namespace editors {

/**
 * @brief 蓝图编辑器主窗口
 * 
 * 这是蓝图编辑器的主窗口类，提供完整的用户界面框架，
 * 包括菜单栏、工具栏、状态栏和各种停靠面板。
 */
class BlueprintEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BlueprintEditorWindow(QWidget *parent = nullptr);
    ~BlueprintEditorWindow();

    // 窗口管理
    void showMaximized();
    void restoreWindowState();
    void saveWindowState();

    // 主题和样式
    void setDarkTheme();
    void setLightTheme();
    void applyCustomStyleSheet();

public slots:
    // 文件操作
    void newBlueprint();
    void openBlueprint();
    void saveBlueprint();
    void saveBlueprintAs();
    void exportBlueprint();
    void importBlueprint();

    // 编辑操作
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void deleteSelected();

    // 视图操作
    void zoomIn();
    void zoomOut();
    void zoomFit();
    void zoomReset();
    void toggleGrid();
    void toggleSnap();

    // 工具操作
    void setSelectTool();
    void setPanTool();
    void setZoomTool();
    void setConnectTool();

    // 窗口操作
    void toggleNodePalette();
    void togglePropertyPanel();
    void toggleOutlinePanel();
    void toggleConsolePanel();
    void resetLayout();

    // 帮助操作
    void showAbout();
    void showHelp();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent *event) override;

private slots:
    void updateStatusBar();
    void updateWindowTitle();
    void onZoomChanged(int zoomLevel);
    void onSelectionChanged();

private:
    void setupUI();
    void createMenuBar();
    void createToolBars();
    void createStatusBar();
    void createDockWidgets();
    void createCentralWidget();
    void connectSignals();
    void setupShortcuts();
    void loadSettings();
    void saveSettings();

    // UI组件
    QWidget *m_centralWidget;
    QVBoxLayout *m_centralLayout;
    QSplitter *m_mainSplitter;

    // 菜单
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_viewMenu;
    QMenu *m_toolsMenu;
    QMenu *m_windowMenu;
    QMenu *m_helpMenu;

    // 工具栏
    QToolBar *m_mainToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_viewToolBar;
    QToolBar *m_toolsToolBar;

    // 状态栏组件
    QLabel *m_statusLabel;
    QLabel *m_coordinatesLabel;
    QLabel *m_zoomLabel;
    QLabel *m_selectionLabel;
    QProgressBar *m_progressBar;
    QComboBox *m_zoomComboBox;

    // 停靠面板
    QDockWidget *m_nodePaletteDock;
    QDockWidget *m_propertyPanelDock;
    QDockWidget *m_outlinePanelDock;
    QDockWidget *m_consolePanelDock;

    // 面板内容（稍后实现）
    QWidget *m_nodePaletteWidget;
    QWidget *m_propertyPanelWidget;
    QWidget *m_outlinePanelWidget;
    QWidget *m_consolePanelWidget;

    // 动作组
    QActionGroup *m_toolActionGroup;
    QActionGroup *m_viewActionGroup;

    // 动作
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_exportAction;
    QAction *m_importAction;
    QAction *m_exitAction;

    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_selectAllAction;
    QAction *m_deleteAction;

    QAction *m_zoomInAction;
    QAction *m_zoomOutAction;
    QAction *m_zoomFitAction;
    QAction *m_zoomResetAction;
    QAction *m_toggleGridAction;
    QAction *m_toggleSnapAction;

    QAction *m_selectToolAction;
    QAction *m_panToolAction;
    QAction *m_zoomToolAction;
    QAction *m_connectToolAction;

    QAction *m_aboutAction;
    QAction *m_helpAction;

    // 设置和状态
    QSettings *m_settings;
    QTimer *m_statusUpdateTimer;
    QString m_currentFilePath;
    bool m_isModified;
    int m_currentZoomLevel;
    QString m_currentTool;
};

} // namespace editors
} // namespace ui
} // namespace oneday
