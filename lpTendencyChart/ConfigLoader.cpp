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
	m_treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_treeWidget->header()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_treeWidget->header()->hide(); // 隐藏表头
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

	m_treeWidget->setColumnWidth(0, 30); // 为单选框框列设置宽度
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

		QJsonArray childrenArray = categoryObject["children"].toArray();
		for (int j = 0; j < childrenArray.size(); ++j) {
			QJsonObject childObject = childrenArray[j].toObject();
			QString childName = childObject["name"].toString();

			QTreeWidgetItem *childItem = new QTreeWidgetItem(parentItem);
			childItem->setText(1, childName); // 子项名称也在右侧列
		}
	}

	connect(m_buttonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), [this](QAbstractButton *button) {
		int id = m_buttonGroup->id(button);
		for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
			QTreeWidgetItem *item = m_treeWidget->topLevelItem(i);
			if (i == id) {
				// 如果是当前点击的项，确保它是展开的
				m_treeWidget->expandItem(item);
			}
			else {
				// 其他项则收起
				m_treeWidget->collapseItem(item);
			}
		}

		QString selectedCategory = curveNames.at(id);
		emit curveDisplayChanged(selectedCategory, true);
	});
}

//void ConfigLoader::loadConfig(const QString &filePath)
//{
//	QString ReadPath = QCoreApplication::applicationDirPath() + "/" + filePath;
//	QFile file(ReadPath);
//	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//		return;
//
//	QByteArray fileData = file.readAll();
//	qDebug() << "File data:" << fileData; // 调试输出文件内容
//	QJsonDocument doc = QJsonDocument::fromJson(fileData);
//	qDebug() << "Is doc null?" << doc.isNull(); // 检查doc是否为空
//	qDebug() << "Is doc an object?" << doc.isObject(); // 检查doc是否为对象
//
//
//
//
//	if (doc.isNull() || !doc.isObject())
//		return;
//
//	QJsonObject jsonObject = doc.object();
//	QJsonArray categoriesArray = jsonObject["categories"].toArray();
//
//	m_tableWidget->setRowCount(categoriesArray.size() + 1); // 根据配置文件动态设置行数
//
//	m_tableWidget->setColumnWidth(0, 30); // 为单选框框列设置宽度
//	//m_tableWidget->setColumnWidth(1, 180); // 为名称列设置更大的宽度
//
//	QFont font = m_tableWidget->font();
//	font.setPointSize(font.pointSize() + 2); // 增加字体大小
//	m_tableWidget->setFont(font);
//
//	// 设置标题行
//	QTableWidgetItem* headerItem = new QTableWidgetItem("趋势图指标勾选");
//	headerItem->setFlags(Qt::ItemIsEnabled); // 设置为启用状态，但不可编辑
//	m_tableWidget->setItem(0, 0, headerItem);
//	m_tableWidget->setSpan(0, 0, 1, 2); // 将标题跨越两列
//
//	QButtonGroup* buttonGroup = new QButtonGroup(m_tableWidget); // 创建按钮组管理单选逻辑
//	buttonGroup->setExclusive(true); // 确保只能选择一个按钮
//
//	for (int i = 0; i < categoriesArray.size(); ++i) {
//		QJsonObject categoryObject = categoriesArray[i].toObject();
//		QString name = categoryObject["name"].toString();
//		bool selected = categoryObject["selected"].toBool(); // 获取选中状态
//
//		curveNames.append(name); // 存储曲线名称
//
//		QRadioButton* radioButton = new QRadioButton();
//		buttonGroup->addButton(radioButton, i); // 将单选按钮添加到按钮组
//		m_tableWidget->setCellWidget(i + 1, 0, radioButton);
//
//		if (selected) { // 如果该选项被选中，则设置单选按钮为选中状态
//			radioButton->setChecked(true);
//		}
//
//
//		QTableWidgetItem* nameItem = new QTableWidgetItem(name);
//		nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable); // 设置名称列不可编辑
//		m_tableWidget->setItem(i + 1, 1, nameItem);
//	}
//
//	// 连接按钮组的信号以处理选项更改
//	connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [this](int id) {
//		QString selectedCategory = curveNames.at(id);
//		emit curveDisplayChanged(selectedCategory, true);
//	});
//
//	m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第一列根据内容调整大小
//	m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // 第二列填充剩余空间
//}




void ConfigLoader::saveConfig(const QString &filePath)
{
	//QJsonArray categoriesArray;

	//// 假设 curveNames 存储了所有类别的名称，与表格行一一对应
	//for (int i = 0; i < m_tableWidget->rowCount() - 1; ++i) { // 减1是因为第一行可能是标题行
	//	QRadioButton* radioButton = qobject_cast<QRadioButton*>(m_tableWidget->cellWidget(i + 1, 0)); // 假设单选按钮在第0列
	//	if (radioButton) {
	//		QJsonObject categoryObject;
	//		categoryObject["name"] = curveNames.at(i); // 使用存储的类别名称
	//		categoryObject["selected"] = radioButton->isChecked(); // 设置选中状态
	//		// 如果有子类别信息需要保存，也可以在这里添加
	//		categoriesArray.append(categoryObject);
	//	}
	//}

	//QJsonObject rootObject;
	//rootObject["categories"] = categoriesArray; // 将更新后的类别数组放入根对象

	//QString WritePath = QCoreApplication::applicationDirPath() + "/" + filePath;
	//QFile file(WritePath);
	//if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
	//	return;

	//QJsonDocument doc(rootObject);
	//file.write(doc.toJson());
}