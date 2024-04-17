#include "ConfigLoader.h"
#include <QFile>
#include <QTextStream>
#pragma execution_character_set("utf-8")

ConfigLoader::ConfigLoader(QTableWidget *tableWidget, QObject *parent)
	: QObject(parent), m_tableWidget(tableWidget)
{
	m_tableWidget->setColumnCount(2); // 设置为2列，一列是复选框，一列是名称
	m_tableWidget->verticalHeader()->setVisible(false); // 隐藏行号
	m_tableWidget->horizontalHeader()->setVisible(false); // 隐藏列号
	m_tableWidget->setShowGrid(false);
}

QStringList ConfigLoader::getCurveNames() const {
	return curveNames;
}


void ConfigLoader::loadConfig(const QString &filePath)
{
	QString ReadPath = QCoreApplication::applicationDirPath() + "/" + filePath;
	QFile file(ReadPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QByteArray fileData = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(fileData);

	if (doc.isNull() || !doc.isObject())
		return;

	QJsonObject jsonObject = doc.object();
	QJsonArray configArray = jsonObject["event_name"].toArray();

	m_tableWidget->setRowCount(configArray.size() + 1); // 根据配置文件动态设置行数

	m_tableWidget->setColumnWidth(0, 30); // 为复选框列设置宽度
	//m_tableWidget->setColumnWidth(1, 180); // 为名称列设置更大的宽度

	QFont font = m_tableWidget->font();
	font.setPointSize(font.pointSize() + 2); // 增加字体大小
	m_tableWidget->setFont(font);

	// 添加标题行
	QTableWidgetItem* headerItem = new QTableWidgetItem("趋势图指标勾选");
	headerItem->setFlags(Qt::ItemIsEnabled); // 设置为启用状态，但不可编辑
	m_tableWidget->setItem(0, 0, headerItem);
	m_tableWidget->setSpan(0, 0, 1, 2); // 将标题跨越两列

	for (int i = 0; i < configArray.size(); ++i) {
		QJsonObject configObject = configArray[i].toObject();
		QString name = configObject["name"].toString();
		bool display = configObject["display"].toBool();

		if (display) { // 只有当display为true时，才考虑这个曲线名称
			curveNames.append(name); // 存储曲线名称
		}


		QCheckBox* checkBox = new QCheckBox();
		checkBox->setChecked(display);
		connect(checkBox, &QCheckBox::toggled, this, [this, name](bool checked) {
			emit curveDisplayChanged(name, checked);
		});
		m_tableWidget->setCellWidget(i + 1, 0, checkBox);

		QTableWidgetItem* nameItem = new QTableWidgetItem(name);
		nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable); // 设置名称列不可编辑
		m_tableWidget->setItem(i + 1, 1, nameItem);
	}
	m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第一列根据内容调整大小
	m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // 第二列填充剩余空间
}

void ConfigLoader::saveConfig(const QString &filePath)
{
	QJsonArray configArray;

	for (int i = 0; i < m_tableWidget->rowCount(); ++i) {
		QCheckBox* checkBox = qobject_cast<QCheckBox*>(m_tableWidget->cellWidget(i, 0));
		QTableWidgetItem* item = m_tableWidget->item(i, 1);

		if (checkBox && item) {
			QJsonObject configObject;
			configObject["name"] = item->text();
			configObject["display"] = checkBox->isChecked();
			configArray.append(configObject);
		}
	}

	QJsonObject jsonObject;
	jsonObject["event_name"] = configArray;


	QString WritePath = QCoreApplication::applicationDirPath() + "/" + filePath;
	QFile file(WritePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QJsonDocument doc(jsonObject);
	file.write(doc.toJson());
}


