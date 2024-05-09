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



}

void DataScope::setColumnNames(const QStringList & names)
{

	m_columnNames = names;
	data_tableWidget->setColumnCount(m_columnNames.size() + 1); // 加1是因为第一列是X值
	QStringList headers = QStringList() << "位置（m）";
	headers.append(m_columnNames);
	data_tableWidget->setHorizontalHeaderLabels(headers);

	// 隐藏行号
	data_tableWidget->verticalHeader()->setVisible(false);

	data_tableWidget->setColumnWidth(0, 100); // 设置第一列的固定宽度
	data_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第一列宽度固定
	// 设置其余列为可交互的拉伸模式
	for (int i = 1; i < data_tableWidget->columnCount(); ++i) {
		data_tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
	}

	  // 设置最后一列顶满表格
    data_tableWidget->horizontalHeader()->setSectionResizeMode(data_tableWidget->columnCount() - 1, QHeaderView::Stretch);




	// 设置工具提示显示完整的列名称
	for (int i = 0; i < headers.size(); ++i) {
		QTableWidgetItem* headerItem = data_tableWidget->horizontalHeaderItem(i);
		if (!headerItem) {
			headerItem = new QTableWidgetItem();
			data_tableWidget->setHorizontalHeaderItem(i, headerItem);
		}
		headerItem->setToolTip(headers[i]);
	}
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