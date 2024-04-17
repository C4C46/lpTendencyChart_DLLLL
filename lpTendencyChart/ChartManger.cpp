#include "ChartManger.h"

#pragma execution_character_set("utf-8")

ChartManager::ChartManager(QObject *parent, QWidget *parentWidget, const QStringList &curveNames)
	: QObject(parent), m_widget(parentWidget), curveNames(curveNames)
{
	plot = new QwtPlot(m_widget);
	//plot->setTitle("实时趋势图");
	plot->setCanvasBackground(Qt::white);

	// 启用图例
	plot->insertLegend(new QwtLegend(), QwtPlot::TopLegend);

	// 设置X轴和Y轴的标题
	plot->setAxisTitle(QwtPlot::xBottom,"");
	plot->setAxisTitle(QwtPlot::yLeft,"");

	plot->setCanvasBackground(QColor(14,22,55));

	//// 设置默认的x轴和y轴间隔
	//plot->setAxisScale(QwtPlot::xBottom, 0, 100, xInterval); // 使用xInterval初始化x轴
	//plot->setAxisScale(QwtPlot::yLeft, 0, 100, yInterval); // 使用yInterval初始化y轴

	// 创建并配置网格
	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->attach(plot); // 将网格附加到图表
	grid->setVisible(false); // 隐藏网格线


	updaterThread = new ChartUpdaterThread(this, curveNames);

	if (!curveNames.isEmpty())
	{
		//定义颜色生成步长
		int colorStep = 360 / curveNames.size();
		for (int i = 0; i < curveNames.size(); ++i)
		{
			QColor color = QColor::fromHsv((colorStep * i) % 360, 255, 180);
			addCurve(curveNames[i], color);
		}
	}

	if (m_widget)
	{
		// 使用QSplitter代替原来的布局
		QSplitter *splitter = new QSplitter(Qt::Vertical, m_widget);
		splitter->addWidget(plot);


		table = new QTableWidget(m_widget);
		table->setRowCount(0); // 初始时没有行
		table->setColumnCount(curveNames.size() + 1); // 列数根据曲线数量设置
		QStringList headers = { "位置(m)" };
		headers.append(curveNames);
		table->setHorizontalHeaderLabels(headers);
		//table->setHorizontalHeaderLabels(curveNames); // 使用曲线名称作为列标题
		table->horizontalHeader()->setStretchLastSection(true);
		table->setEditTriggers(QAbstractItemView::NoEditTriggers);

		table->setColumnWidth(0, 100);
		table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);

		for (int i = 1; i < table->columnCount(); ++i) {
			table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
		}
		table->verticalHeader()->setVisible(false);
		splitter->addWidget(table);

		QVBoxLayout *layout = new QVBoxLayout(m_widget);
		layout->addWidget(splitter);
		m_widget->setLayout(layout);

	}

	QwtLegend *legend = new QwtLegend();
	legend->setDefaultItemMode(QwtLegendData::Clickable);
	plot->insertLegend(legend, QwtPlot::TopLegend);


	connect(updaterThread, &ChartUpdaterThread::updateChart, this, &ChartManager::onChartUpdate);
	// 连接图例点击信号
	connect(legend, SIGNAL(clicked(const QVariant &, int)), this, SLOT(onLegendClicked(const QVariant &, int)));

	installEventFilters();

	//plot->replot(); // 重绘图表以应用新的轴间隔

}



ChartManager::~ChartManager() {
	if (updaterThread) {
		updaterThread->stopRunning();
		updaterThread->wait(); 
		delete updaterThread; 
		updaterThread = nullptr;
	}

}


void ChartManager::start() {
	if (updaterThread && !updaterThread->isRunning()) {
		updaterThread->start();
	}
}

QWidget* ChartManager::getWidget() {
	return m_widget;
}


