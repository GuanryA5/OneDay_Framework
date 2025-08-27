#include "property_panel.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>
#include <iostream>

namespace oneday::ui {

PropertyPanel::PropertyPanel(QWidget* parent)
    : QWidget(parent),
      m_scrollArea(nullptr),
      m_contentWidget(nullptr),
      m_mainLayout(nullptr),
      m_basicGroup(nullptr),
      m_advancedGroup(nullptr),
      m_appearanceGroup(nullptr) {
    setupUI();
    std::cout << "PropertyPanel created" << std::endl;
}

void PropertyPanel::setupUI() {
    // 设置面板属性
    setMinimumWidth(280);
    setMaximumWidth(400);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // 创建主布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(0);

    // 创建标题
    QLabel* titleLabel = new QLabel("属性编辑器", this);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: #333333;"
        "    padding: 8px;"
        "    background-color: #f0f0f0;"
        "    border-bottom: 1px solid #cccccc;"
        "}");
    layout->addWidget(titleLabel);

    // 创建滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    layout->addWidget(m_scrollArea);

    // 创建内容窗口
    m_contentWidget = new QWidget();
    m_contentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    m_scrollArea->setWidget(m_contentWidget);

    // 创建主布局
    m_mainLayout = new QVBoxLayout(m_contentWidget);
    m_mainLayout->setContentsMargins(8, 8, 8, 8);
    m_mainLayout->setSpacing(8);

    // 创建默认的空状态提示
    QLabel* emptyLabel = new QLabel("请选择一个节点\n来编辑其属性", m_contentWidget);
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setStyleSheet(
        "QLabel {"
        "    color: #888888;"
        "    font-size: 12px;"
        "    padding: 20px;"
        "}");
    m_mainLayout->addWidget(emptyLabel);
    m_mainLayout->addStretch();
}

void PropertyPanel::setCurrentNode(const QString& nodeType,
                                   const QString& nodeId,
                                   const canvas::NodeProperties* properties) {
    if (m_currentNodeType == nodeType && m_currentNodeId == nodeId) {
        return;  // 没有变化，不需要更新
    }

    std::cout << "PropertyPanel: Setting current node - Type: " << nodeType.toStdString()
              << ", ID: " << nodeId.toStdString() << std::endl;

    m_currentNodeType = nodeType;
    m_currentNodeId = nodeId;

    // 清空现有内容
    clearProperties();

    if (!nodeType.isEmpty()) {
        if (properties) {
            // 使用提供的属性
            createPropertiesFromDefinitions(*properties);
        } else {
            // 根据节点类型创建默认属性
            canvas::NodeProperties defaultProps =
                canvas::NodePropertyFactory::createPropertiesForNodeType(nodeType);
            createPropertiesFromDefinitions(defaultProps);
        }
    }
}

void PropertyPanel::clearProperties() {
    // 清空所有子控件
    QLayoutItem* item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    // 清空属性控件映射
    m_propertyWidgets.clear();

    // 重置属性组指针
    m_basicGroup = nullptr;
    m_advancedGroup = nullptr;
    m_appearanceGroup = nullptr;

    if (m_currentNodeType.isEmpty()) {
        // 显示空状态
        QLabel* emptyLabel = new QLabel("请选择一个节点\n来编辑其属性", m_contentWidget);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet(
            "QLabel {"
            "    color: #888888;"
            "    font-size: 12px;"
            "    padding: 20px;"
            "}");
        m_mainLayout->addWidget(emptyLabel);
        m_mainLayout->addStretch();
    }
}

void PropertyPanel::refreshProperties() {
    // 刷新当前属性显示
    if (!m_currentNodeType.isEmpty()) {
        setCurrentNode(m_currentNodeType, m_currentNodeId);
    }
}

