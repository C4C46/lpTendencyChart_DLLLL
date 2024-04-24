#include "ConfigLoader.h"
#include <QFile>
#include <QTextStream>
#include <QButtonGroup>
#include <QRadioButton>
#pragma execution_character_set("utf-8")

ConfigLoader::ConfigLoader(QTreeWidget *treeWidget, QObject *parent)
	: QObject(parent), m_treeWidget(treeWidget)
{
	m_treeWidget->setColumnCount(2); // 设置为2列，一列是名称，一列是单选按钮
	m_treeWidget->header()->setSectionResizeMode(0, QHeaderView::Fixed);
	m_treeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_treeWidget->setHeaderLabels(QStringList() << "" << "趋势勾选指标"); // 第一列留空，第二列设置为“趋势勾选指标”
	//m_treeWidget->header()->hide(); // 隐藏表头
	m_treeWidget->setRootIsDecorated(false); // 隐藏树状结构的展开/收缩箭头
	m_buttonGroup = new QButtonGroup(this);
	m_buttonGroup->setExclusive(true);
}

QStringList ConfigLoader::getCurveNames() const {
	QStringList selectedCurveNames;
	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parentItem = m_treeWidget->topLevelItem(i);
		for (int j = 0; j < parentItem->childCount(); ++j) {
			QTreeWidgetItem *childItem = parentItem->child(j);
			QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
			if (checkBox && checkBox->isChecked()) {
				selectedCurveNames.append(childItem->text(1)); // 添加选中的子项名称
			}
		}
	}
	return selectedCurveNames;
}



QStringList ConfigLoader::getParentCategoryNames() const {
	QStringList parentNames;

	if (configDoc.isNull() || !configDoc.isObject()) {
		return parentNames;
	}

	QJsonObject rootObj = configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (const QJsonValue& categoryVal : categories) {
		QJsonObject categoryObj = categoryVal.toObject();
		QString categoryName = categoryObj["name"].toString();
		// 添加父类名称到列表中
		parentNames.append(categoryName);
	}

	return parentNames;
}


QVariantMap ConfigLoader::getSettingDefaultValue(const QString& settingName) {
	QVariantMap settingDefaults;


	if (configDoc.isNull() || !configDoc.isObject()) {
		return settingDefaults;
	}

	QJsonObject rootObj = configDoc.object();
	QJsonArray categories = rootObj["categories"].toArray();

	for (const QJsonValue& categoryVal : categories) {
		QJsonObject categoryObj = categoryVal.toObject();
		QString categoryName = categoryObj["name"].toString();


		if (categoryName == settingName) {
			qDebug() << "Object:" << categoryObj;
			// 找到了匹配的设置项，读取其默认值
			if (categoryObj.contains("settings")) {
				QJsonObject settingsObj = categoryObj["settings"].toObject();
				// 从settingsObj中读取yAxisRange
				if (settingsObj.contains("yAxisRange")) {
					QJsonArray yAxisRange = settingsObj["yAxisRange"].toArray();
					settingDefaults["yAxisRange"] = QVariantList{ yAxisRange.at(0).toDouble(), yAxisRange.at(1).toDouble() };
				}
				// 从settingsObj中读取warningValue
				if (settingsObj.contains("warningValue")) {
					QJsonArray warningValue = settingsObj["warningValue"].toArray();
					settingDefaults["warningValue"] = QVariantList{ warningValue.at(0).toDouble(), warningValue.at(1).toDouble() };
				}
				// 从settingsObj中读取alarmValue
				if (settingsObj.contains("alarmValue")) {
					QJsonArray alarmValue = settingsObj["alarmValue"].toArray();
					settingDefaults["alarmValue"] = QVariantList{ alarmValue.at(0).toDouble(), alarmValue.at(1).toDouble() };
				}
			}
			break; // 找到后退出循环
		}
	}

	return settingDefaults;
}

