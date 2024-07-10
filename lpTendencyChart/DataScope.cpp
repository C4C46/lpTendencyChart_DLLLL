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

	QFont font = data_tableWidget->font();
	font.setPointSize(12);
	data_tableWidget->setFont(font);

	//updateTimer = new QTimer(this);
	//connect(updateTimer, &QTimer::timeout, this, &DataScope::batchUpdateTable);
	//updateTimer->start(1000); // 每1000毫秒（1秒）触发一次

	qRegisterMetaType< QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>>>("QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>>");
	m_dataScopeThread = new lpDataScopeThread();
	m_thread = new QThread(this);
	m_dataScopeThread->moveToThread(m_thread);

	//connect(this, &DataScope::sgDataCache, m_dataScopeThread, &lpDataScopeThread::onDataCache);
	//connect(this, SIGNAL(sgDataCache12()), m_dataScopeThread, SLOT(onDataCache12()));

	connect(m_dataScopeThread, &lpDataScopeThread::sgSendData, this, &DataScope::onSendData);
	

	connect(m_thread, &QThread::started, m_dataScopeThread, &lpDataScopeThread::process);

	m_thread->start();

}



DataScope::~DataScope()
{
	if (m_thread && m_thread->isRunning()) {
		m_dataScopeThread->stopThread();  
		m_thread->quit();
		m_thread->wait();
	}
	delete m_dataScopeThread;

	//updateTimer->stop();
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

{    // 将数据添加到缓存中
	dataCache[curveName].append(qMakePair(x, qMakePair(y, QVariantList{ warningValue, alarmValue })));

	hasNewData = true; // 设置有新数据的标志
	//emit sgDataCache(dataCache);
	m_dataScopeThread->onDataCache(dataCache);

}


//数据更新
void DataScope::onSendData(QString DataName,double xData,double yData, QVariantList warningValue, QVariantList AlarmingValue)
{


	int columnIndex = m_columnNames.indexOf(DataName) + 1;
	if (columnIndex <= 0) return;

	// 使用哈希表来存储xData与行号的映射，减少查找时间
	static QMap<double, int> xDataToRowMap;
	int existingRow = xDataToRowMap.value(xData, -1);
	data_tableWidget->setUpdatesEnabled(false);
	if (existingRow == -1) {
		existingRow = data_tableWidget->rowCount();
		data_tableWidget->insertRow(existingRow);
		data_tableWidget->setItem(existingRow, 0, new QTableWidgetItem(QString::number(xData)));
		xDataToRowMap[xData] = existingRow; // 更新映射表
	}

	QTableWidgetItem *item = new QTableWidgetItem(QString::number(yData));
	// 设置背景颜色根据警告和报警值
	if (!AlarmingValue.isEmpty() && (yData > AlarmingValue[0].toDouble() || yData < AlarmingValue[1].toDouble())) {
		item->setBackground(Qt::red);
	}
	else if (!warningValue.isEmpty() && (yData > warningValue[0].toDouble() || yData < warningValue[1].toDouble())) {
		item->setBackground(QColor(255, 165, 0));
	}
	else {
		item->setBackground(Qt::white);
	}

	data_tableWidget->setItem(existingRow, columnIndex, item);
	data_tableWidget->setUpdatesEnabled(true);

	if (autoScrollEnabled) {
		data_tableWidget->scrollToBottom();
	}
	dataCache.clear(); // 清空缓存

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
	//qDebug() << "Saving settings for identifier:" << identifier;
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
	//qDebug() << "Loading settings for identifier:" << identifier;
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


