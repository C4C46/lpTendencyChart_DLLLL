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
		////定义颜色生成步长
		//int colorStep = 360 / curveNames.size();
		for (int i = 0; i < curveNames.size(); ++i)
		{
			//QColor color = QColor::fromHsv((colorStep * i) % 360, 255, 180);
			addCurve(curveNames[i]);
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
	//QVBoxLayout layout(&dialog);
	dialog.setWindowTitle("预警值设置");
	dialog.resize(650, 500);
	dialog.setFixedSize(dialog.size());

	QGridLayout *gridLayout = new QGridLayout();

	QStringList warningLabels = {
	"设置A/B面左侧陶瓷宽度预警值（毫米）",
	"设置A/B面右侧陶瓷宽度预警值（毫米）", // 假设的新标签
	"设置A/B面电浆宽度预警值（毫米）", // 假设的新标签
	"设置A/B面整体宽度预警值（毫米）"  // 假设的新标签
	};


	QStringList alarmLabels = {
	"设置A/B面左侧陶瓷宽度告警值（毫米）",
	"设置A/B面右侧陶瓷宽度告警值（毫米）", // 假设的新标签
	"设置A/B面电浆宽度告警值（毫米）", // 假设的新标签
	"设置A/B面整体宽度告警值（毫米）"  // 假设的新标签
	};

	for (int i = 0; i < warningLabels.size(); ++i) {

		int row = i / 2; // 计算行
		int col = i % 2; // 计算列

		QVBoxLayout *vLayout = new QVBoxLayout();
		// 预警值标签
		QLabel* warningLabel = new QLabel(warningLabels[i], &dialog);
		warningLabel->setStyleSheet("QLabel { color: orange; font-weight: bold;}");
		vLayout->addWidget(warningLabel);

		// 上限区域
		QHBoxLayout* upperLayout = new QHBoxLayout;
		QLabel* upperLabel = new QLabel("上限区域：", &dialog);
		QLineEdit* upperInput1 = new QLineEdit(&dialog);
		upperInput1->setValidator(new QDoubleValidator(0, 10000, 2, upperInput1));

		QLabel *separator1 = new QLabel(" 一 ", &dialog); // 分隔符
		QLineEdit* upperInput2 = new QLineEdit(&dialog);
		upperInput2->setValidator(new QDoubleValidator(0, 10000, 2, upperInput2));
		upperLayout->addWidget(upperLabel);
		upperLayout->addWidget(upperInput1);
		upperLayout->addWidget(separator1); // 添加分隔符
		upperLayout->addWidget(upperInput2);
		vLayout->addLayout(upperLayout);


		// 添加一个垂直空白控件作为上限区域和下限区域之间的间隔
		vLayout->addItem(new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));

		// 下限区域
		QHBoxLayout* lowerLayout = new QHBoxLayout;
		QLabel* lowerLabel = new QLabel("下限区域：", &dialog);
		QLineEdit* lowerInput1 = new QLineEdit(&dialog);
		lowerInput1->setValidator(new QDoubleValidator(0, 10000, 2, lowerInput1));

		QLabel *separator2 = new QLabel(" 一 ", &dialog);
		QLineEdit* lowerInput2 = new QLineEdit(&dialog);
		lowerInput2->setValidator(new QDoubleValidator(0, 10000, 2, lowerInput2));
		lowerLayout->addWidget(lowerLabel);
		lowerLayout->addWidget(lowerInput1);
		lowerLayout->addWidget(separator2);
		lowerLayout->addWidget(lowerInput2);
		vLayout->addLayout(lowerLayout);

		// 告警值标签
		QLabel* alarmLabel = new QLabel(alarmLabels[i], &dialog);
		alarmLabel->setStyleSheet("QLabel { color: #A52A2A; font-weight: bold; }");
		vLayout->addWidget(alarmLabel);

		// 告警值区域
		QHBoxLayout* alarmLayout = new QHBoxLayout;
		QLabel* greaterLabel = new QLabel("大于", &dialog);
		QLineEdit* greaterInput = new QLineEdit(&dialog);
		greaterInput->setValidator(new QDoubleValidator(0, 10000, 2, greaterInput));
		QLabel* lessLabel = new QLabel("或小于", &dialog);
		QLineEdit* lessInput = new QLineEdit(&dialog);
		lessInput->setValidator(new QDoubleValidator(0, 10000, 2, lessInput));
		alarmLayout->addWidget(greaterLabel);
		alarmLayout->addWidget(greaterInput);
		alarmLayout->addWidget(lessLabel);
		alarmLayout->addWidget(lessInput);
		vLayout->addLayout(alarmLayout);

		// 将垂直布局添加到网格布局的相应位置
		gridLayout->addLayout(vLayout, row, col);
	}


	// 设置网格布局的间距
	gridLayout->setHorizontalSpacing(20); // 水平间距
	gridLayout->setVerticalSpacing(10); // 垂直间距

	// 设置对话框的样式
	dialog.setStyleSheet("QDialog { background-color: #f0f0f0; }"
		"QLabel { font-size: 12pt; }"
		"QLineEdit { border: 1px solid #999999; border-radius: 4px; padding: 2px; background-color: #ffffff; }"
		"QPushButton { border: 2px solid #4a76a8; border-radius: 6px; background-color: #5a8bbf; padding: 5px; font-size: 10pt; color: white; min-width: 80px; }"
		"QPushButton:hover { background-color: #6b9cd5; }"
		"QPushButton:pressed { background-color: #487aa1; }");

	// 其他布局和控件设置保持不变


	// 创建确定和取消按钮
	QPushButton *confirmButton = new QPushButton("确定", &dialog);
	QPushButton *cancelButton = new QPushButton("取消", &dialog);


	// 设置按钮的布局
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch(); // 添加弹性空间，使按钮靠右对齐
	buttonLayout->addWidget(confirmButton);
	buttonLayout->addSpacing(10); // 在两个按钮之间添加10像素的间距
	buttonLayout->addWidget(cancelButton);


	// 连接按钮的信号与槽
	QObject::connect(confirmButton, &QPushButton::clicked, &dialog, &QDialog::accept);
	QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

	// 将按钮布局添加到网格布局的下方
	gridLayout->addLayout(buttonLayout, warningLabels.size() / 2 + 1, 0, 1, 2);

	// 设置网格布局的间距和边距
	gridLayout->setHorizontalSpacing(30); // 增加水平间距
	gridLayout->setVerticalSpacing(15); // 增加垂直间距
	gridLayout->setContentsMargins(20, 20, 20, 20); // 设置布局的边距

	// 将网格布局添加到对话框
	dialog.setLayout(gridLayout);


	// 显示对话框
	if (dialog.exec() == QDialog::Accepted) {
		//xInterval = xInput.text().toDouble();
		//yInterval = yInput.text().toDouble();

		//// 计算新的X轴和Y轴范围
		//double xMin = 0;
		//double xMax = xMin + xInterval; // 以xInterval为间隔计算xMax
		//double yMin = 0;
		//double yMax = yMin + yInterval; // 以yInterval为间隔计算yMax

		//// 更新图表的X轴和Y轴范围
		//plot->setAxisScale(QwtPlot::xBottom, xMin, xMax);
		//plot->setAxisScale(QwtPlot::yLeft, yMin, yMax);
		//plot->replot(); // 重绘图表以应用新的间隔
	}
}

QColor colorFromName(const QString &name) {
	// 使用Qt的qHash函数生成一个基于字符串的哈希值
	quint32 hashValue = qHash(name);

	// 使用哈希值来生成颜色
	int h = (hashValue % 360); // 色相值在0到359之间
	int s = 200 + (hashValue % 55); // 饱和度在200到255之间
	int v = 150 + (hashValue % 105); // 明度在150到255之间

	return QColor::fromHsv(h, s, v);
}



void ChartManager::addCurve(const QString &curveName) {
	QwtPlotCurve *curve = new QwtPlotCurve(curveName);
	curve->setTitle(curveName); // 设置曲线的标题，这将在图例中显示
		// 设置曲线标题的字体大小
	QwtText title(curveName);
	QFont font;
	font.setPointSize(12); // 设置字体大小为12点
	title.setFont(font);
	curve->setTitle(title);


	QColor color = colorFromName(curveName);
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
			//QColor color = QColor::fromHsv((curves.size() * 30) % 360, 255, 180); // 示例颜色
			addCurve(curveName); // 假设这个方法已经实现

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