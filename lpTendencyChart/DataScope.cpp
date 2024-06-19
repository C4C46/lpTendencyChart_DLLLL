#include "DataScope.h"
#pragma execution_character_set("utf-8")

DataScope::DataScope(QTableWidget* tableWidget, QObject* parent)
	: QObject(parent), data_tableWidget(tableWidget) {
	// 初始化表格设置
	data_tableWidget->setColumnCount(0); // 初始列数设置为0
	data_tableWidget->setHorizontalHeaderLabels(QStringList() << "X Value"); // 设置第一列标题
	data_tableWidget->verticalHeader()->setVisible(false);
	data_tableWidget->verticalScrollBar()->installEventFilter(this);  // 安装事件过滤器

	data_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	data_tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

	// 允许列拖放重排
	data_tableWidget->horizontalHeader()->setSectionsMovable(true);
	data_tableWidget->horizontalHeader()->setDragEnabled(true);
	data_tableWidget->horizontalHeader()->setDragDropMode(QAbstractItemView::InternalMove);
	data_tableWidget->horizontalHeader()->setDropIndicatorShown(true);

}



void DataScope::setColumnNames(const QStringList & names)
{


	saveTableSettings(m_columnNames);


	m_columnNames = names;
	data_tableWidget->setColumnCount(m_columnNames.size() + 1); // 加1是因为第一列是X值
	QStringList headers = QStringList() << "位置（m）";
	headers.append(m_columnNames);
	data_tableWidget->setHorizontalHeaderLabels(headers);

	// 隐藏行号
	data_tableWidget->verticalHeader()->setVisible(false);

	data_tableWidget->setColumnWidth(0, 100); // 设置第一列的固定宽度
	data_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第一列宽度固定

	QFontMetrics metrics(data_tableWidget->font());
	int minWidth = 220; // 最小宽度
	for (int i = 1; i < headers.size(); ++i) {
		int width = metrics.width(headers[i]) + 20; // 加20像素留白
		width = qMax(width, minWidth); // 确保不小于最小宽度
		data_tableWidget->setColumnWidth(i, width);
		data_tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
	
	}

	// 设置工具提示显示完整的列名称
	for (int i = 0; i < headers.size(); ++i) {
		QTableWidgetItem* headerItem = data_tableWidget->horizontalHeaderItem(i);
		if (!headerItem) {
			headerItem = new QTableWidgetItem();
			data_tableWidget->setHorizontalHeaderItem(i, headerItem);
		}
		headerItem->setToolTip(headers[i]);
	}

	loadTableSettings(m_columnNames);
}




void DataScope::addData(const QString &curveName, double x, double y, const QVariantList &warningValue, const QVariantList &alarmValue)
{
	int columnIndex = m_columnNames.indexOf(curveName) + 1; // 加1是因为第一列是X值
	if (columnIndex <= 0) {
		// 如果找不到对应的列名，直接返回
		return;
	}
	// 查找是否已存在该x值的行
	int existingRow = -1;
	for (int i = 0; i < data_tableWidget->rowCount(); ++i) {
		if (data_tableWidget->item(i, 0) && qFuzzyCompare(data_tableWidget->item(i, 0)->text().toDouble(), x)) {
			existingRow = i;
			break;
		}
	}

	if (existingRow == -1) { // 如果x值不存在，添加新行
		existingRow = data_tableWidget->rowCount();
		data_tableWidget->insertRow(existingRow);
		data_tableWidget->setItem(existingRow, 0, new QTableWidgetItem(QString::number(x))); // 设置X值
	}

	QTableWidgetItem *item = new QTableWidgetItem(QString::number(y)); // 创建新项设置Y值


		// 先检查报警值范围
	if (!alarmValue.isEmpty() && (y > alarmValue[0].toDouble() || y < alarmValue[1].toDouble())) {
		item->setBackground(Qt::red); // 设置背景为红色
	}
	// 然后检查预警值范围
	else if (!warningValue.isEmpty() && (y > warningValue[0].toDouble() || y < warningValue[1].toDouble())) {
		item->setBackground(QColor(255, 165, 0)); // 设置背景为橙色
	}
	// 否则设置为正常颜色
	else {
		item->setBackground(Qt::white); // 或者使用 item->setBackground(QBrush()); 来使用默认颜色
	}


	data_tableWidget->setItem(existingRow, columnIndex, item); // 更新对应列的Y值

	if (autoScrollEnabled) {
		data_tableWidget->scrollToBottom();
	}
}