void PropertyPanel::createPropertiesFromDefinitions(const canvas::NodeProperties& properties) {
    std::cout << "Creating properties from definitions" << std::endl;

    // 获取所有分类
    QStringList categories = properties.getCategories();

    // 为每个分类创建属性组
    QHash<QString, QGroupBox*> categoryGroups;

    for (const QString& category : categories) {
        QGroupBox* group = new QGroupBox(category, m_contentWidget);
        group->setStyleSheet(
            "QGroupBox {"
            "    font-weight: bold;"
            "    border: 1px solid #cccccc;"
            "    border-radius: 4px;"
            "    margin-top: 8px;"
            "    padding-top: 8px;"
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    left: 8px;"
            "    padding: 0 4px 0 4px;"
            "}");

        QVBoxLayout* layout = new QVBoxLayout(group);
        layout->setContentsMargins(8, 8, 8, 8);
        layout->setSpacing(6);

        m_mainLayout->addWidget(group);
        categoryGroups[category] = group;
    }

    // 为每个属性创建控件
    QList<canvas::PropertyDefinition> definitions = properties.getAllPropertyDefinitions();
    for (const auto& definition : definitions) {
        if (!definition.visible)
            continue;

        QGroupBox* group = categoryGroups.value(definition.category);
        if (group) {
            QVariant currentValue = properties.getPropertyValue(definition.name);
            addPropertyWidget(group, definition, currentValue);
        }
    }

    // 添加弹性空间
    m_mainLayout->addStretch();
}

void PropertyPanel::addPropertyWidget(QGroupBox* group,
                                      const canvas::PropertyDefinition& definition,
                                      const QVariant& value) {
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(group->layout());
    if (!layout)
        return;

    switch (definition.type) {
        case canvas::PropertyType::String: {
            QHBoxLayout* rowLayout = new QHBoxLayout();

            QLabel* labelWidget = new QLabel(definition.displayName + ":", group);
            labelWidget->setMinimumWidth(80);
            labelWidget->setStyleSheet("QLabel { font-weight: normal; }");

            QLineEdit* lineEdit = new QLineEdit(value.toString(), group);
            lineEdit->setStyleSheet(
                "QLineEdit {"
                "    border: 1px solid #cccccc;"
                "    border-radius: 3px;"
                "    padding: 4px;"
                "    background-color: white;"
                "}"
                "QLineEdit:focus {"
                "    border-color: #4A90E2;"
                "}");

            connect(
                lineEdit, &QLineEdit::textChanged, this, &PropertyPanel::onPropertyValueChanged);

            rowLayout->addWidget(labelWidget);
            rowLayout->addWidget(lineEdit);
            layout->addLayout(rowLayout);
            m_propertyWidgets[definition.name] = lineEdit;
            break;
        }

        case canvas::PropertyType::Integer: {
            QHBoxLayout* rowLayout = new QHBoxLayout();

            QLabel* labelWidget = new QLabel(definition.displayName + ":", group);
            labelWidget->setMinimumWidth(80);
            labelWidget->setStyleSheet("QLabel { font-weight: normal; }");

            QSpinBox* spinBox = new QSpinBox(group);
            spinBox->setRange(definition.minValue.toInt(), definition.maxValue.toInt());
            spinBox->setValue(value.toInt());
            spinBox->setStyleSheet(
                "QSpinBox {"
                "    border: 1px solid #cccccc;"
                "    border-radius: 3px;"
                "    padding: 4px;"
                "    background-color: white;"
                "}"
                "QSpinBox:focus {"
                "    border-color: #4A90E2;"
                "}");

            connect(spinBox,
                    QOverload<int>::of(&QSpinBox::valueChanged),
                    this,
                    &PropertyPanel::onPropertyValueChanged);

            rowLayout->addWidget(labelWidget);
            rowLayout->addWidget(spinBox);
            layout->addLayout(rowLayout);
            m_propertyWidgets[definition.name] = spinBox;
            break;
        }

        case canvas::PropertyType::Double: {
            QHBoxLayout* rowLayout = new QHBoxLayout();

            QLabel* labelWidget = new QLabel(definition.displayName + ":", group);
            labelWidget->setMinimumWidth(80);
            labelWidget->setStyleSheet("QLabel { font-weight: normal; }");

            QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(group);
            doubleSpinBox->setRange(definition.minValue.toDouble(), definition.maxValue.toDouble());
            doubleSpinBox->setValue(value.toDouble());
            doubleSpinBox->setDecimals(2);
            doubleSpinBox->setStyleSheet(
                "QDoubleSpinBox {"
                "    border: 1px solid #cccccc;"
                "    border-radius: 3px;"
                "    padding: 4px;"
                "    background-color: white;"
                "}"
                "QDoubleSpinBox:focus {"
                "    border-color: #4A90E2;"
                "}");

            connect(doubleSpinBox,
                    QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                    this,
                    &PropertyPanel::onPropertyValueChanged);

            rowLayout->addWidget(labelWidget);
            rowLayout->addWidget(doubleSpinBox);
            layout->addLayout(rowLayout);
            m_propertyWidgets[definition.name] = doubleSpinBox;
            break;
        }

        case canvas::PropertyType::Boolean: {
            QCheckBox* checkBox = new QCheckBox(definition.displayName, group);
            checkBox->setChecked(value.toBool());
            checkBox->setStyleSheet(
                "QCheckBox {"
                "    font-weight: normal;"
                "    spacing: 8px;"
                "}"
                "QCheckBox::indicator {"
                "    width: 16px;"
                "    height: 16px;"
                "}"
                "QCheckBox::indicator:unchecked {"
                "    border: 1px solid #cccccc;"
                "    border-radius: 3px;"
                "    background-color: white;"
                "}"
                "QCheckBox::indicator:checked {"
                "    border: 1px solid #4A90E2;"
                "    border-radius: 3px;"
                "    background-color: #4A90E2;"
                "}");

            connect(checkBox, &QCheckBox::toggled, this, &PropertyPanel::onPropertyValueChanged);

            layout->addWidget(checkBox);
            m_propertyWidgets[definition.name] = checkBox;
            break;
        }

        case canvas::PropertyType::Enum: {
            QHBoxLayout* rowLayout = new QHBoxLayout();

            QLabel* labelWidget = new QLabel(definition.displayName + ":", group);
            labelWidget->setMinimumWidth(80);
            labelWidget->setStyleSheet("QLabel { font-weight: normal; }");

            QComboBox* comboBox = new QComboBox(group);
            comboBox->addItems(definition.enumOptions);

            // 设置当前值
            QString currentText = value.toString();
            int index = comboBox->findText(currentText);
            if (index >= 0) {
                comboBox->setCurrentIndex(index);
            }

            comboBox->setStyleSheet(
                "QComboBox {"
                "    border: 1px solid #cccccc;"
                "    border-radius: 3px;"
                "    padding: 4px;"
                "    background-color: white;"
                "}"
                "QComboBox:focus {"
                "    border-color: #4A90E2;"
                "}"
                "QComboBox::drop-down {"
                "    border: none;"
                "}"
                "QComboBox::down-arrow {"
                "    width: 12px;"
                "    height: 12px;"
                "}");

            connect(comboBox,
                    QOverload<int>::of(&QComboBox::currentIndexChanged),
                    this,
                    &PropertyPanel::onPropertyValueChanged);

            rowLayout->addWidget(labelWidget);
            rowLayout->addWidget(comboBox);
            layout->addLayout(rowLayout);
            m_propertyWidgets[definition.name] = comboBox;
            break;
        }

        default:
            // 其他类型暂时不支持
            break;
    }
}