void ChartManager::onChartUpdate(const QString &curveName, int x, qreal y) {


	if (!xDataMap.contains(curveName) || !yDataMap.contains(curveName)) {
		return; // 如果曲线名称不存在，则直接返回
	}

	xDataMap[curveName] << x;
	yDataMap[curveName] << y;

	for (auto &curve : curves) {
		if (curve->title().text() == curveName) {
			curve->setSamples(xDataMap[curveName], yDataMap[curveName]); // 更新指定曲线的数据点
			break;
		}
	}
		// 确定当前数据点所在的X轴区间
	int xIntervalIndex = x / xInterval; // 计算当前数据点属于哪个间隔区间
	double xMin = xIntervalIndex * xInterval;
	double xMax = xMin + xInterval;

	// Y轴区间计算
	int yIntervalIndex = y / yInterval; // 计算当前数据点属于哪个间隔区间
	double yMin = yIntervalIndex * yInterval;
	double yMax = yMin + yInterval;

	// 更新X轴和Y轴的范围
	plot->setAxisScale(QwtPlot::xBottom, xMin, xMax);
	plot->setAxisScale(QwtPlot::yLeft, yMin, yMax);

	plot->replot(); // 重绘图表


	// 检查是否需要添加新行
	int newRow = table->rowCount();
	table->insertRow(newRow); // 插入新行
	  // 在第一列设置x坐标
	table->setItem(newRow, 0, new QTableWidgetItem(QString::number(x)));

	// 确定曲线名称对应的列索引（加1因为第一列是x坐标）
	int columnIndex = curveNames.indexOf(curveName) + 1;
	if (columnIndex == 0) return; // 如果找不到对应的曲线名称，直接返回

	// 在对应的列下添加y值
	table->setItem(newRow, columnIndex, new QTableWidgetItem(QString::number(y)));

	// 按照曲线更新数据，可能需要填充其他列的空白单元格
	for (int i = 0; i < table->columnCount(); ++i) {
		if (i != columnIndex && !table->item(newRow, i)) {
			table->setItem(newRow, i, new QTableWidgetItem("NaN")); // 填充空白以保持表格整齐
		}
	}

	table->scrollToBottom();
}

void ChartManager::onIntervalPBClicked() {
	QDialog dialog(m_widget); // 使用当前widget作为父窗口
	QVBoxLayout layout(&dialog);
	dialog.setWindowTitle("参数设置");

	QLabel xLabel("设置x轴间隔米数：", &dialog);
	QLineEdit xInput(&dialog);
	xInput.setText(QString::number(xInterval)); // 使用xInterval初始化文本
	xInput.setValidator(new QDoubleValidator(0, 10000, 2, &xInput)); // 限制输入为数字

	QLabel yLabel("设置y轴间隔毫米数：", &dialog);
	QLineEdit yInput(&dialog);
	yInput.setText(QString::number(yInterval)); // 使用yInterval初始化文本
	yInput.setValidator(new QDoubleValidator(0, 10000, 2, &yInput)); // 限制输入为数字

	QPushButton confirmButton("确认", &dialog);
	connect(&confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);

	layout.addWidget(&xLabel);
	layout.addWidget(&xInput);
	layout.addWidget(&yLabel);
	layout.addWidget(&yInput);
	layout.addWidget(&confirmButton);

	// 显示对话框
	if (dialog.exec() == QDialog::Accepted) {
		xInterval = xInput.text().toDouble();
		yInterval = yInput.text().toDouble();

		// 计算新的X轴和Y轴范围
		double xMin = 0;
		double xMax = xMin + xInterval; // 以xInterval为间隔计算xMax
		double yMin = 0;
		double yMax = yMin + yInterval; // 以yInterval为间隔计算yMax

		// 更新图表的X轴和Y轴范围
		plot->setAxisScale(QwtPlot::xBottom, xMin, xMax);
		plot->setAxisScale(QwtPlot::yLeft, yMin, yMax);
		plot->replot(); // 重绘图表以应用新的间隔
	}
}


