#pragma once

#include <QMainWindow>
#include <memory>

namespace oneday {
namespace ui {

/**
 * @brief 主窗口类
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewProject();
    void onOpenProject();
    void onSaveProject();
    void onExit();

private:
    void setupUI();
    void setupMenus();
    void setupToolbars();
    void setupStatusBar();
    
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace ui
} // namespace oneday