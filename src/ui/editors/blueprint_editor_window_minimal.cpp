#include "blueprint_editor_window_minimal.h"

#include <QHBoxLayout>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>

#include "../canvas/infinite_canvas.h"
#include "node_palette.h"
#include "property_panel.h"

namespace oneday {
namespace ui {
namespace editors {

BlueprintEditorWindowMinimal::BlueprintEditorWindowMinimal(QWidget* parent)
    : QMainWindow(parent), m_canvas(nullptr), m_nodePalette(nullptr), m_statusLabel(nullptr) {
    std::cout << "Creating minimal blueprint editor window..." << std::endl;

    try {
        setupUI();

        // 测试QSettings（这可能是问题之一）
        std::cout << "Testing QSettings..." << std::endl;
        QSettings settings("OneDay", "BlueprintEditor");
        settings.setValue("test", "value");
        QString testValue = settings.value("test", "default").toString();
        std::cout << "QSettings test successful: " << testValue.toStdString() << std::endl;

        std::cout << "Minimal blueprint editor window created successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error creating minimal window: " << e.what() << std::endl;
        throw;
    }
}

void BlueprintEditorWindowMinimal::setupUI() {
    std::cout << "Setting up UI with infinite canvas and node palette..." << std::endl;

    // 设置窗口属性
    setWindowTitle("OneDay Framework - Blueprint Editor (Full Layout)");
    setMinimumSize(1200, 700);
    resize(1600, 900);

    // 创建主分割器
    QSplitter* mainSplitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(mainSplitter);

    // 创建节点调色板
    std::cout << "Creating node palette..." << std::endl;
    m_nodePalette = new ui::NodePalette(this);
    m_nodePalette->setMinimumWidth(250);
    m_nodePalette->setMaximumWidth(350);
    mainSplitter->addWidget(m_nodePalette);
    std::cout << "Node palette created" << std::endl;

    // 创建无限画布
    std::cout << "Creating infinite canvas..." << std::endl;
    m_canvas = new canvas::InfiniteCanvas(this);
    mainSplitter->addWidget(m_canvas);
    std::cout << "Infinite canvas created" << std::endl;

    // 创建属性面板
    std::cout << "Creating property panel..." << std::endl;
    m_propertyPanel = new ui::PropertyPanel(this);
    mainSplitter->addWidget(m_propertyPanel);
    std::cout << "Property panel created" << std::endl;

    // 设置分割器比例 (节点调色板:画布:属性面板 = 1:3:1)
    mainSplitter->setStretchFactor(0, 1);  // 节点调色板
    mainSplitter->setStretchFactor(1, 3);  // 画布
    mainSplitter->setStretchFactor(2, 1);  // 属性面板

    // 创建状态栏
    m_statusLabel = new QLabel("蓝图编辑器 - 从左侧拖拽节点到画布，选中节点查看右侧属性面板", this);
    statusBar()->addWidget(m_statusLabel);

    // 连接画布信号
    connect(m_canvas, &canvas::InfiniteCanvas::zoomLevelChanged, this, [this](qreal zoom) {
        m_statusLabel->setText(
            QString("缩放: %1% - 从左侧拖拽节点到画布，使用Ctrl+滚轮缩放").arg(qRound(zoom * 100)));
    });

    connect(m_canvas, &canvas::InfiniteCanvas::canvasClicked, this, [this](const QPointF& pos) {
        std::cout << "Canvas clicked at: (" << pos.x() << ", " << pos.y() << ")" << std::endl;
    });

    connect(m_canvas,
            &canvas::InfiniteCanvas::nodeDropped,
            this,
            [this](const QString& nodeType, const QPointF& pos) {
                std::cout << "Node dropped on canvas: " << nodeType.toStdString() << " at ("
                          << pos.x() << ", " << pos.y() << ")" << std::endl;
                m_statusLabel->setText(QString("在画布上创建节点: %1 位置: (%2, %3)")
                                           .arg(nodeType)
                                           .arg(qRound(pos.x()))
                                           .arg(qRound(pos.y())));
                // 在画布上实际创建节点图形
                m_canvas->createNode(nodeType, pos);
            });

    // 连接节点调色板信号
    connect(m_nodePalette, &ui::NodePalette::nodeRequested, this, [this](const QString& nodeType) {
        std::cout << "Node requested: " << nodeType.toStdString() << std::endl;
        m_statusLabel->setText(QString("双击创建节点: %1").arg(nodeType));
        // 在画布中心创建节点
        QPointF centerPos = m_canvas->mapToScene(m_canvas->viewport()->rect().center());
        m_canvas->createNode(nodeType, centerPos);
    });

    std::cout << "UI setup complete with node palette integration" << std::endl;
}

}  // namespace editors
}  // namespace ui
}  // namespace oneday