bool DataScope::eventFilter(QObject *obj, QEvent *event) {
	if (obj == data_tableWidget->verticalScrollBar()) {
		if (event->type() == QEvent::MouseButtonPress) {
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			if (mouseEvent->button() == Qt::LeftButton) {
				// 鼠标左键按下，停止自动滚动
				autoScrollEnabled = false;
			}
			else if (mouseEvent->button() == Qt::RightButton) {
				// 鼠标右键按下，恢复自动滚动
				autoScrollEnabled = true;
				data_tableWidget->scrollToBottom();
				return true; // 消费掉此事件，防止进一步处理
			}
		}
		else if (event->type() == QEvent::Wheel) {
			QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
			// 检查是否滚动到底部
			if (data_tableWidget->verticalScrollBar()->value() == data_tableWidget->verticalScrollBar()->maximum()) {
				autoScrollEnabled = true;
				data_tableWidget->scrollToBottom();
			}
			else {
				autoScrollEnabled = false;
			}
		}
		else if (event->type() == QEvent::ContextMenu) {
			// 忽略上下文菜单事件
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

void DataScope::saveTableSettings(const QStringList& identifiers) {
	QString identifier = identifiers.join("_");
	qDebug() << "Saving settings for identifier:" << identifier;
	settingsCache["DataTableSettings_" + identifier + "_ColumnWidths"].clear();
	settingsCache["DataTableSettings_" + identifier + "_ColumnPositions"].clear();

	QVariantList widths, positions;
	for (int i = 0; i < data_tableWidget->columnCount(); ++i) {
		widths << data_tableWidget->columnWidth(i);
		positions << data_tableWidget->horizontalHeader()->visualIndex(i);
	}
	settingsCache["DataTableSettings_" + identifier + "_ColumnWidths"] = widths;
	settingsCache["DataTableSettings_" + identifier + "_ColumnPositions"] = positions;
}

void DataScope::loadTableSettings(const QStringList& identifiers) {
	QString identifier = identifiers.join("_");
	qDebug() << "Loading settings for identifier:" << identifier;
	QVariantList widths = settingsCache["DataTableSettings_" + identifier + "_ColumnWidths"].toList();
	QVariantList positions = settingsCache["DataTableSettings_" + identifier + "_ColumnPositions"].toList();

	for (int i = 0; i < widths.size(); ++i) {
		data_tableWidget->setColumnWidth(i, widths[i].toInt());
	}
	for (int i = 0; i < positions.size(); ++i) {
		data_tableWidget->horizontalHeader()->moveSection(data_tableWidget->horizontalHeader()->visualIndex(i), positions[i].toInt());
	}

}

void DataScope::loadSettingsFromFile() {
	QString settingsFile = QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini");
	QSettings settings(settingsFile, QSettings::IniFormat);
	settings.beginGroup("DataTableSettings");
	settingsCache = settings.value("SettingsCache").toMap();
	m_columnNames = settings.value("SelectedSubclassNames").toStringList();  // 加载列名

	settings.endGroup();
	qDebug() << "Settings loaded from file:" << settingsFile;
	qDebug() << "SettingsCache size:" << settingsCache.size();

	loadTableSettings(m_columnNames);

}

void DataScope::saveSettingsToFile() {
	QString settingsFile = QDir(QCoreApplication::applicationDirPath()).filePath("settings.ini");
	QSettings settings(settingsFile, QSettings::IniFormat);
	settings.beginGroup("DataTableSettings");

	settings.setValue("SelectedSubclassNames", m_columnNames);
	settings.setValue("SettingsCache", settingsCache);
	settings.endGroup();
	qDebug() << "Settings saved to file:" << settingsFile;
	qDebug() << "SettingsCache size:" << settingsCache.size();
}