void ChartManager::addCurve(const QString &curveName, const QColor &color) {
	QwtPlotCurve *curve = new QwtPlotCurve(curveName);
	curve->setTitle(curveName); // 设置曲线的标题，这将在图例中显示
		// 设置曲线标题的字体大小
	QwtText title(curveName);
	QFont font;
	font.setPointSize(12); // 设置字体大小为12点
	title.setFont(font);
	curve->setTitle(title);

	curve->setPen(color, 3); // 设置曲线颜色和宽度
	curve->attach(plot);
	curves.append(curve);
	xDataMap[curveName] = QVector<double>(); // 初始化数据存储
	yDataMap[curveName] = QVector<double>();
}

void ChartManager::onLegendClicked(const QVariant &itemInfo, int index) {
	QwtPlotItem *clickedItem = plot->infoToItem(itemInfo);
	if (!clickedItem) return;

	// 首先重置所有曲线到初始透明度状态
	resetCurvesOpacity();

	// 然后设置未被点击曲线的透明度
	for (auto &curve : curves) {
		if (curve != clickedItem) {
			QColor color = curve->pen().color();
			color.setAlpha(50); // 设置为半透明
			curve->setPen(QPen(color, 2));
		}
	}

	plot->replot(); // 重绘图表以应用更改
}


void ChartManager::resetCurvesOpacity() {
	for (auto &curve : curves) {
		QColor color = curve->pen().color();
		color.setAlpha(255); // 设置为完全不透明
		curve->setPen(QPen(color, 3));
	}
}


void ChartManager::installEventFilters() {
	plot->canvas()->installEventFilter(this);
}

bool ChartManager::eventFilter(QObject *watched, QEvent *event) {
	if (watched == plot->canvas() && event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		if (mouseEvent->button() == Qt::LeftButton) {
			resetCurvesOpacity();
			plot->replot();
			return true; // 表示事件已处理
		}
	}
	return QObject::eventFilter(watched, event); // 对于其他事件，调用基类的事件过滤器方法
}


void ChartManager::onCurveDisplayChanged(const QString &curveName, bool display) {
	if (display) {
		// 检查曲线是否已存在
		bool curveExists = false;
		for (auto &curve : curves) {
			if (curve->title().text() == curveName) {
				curveExists = true;
				break;
			}
		}

		if (!curveExists) {
			// 曲线不存在，创建新曲线
			QColor color = QColor::fromHsv((curves.size() * 30) % 360, 255, 180); // 示例颜色
			addCurve(curveName, color); // 假设这个方法已经实现

			// 更新ChartUpdaterThread中的曲线名称列表
			QStringList updatedCurveNames;
			for (auto &curve : curves) {
				updatedCurveNames.append(curve->title().text());
			}
			updaterThread->updateCurveNames(updatedCurveNames);

			// 添加曲线名称到curveNames列表，用于表格列的同步
			curveNames.append(curveName);

			// 添加列到表格
			int newColumnIndex = table->columnCount();
			table->insertColumn(newColumnIndex);
			table->setHorizontalHeaderItem(newColumnIndex, new QTableWidgetItem(curveName));
			// 为已有行的新列填充默认值
			for (int row = 0; row < table->rowCount(); ++row) {
				table->setItem(row, newColumnIndex, new QTableWidgetItem("NaN"));
			}
		}
	}
	else {
		// 处理隐藏曲线的逻辑
		for (auto &curve : curves) {
			if (curve->title().text() == curveName) {
				curve->detach(); // 从图表中移除曲线
				curves.removeOne(curve); // 从曲线列表中移除
				break;
			}
		}

		// 更新ChartUpdaterThread中的曲线名称列表
		QStringList updatedCurveNames;
		for (auto &curve : curves) {
			updatedCurveNames.append(curve->title().text());
		}
		updaterThread->updateCurveNames(updatedCurveNames);
		int columnIndex = curveNames.indexOf(curveName) + 1; // +1 因为第一列是固定的“位置(m)”
		if (columnIndex > 0) {
			table->removeColumn(columnIndex);
			curveNames.removeAll(curveName); // 从曲线名称列表中移除
		}
	}

	plot->replot(); // 重绘图表
}