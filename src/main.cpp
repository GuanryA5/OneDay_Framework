#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <iostream>
#include <memory>

#include "core/common/config.h"
#include "core/common/encoding_utils.h"
#include "core/common/logger.h"
#include "ui/editors/blueprint_editor_window_minimal.h"

int main(int argc, char* argv[]) {
    try {
        std::cout << "Starting OneDay Framework..." << std::endl;

        // 设置控制台UTF-8编码（Windows下必需）
        // oneday::core::EncodingUtils::setConsoleUtf8();  // 临时注释掉测试
        std::cout << "Console encoding step skipped for testing" << std::endl;

        QApplication app(argc, argv);
        std::cout << "QApplication created successfully" << std::endl;

        // 设置应用程序信息
        app.setApplicationName("OneDay Framework");
        app.setApplicationVersion("1.0.0");
        app.setOrganizationName("OneDay");
        app.setOrganizationDomain("oneday-framework.com");
        std::cout << "Application info set" << std::endl;

        // 初始化日志系统
        try {
            oneday::core::Logger::initialize();
            std::cout << "Logger initialized successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
            // 继续运行，不退出
        }

        // 加载配置
        try {
            auto config = std::make_unique<oneday::core::Config>();
            if (!config->load("config.json")) {
                std::cout << "Warning: Failed to load configuration, using defaults" << std::endl;
                if (oneday::core::Logger::isInitialized()) {
                    oneday::core::Logger::warning("Failed to load configuration, using defaults");
                }
            } else {
                std::cout << "Configuration loaded successfully" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception while loading config: " << e.what() << std::endl;
            // 继续运行，使用默认配置
        }

        // 创建蓝图编辑器主窗口（最小化版本用于测试）
        std::cout << "Creating minimal blueprint editor window..." << std::endl;
        oneday::ui::editors::BlueprintEditorWindowMinimal window;
        std::cout << "Minimal blueprint editor window created" << std::endl;

        window.show();
        std::cout << "Window shown, starting event loop..." << std::endl;

        if (oneday::core::Logger::isInitialized()) {
            oneday::core::Logger::info("OneDay Framework Blueprint Editor started successfully");
        }

        int result = app.exec();
        std::cout << "Application finished with code: " << result << std::endl;
        return result;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        QMessageBox::critical(
            nullptr, "Fatal Error", QString("A fatal error occurred:\n%1").arg(e.what()));
        return -1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        QMessageBox::critical(nullptr, "Fatal Error", "An unknown fatal error occurred");
        return -1;
    }
}