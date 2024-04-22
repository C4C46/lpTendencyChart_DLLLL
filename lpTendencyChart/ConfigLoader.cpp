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
	return curveNames;
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
			m_treeWidget->setItemWidget(childItem, 0, checkBox); // 将复选框设置在左侧列
		}
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

		QString selectedCategory = curveNames.at(id);
		emit curveDisplayChanged(selectedCategory, true);
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