void PropertyPanel::onPropertyValueChanged() {
    QWidget* sender = qobject_cast<QWidget*>(QObject::sender());
    if (!sender)
        return;

    // 查找属性名称
    QString propertyName;
    for (auto it = m_propertyWidgets.begin(); it != m_propertyWidgets.end(); ++it) {
        if (it.value() == sender) {
            propertyName = it.key();
            break;
        }
    }

    if (propertyName.isEmpty())
        return;

    // 获取新值
    QVariant newValue;
    if (QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender)) {
        newValue = lineEdit->text();
    } else if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(sender)) {
        newValue = spinBox->value();
    } else if (QDoubleSpinBox* doubleSpinBox = qobject_cast<QDoubleSpinBox*>(sender)) {
        newValue = doubleSpinBox->value();
    } else if (QCheckBox* checkBox = qobject_cast<QCheckBox*>(sender)) {
        newValue = checkBox->isChecked();
    } else if (QComboBox* comboBox = qobject_cast<QComboBox*>(sender)) {
        newValue = comboBox->currentText();
    }

    std::cout << "Property changed - Name: " << propertyName.toStdString()
              << ", Value: " << newValue.toString().toStdString() << std::endl;

    // 发出属性改变信号
    emit propertyChanged(m_currentNodeId, propertyName, newValue);
}

}  // namespace oneday::ui