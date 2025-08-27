#pragma once

#include <QStringList>
#include <QWidget>

#include "../canvas/node_property.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QScrollArea;
class QGroupBox;
class QLineEdit;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QComboBox;
class QPushButton;

namespace oneday::ui {

/**
 * @brief 属性编辑面板类
 *
 * 用于编辑选中节点的属性和参数
 */
class PropertyPanel : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief 构造函数
     * @param parent 父窗口
     */
    explicit PropertyPanel(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~PropertyPanel() = default;

    /**
     * @brief 设置当前编辑的节点
     * @param nodeType 节点类型
     * @param nodeId 节点ID（用于标识具体的节点实例）
     * @param properties 节点属性（可选，如果提供则直接使用）
     */
    void setCurrentNode(const QString& nodeType,
                        const QString& nodeId = QString(),
                        const canvas::NodeProperties* properties = nullptr);

    /**
     * @brief 清空属性面板
     */
    void clearProperties();

    /**
     * @brief 刷新属性显示
     */
    void refreshProperties();

  signals:
    /**
     * @brief 属性值改变信号
     * @param nodeId 节点ID
     * @param propertyName 属性名称
     * @param value 新的属性值
     */
    void propertyChanged(const QString& nodeId, const QString& propertyName, const QVariant& value);

  private slots:
    /**
     * @brief 处理属性值改变
     */
    void onPropertyValueChanged();

  private:
    /**
     * @brief 设置用户界面
     */
    void setupUI();

    /**
     * @brief 创建基础属性组
     */
    void createBasicPropertiesGroup();

    /**
     * @brief 创建高级属性组
     */
    void createAdvancedPropertiesGroup();

    /**
     * @brief 创建外观属性组
     */
    void createAppearancePropertiesGroup();

    /**
     * @brief 根据属性定义创建属性控件
     * @param properties 节点属性
     */
    void createPropertiesFromDefinitions(const canvas::NodeProperties& properties);

    /**
     * @brief 根据属性定义添加属性控件
     * @param group 属性组
     * @param definition 属性定义
     * @param value 当前值
     */
    void addPropertyWidget(QGroupBox* group,
                           const canvas::PropertyDefinition& definition,
                           const QVariant& value);

    /**
     * @brief 添加字符串属性
     * @param group 属性组
     * @param name 属性名称
     * @param label 显示标签
     * @param defaultValue 默认值
     */
    void addStringProperty(QGroupBox* group,
                           const QString& name,
                           const QString& label,
                           const QString& defaultValue = QString());

    /**
     * @brief 添加整数属性
     * @param group 属性组
     * @param name 属性名称
     * @param label 显示标签
     * @param defaultValue 默认值
     * @param min 最小值
     * @param max 最大值
     */
    void addIntProperty(QGroupBox* group,
                        const QString& name,
                        const QString& label,
                        int defaultValue = 0,
                        int min = -999999,
                        int max = 999999);

    /**
     * @brief 添加浮点数属性
     * @param group 属性组
     * @param name 属性名称
     * @param label 显示标签
     * @param defaultValue 默认值
     * @param min 最小值
     * @param max 最大值
     * @param decimals 小数位数
     */
    void addDoubleProperty(QGroupBox* group,
                           const QString& name,
                           const QString& label,
                           double defaultValue = 0.0,
                           double min = -999999.0,
                           double max = 999999.0,
                           int decimals = 2);

    /**
     * @brief 添加布尔属性
     * @param group 属性组
     * @param name 属性名称
     * @param label 显示标签
     * @param defaultValue 默认值
     */
    void addBoolProperty(QGroupBox* group,
                         const QString& name,
                         const QString& label,
                         bool defaultValue = false);

    /**
     * @brief 添加枚举属性
     * @param group 属性组
     * @param name 属性名称
     * @param label 显示标签
     * @param options 选项列表
     * @param defaultIndex 默认选择索引
     */
    void addEnumProperty(QGroupBox* group,
                         const QString& name,
                         const QString& label,
                         const QStringList& options,
                         int defaultIndex = 0);

  private:
    // UI组件
    QScrollArea* m_scrollArea;  ///< 滚动区域
    QWidget* m_contentWidget;   ///< 内容窗口
    QVBoxLayout* m_mainLayout;  ///< 主布局

    // 属性组
    QGroupBox* m_basicGroup;       ///< 基础属性组
    QGroupBox* m_advancedGroup;    ///< 高级属性组
    QGroupBox* m_appearanceGroup;  ///< 外观属性组

    // 当前状态
    QString m_currentNodeType;  ///< 当前节点类型
    QString m_currentNodeId;    ///< 当前节点ID

    // 属性控件映射
    QHash<QString, QWidget*> m_propertyWidgets;  ///< 属性名到控件的映射
};

}  // namespace oneday::ui