void ConfigLoader::loadConfig(const QString &filePath) {
	QString readPath = QCoreApplication::applicationDirPath() + "/" + filePath;
	QFile file(readPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QByteArray fileData = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(fileData);

	if (doc.isNull() || !doc.isObject())
		return;

	configDoc = doc;

	// 打印整个文档以验证其内容
	qDebug() << "Config loaded:" << configDoc.toJson(QJsonDocument::Compact);

	QJsonObject jsonObject = doc.object();
	QJsonArray categoriesArray = jsonObject["categories"].toArray();

	m_treeWidget->setColumnWidth(0, 35);
	m_treeWidget->setColumnWidth(1, 180); // 为名称列设置更大的宽度

	QFont font = m_treeWidget->font();
	font.setPointSize(font.pointSize() + 2); // 增加字体大小
	m_treeWidget->setFont(font);


	for (int i = 0; i < categoriesArray.size(); ++i) {
		QJsonObject categoryObject = categoriesArray[i].toObject();
		QString name = categoryObject["name"].toString();
		curveNames.append(name);

		QTreeWidgetItem *parentItem = new QTreeWidgetItem(m_treeWidget);
		parentItem->setText(1, name); // 将名称设置在右侧列

		QRadioButton *radioButton = new QRadioButton();
		m_treeWidget->setItemWidget(parentItem, 0, radioButton); // 将单选按钮设置在左侧列
		m_buttonGroup->addButton(radioButton, i);


		// 根据配置文件设置单选按钮的选中状态
		bool selected = categoryObject["selected"].toBool();
		radioButton->setChecked(selected);

		QJsonArray childrenArray = categoryObject["children"].toArray();
		for (int j = 0; j < childrenArray.size(); ++j) {
			QJsonObject childObject = childrenArray[j].toObject();
			QString childName = childObject["name"].toString();
			bool display = childObject["display"].toBool();

			QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
			childItem->setText(1, childName); // 子项名称也在右侧列


			QCheckBox *checkBox = new QCheckBox();
			checkBox->setChecked(display); // 根据配置文件设置复选框的状态
			checkBox->setEnabled(selected); // 根据父项的选中状态启用或禁用复选框
			m_treeWidget->setItemWidget(childItem, 0, checkBox); // 将复选框设置在左侧列

			QObject::connect(checkBox, &QCheckBox::toggled, [this, childName](bool checked) {
				emit curveDisplayChanged(childName, checked);
			});
		}

		// 当单选按钮状态改变时，更新子复选框的可用状态
		QObject::connect(radioButton, &QRadioButton::toggled, [this, parentItem, categoryObject](bool checked) {
			if (checked) {
				// 发出Y轴范围改变的信号
				QVariantMap settingDefaults = getSettingDefaultValue(parentItem->text(1));
				if (settingDefaults.contains("yAxisRange")) {
					emit yAxisRangeChanged(settingDefaults["yAxisRange"].toList());
				}
				if (settingDefaults.contains("warningValue")) {
					emit warningValueChanged(settingDefaults["warningValue"].toList());
				}
				if (settingDefaults.contains("alarmValue")) {
					emit alarmValueChanged(settingDefaults["alarmValue"].toList());
				}
			}
			
			for (int j = 0; j < parentItem->childCount(); ++j) {
				QTreeWidgetItem *childItem = parentItem->child(j);
				QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
				if (checkBox) {
					checkBox->setEnabled(checked);
					if (!checked) checkBox->setChecked(false); // 如果父项未选中，也取消子项的勾选状态
				}
			}
		});
		//// 如果父项被选中，则展开该项
		//if (selected) {
		//	m_treeWidget->expandItem(parentItem);
		//}

	}

	connect(m_buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), [this](QAbstractButton *button) {
		int id = m_buttonGroup->id(button);
		for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
			QTreeWidgetItem *item = m_treeWidget->topLevelItem(i);
			if (i == id) {
				// 如果是当前点击的项，确保它是展开的
				m_treeWidget->expandItem(item);
				// 勾选所有子项的复选框
				for (int j = 0; j < item->childCount(); ++j) {
					QTreeWidgetItem *childItem = item->child(j);
					QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
					if (checkBox) checkBox->setChecked(true);
				}
			}
			else {
				// 其他项则收起
				m_treeWidget->collapseItem(item);
				// 取消勾选所有子项的复选框
				for (int j = 0; j < item->childCount(); ++j) {
					QTreeWidgetItem *childItem = item->child(j);
					QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
					if (checkBox) checkBox->setChecked(false);
				}
			}
		}

		//QString selectedCategory = curveNames.at(id);
		//emit curveDisplayChanged(selectedCategory, true);
	});
}




void ConfigLoader::saveConfig(const QString &filePath)
{
	QString savePath = QCoreApplication::applicationDirPath() + "/" + filePath;
	QFile file(savePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QJsonObject rootObject;
	QJsonArray categoriesArray;

	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
		QTreeWidgetItem *parentItem = m_treeWidget->topLevelItem(i);
		QJsonObject categoryObject;
		categoryObject["name"] = parentItem->text(1); // 使用右侧列的文本作为名称

		QRadioButton *radioButton = qobject_cast<QRadioButton *>(m_treeWidget->itemWidget(parentItem, 0));
		categoryObject["selected"] = radioButton && radioButton->isChecked(); // 检查单选按钮是否被选中

		QVariantMap settingDefaults = getSettingDefaultValue(parentItem->text(1));
		QJsonObject settingsObject; // 创建一个新的JSON对象来存储设置
		if (!settingDefaults.isEmpty()) {
			if (settingDefaults.contains("yAxisRange")) {
				settingsObject["yAxisRange"] = QJsonArray::fromVariantList(settingDefaults["yAxisRange"].toList());
			}
			if (settingDefaults.contains("warningValue")) {
				settingsObject["warningValue"] = QJsonArray::fromVariantList(settingDefaults["warningValue"].toList());
			}
			if (settingDefaults.contains("alarmValue")) {
				settingsObject["alarmValue"] = QJsonArray::fromVariantList(settingDefaults["alarmValue"].toList());
			}
		}
		categoryObject["settings"] = settingsObject; // 将设置对象添加到类别对象中

		QJsonArray childrenArray;
		for (int j = 0; j < parentItem->childCount(); ++j) {
			QTreeWidgetItem *childItem = parentItem->child(j);
			QJsonObject childObject;
			childObject["name"] = childItem->text(1); // 子项名称

			QCheckBox *checkBox = qobject_cast<QCheckBox *>(m_treeWidget->itemWidget(childItem, 0));
			childObject["display"] = checkBox && checkBox->isChecked(); // 检查复选框是否被勾选

			childrenArray.append(childObject);
		}

		categoryObject["children"] = childrenArray;
		categoriesArray.append(categoryObject);
	}

	rootObject["categories"] = categoriesArray;

	QJsonDocument doc(rootObject);
	file.write(doc.toJson());
	file.close();
}

QString ConfigLoader::getSelectedParentNames() const {

	for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i)
	{
		QTreeWidgetItem *item = m_treeWidget->topLevelItem(i);
		QRadioButton *radioButton = qobject_cast<QRadioButton *>(m_treeWidget->itemWidget(item, 0));
		if (radioButton && radioButton->isChecked())
		{
			return item->text(1);//返回選擇的父類名稱
		}
	}

	return QString();

}