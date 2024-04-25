#include "ChartManger.h"

#pragma execution_character_set("utf-8")

ChartManager::ChartManager(QObject *parent, QWidget *parentWidget, const QStringList &curveNames,
	ConfigLoader* configLoader, ChartUpdaterThread* updaterThread)
	: QObject(parent), m_widget(parentWidget), curveNames(curveNames),
	m_configLoader(configLoader), updaterThread(updaterThread)
{
	plot = new QwtPlot(m_widget);
	//plot->setTitle("实时趋势图");
	plot->setCanvasBackground(Qt::white);

	// 启用图例
	plot->insertLegend(new QwtLegend(), QwtPlot::TopLegend);

	// 设置X轴和Y轴的标题
	plot->setAxisTitle(QwtPlot::xBottom,"");
	plot->setAxisTitle(QwtPlot::yLeft,"");

	QString selectedParentNames = m_configLoader->getSelectedParentNames();
	if (!selectedParentNames.isEmpty())
	{
		QVariantMap settingDefaults = m_configLoader->getSettingDefaultValue(selectedParentNames);
		if (settingDefaults.contains("yAxisRange"))
		{
			QVariantList yAxisRange = settingDefaults["yAxisRange"].toList();
			if (yAxisRange.size() == 2) {
				double minY = yAxisRange[0].toDouble();
				double maxY = yAxisRange[1].toDouble();
				plot->setAxisScale(QwtPlot::yLeft, minY, maxY);
			}
		}
		plot->replot();
	}
	// 设置X轴和Y轴的初始范围
	plot->setAxisScale(QwtPlot::xBottom, 0, 50); // 设置X轴的范围为0-50
	//plot->setAxisScale(QwtPlot::yLeft, 0, 900); // 设置Y轴的范围为0-900


	plot->setCanvasBackground(QColor(14,22,55));

	//// 设置默认的x轴和y轴间隔
	//plot->setAxisScale(QwtPlot::xBottom, 0, 100, xInterval); // 使用xInterval初始化x轴
	//plot->setAxisScale(QwtPlot::yLeft, 0, 100, yInterval); // 使用yInterval初始化y轴

	// 创建并配置网格
	QwtPlotGrid *grid = new QwtPlotGrid();
	grid->attach(plot); // 将网格附加到图表
	grid->setVisible(false); // 隐藏网格线


	//updaterThread = new ChartUpdaterThread(this, curveNames);

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

	connect(m_configLoader, &ConfigLoader::yAxisRangeChanged, this, &ChartManager::updateYAxisRange);
	connect(m_configLoader, &ConfigLoader::warningValueChanged, this, &ChartManager::updateWarningValue);
	connect(m_configLoader, &ConfigLoader::alarmValueChanged, this, &ChartManager::updateAlarmValue);
	installEventFilters();

	//plot->replot(); // 重绘图表以应用新的轴间隔

}



ChartManager::~ChartManager() {
	//if (updaterThread) {
	//	updaterThread->stopRunning();
	//	updaterThread->wait();
	//	delete updaterThread;
	//	updaterThread = nullptr;
	//}

}


void ChartManager::start() {
	if (updaterThread && !updaterThread->isRunning()) {
		updaterThread->start();
	}
}

QWidget* ChartManager::getWidget() {
	return m_widget;
}


double ChartManager::adjustXValue(double originalX) {
	// 四舍五入到最近的0.1
	double adjustedX = std::round(originalX * 10) / 10.0;
	return adjustedX;
}

void ChartManager::onChartUpdate(const QString &curveName, int x, qreal y) {
	if (x >= 3000) {
		// 清除所有曲线的数据
		for (auto &curve : curves) {
			xDataMap[curve->title().text()].clear();
			yDataMap[curve->title().text()].clear();
			curve->setSamples(xDataMap[curve->title().text()], yDataMap[curve->title().text()]);
		}
		// 重置x轴范围
		plot->setAxisScale(QwtPlot::xBottom, 0, 50);
		plot->replot();
		return; // 早期返回，避免在清除后添加数据点
	}

	// 调整x值
	x = adjustXValue(x);

	// 原有的更新曲线数据的逻辑
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

	// 原有的逻辑，用于根据新的数据点更新x轴范围
	if (!isViewingHistory) {
		xInterval = 50; // 这里设置x轴的间隔值，根据实际情况调整
		int xMaxCurrent = plot->axisScaleDiv(QwtPlot::xBottom).upperBound(); // 获取当前x轴的最大值
		if (x >= xMaxCurrent) {
			int xMinNew = ((x / xInterval) * xInterval);
			int xMaxNew = xMinNew + xInterval;
			plot->setAxisScale(QwtPlot::xBottom, xMinNew, xMaxNew);
		}
	}

	plot->replot(); // 重绘图表
}

void ChartManager::onIntervalPBClicked() {
	QDialog dialog(m_widget); // 使用当前widget作为父窗口
	dialog.setWindowTitle("参数设置");
	dialog.resize(550, 800);
	dialog.setFixedSize(dialog.size());

	QGridLayout *gridLayout = new QGridLayout();


	// 通过ConfigLoader获取父类名字
	QStringList settingLabels = m_configLoader->getParentCategoryNames();

	//QStringList settingLabels = {
	//	"A/B面整体宽度",
	//	"A/B面电浆宽度",
	//	"A/B面左侧陶瓷宽度",
	//	"A/B面右侧陶瓷宽度",
	//	"A/B面对齐度"
	//};

	// 对于每个设置项，我们需要创建不同的控件
	for (int i = 0; i < settingLabels.size(); ++i) {
		int row = i * 4; // 每个设置占用4行
		QString settingName = settingLabels[i];

		// 从配置加载器获取默认值
		QVariantMap settingDefaults = m_configLoader->getSettingDefaultValue(settingName);
		// 设置标签
		QLabel* settingLabel = new QLabel(settingLabels[i], &dialog);
		settingLabel->setStyleSheet("QLabel { font-weight: bold; }");
		gridLayout->addWidget(settingLabel, row, 0, 1, 2);

		// 趋势图y轴显示区域设置
		QHBoxLayout* rangeLayout = new QHBoxLayout;
		QLabel* rangeLabel = new QLabel("设置趋势图y轴显示区域（毫米）：", &dialog);
		QLineEdit* rangeInput1 = new QLineEdit(&dialog);
		rangeInput1->setValidator(new QDoubleValidator(0, 10000, 2, rangeInput1));
		rangeInput1->setReadOnly(true); // 设置为只读
		QLabel *separator = new QLabel(" --- ", &dialog);
		QLineEdit* rangeInput2 = new QLineEdit(&dialog);
		rangeInput2->setValidator(new QDoubleValidator(0, 10000, 2, rangeInput2));
		rangeInput2->setReadOnly(true); // 设置为只读
		QVariantList yAxisRange = settingDefaults["yAxisRange"].toList();
		if (!yAxisRange.isEmpty()) {
			rangeInput1->setText(QString::number(yAxisRange[0].toDouble(), 'f', 2));
			rangeInput2->setText(QString::number(yAxisRange[1].toDouble(), 'f', 2));
		}

		rangeLayout->addWidget(rangeLabel);
		rangeLayout->addWidget(rangeInput1);
		rangeLayout->addWidget(separator);
		rangeLayout->addWidget(rangeInput2);
		gridLayout->addLayout(rangeLayout, row + 1, 0, 1, 2);

		// 预警值设置
		QHBoxLayout* warningLayout = new QHBoxLayout;
		QLabel* warningLabel = new QLabel(QString("设置%1预警值（毫米）：").arg(settingLabels[i]), &dialog);
		QLabel* greaterWarningLabel = new QLabel("大于", &dialog);
		QLineEdit* greaterWarningInput = new QLineEdit(&dialog);
		greaterWarningInput->setValidator(new QDoubleValidator(0, 10000, 2, greaterWarningInput));
		greaterWarningInput->setReadOnly(true); // 设置为只读
		QLabel* lessWarningLabel = new QLabel("或小于", &dialog);
		QLineEdit* lessWarningInput = new QLineEdit(&dialog);
		lessWarningInput->setValidator(new QDoubleValidator(0, 10000, 2, lessWarningInput));
		lessWarningInput->setReadOnly(true); // 设置为只读
		QVariantList warningValue = settingDefaults["warningValue"].toList();
		if (!warningValue.isEmpty()) {
			greaterWarningInput->setText(QString::number(warningValue[0].toDouble(), 'f', 2));
			lessWarningInput->setText(QString::number(warningValue[1].toDouble(), 'f', 2));
		}

		warningLayout->addWidget(warningLabel);
		warningLayout->addWidget(greaterWarningLabel);
		warningLayout->addWidget(greaterWarningInput);
		warningLayout->addWidget(lessWarningLabel);
		warningLayout->addWidget(lessWarningInput);
		gridLayout->addLayout(warningLayout, row + 2, 0, 1, 2);

		// 告警值设置
		QHBoxLayout* alarmLayout = new QHBoxLayout;
		QLabel* alarmLabel = new QLabel(QString("设置%1告警值（毫米）：").arg(settingLabels[i]), &dialog);
		QLabel* greaterAlarmLabel = new QLabel("大于", &dialog);
		QLineEdit* greaterAlarmInput = new QLineEdit(&dialog);
		greaterAlarmInput->setValidator(new QDoubleValidator(0, 10000, 2, greaterAlarmInput));
		greaterAlarmInput->setReadOnly(true); // 设置为只读
		QLabel* lessAlarmLabel = new QLabel("或小于", &dialog);
		QLineEdit* lessAlarmInput = new QLineEdit(&dialog);
		lessAlarmInput->setValidator(new QDoubleValidator(0, 10000, 2, lessAlarmInput));
		lessAlarmInput->setReadOnly(true); // 设置为只读
		QVariantList alarmValue = settingDefaults["alarmValue"].toList();
		if (!alarmValue.isEmpty()) {
			greaterAlarmInput->setText(QString::number(alarmValue[0].toDouble(), 'f', 2));
			lessAlarmInput->setText(QString::number(alarmValue[1].toDouble(), 'f', 2)); // 更正此处
		}
		alarmLayout->addWidget(alarmLabel);
		alarmLayout->addWidget(greaterAlarmLabel);
		alarmLayout->addWidget(greaterAlarmInput);
		alarmLayout->addWidget(lessAlarmLabel);
		alarmLayout->addWidget(lessAlarmInput);
		gridLayout->addLayout(alarmLayout, row + 3, 0, 1, 2);
	}


	// 设置网格布局的间距和边距
	gridLayout->setHorizontalSpacing(20);
	gridLayout->setVerticalSpacing(10);
	gridLayout->setContentsMargins(20, 20, 20, 20);

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
	gridLayout->addLayout(buttonLayout, settingLabels.size() * 4, 0, 1, 2); // 调整行位置以适应新的布局

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
	if (watched == plot->canvas()) {
		switch (event->type()) {
		case QEvent::MouseButtonPress: {
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			if (mouseEvent->button() == Qt::LeftButton) {
				dragStartPosition = mouseEvent->pos();
				xMinCurrent = plot->axisScaleDiv(QwtPlot::xBottom).lowerBound();
				xMaxCurrent = plot->axisScaleDiv(QwtPlot::xBottom).upperBound();
				isDragging = true;

				resetCurvesOpacity();
				plot->replot(); // 重绘图表以应用更改
				//isViewingHistory = true; // 开始查看历史
				return true;
			}
			else if (mouseEvent->button() == Qt::RightButton) {
				// 当用户右键点击时，退出历史查看模式
				isViewingHistory = false;
			}
			break;
		}
		case QEvent::MouseMove: {
			if (isDragging) {
				QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
				int dx = mouseEvent->pos().x() - dragStartPosition.x();
				isViewingHistory = true;
				double shift = (xMaxCurrent - xMinCurrent) * dx / plot->canvas()->width();
				plot->setAxisScale(QwtPlot::xBottom, xMinCurrent - shift, xMaxCurrent - shift);
				plot->replot();
				return true;
			}
			break;
		}
		case QEvent::MouseButtonRelease: {
			if (isDragging) {
				isDragging = false;
				//isViewingHistory = false; // 结束查看历史
				return true;
			}
			break;
		}
		default:
			break;
		}
	}
	return QObject::eventFilter(watched, event);
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

			// 添加曲线名称到curveNames列表，用于其他同步操作
			curveNames.append(curveName);
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
		curveNames.removeAll(curveName); // 从曲线名称列表中移除
	}

	plot->replot(); // 重绘图表
}



void ChartManager::updateYAxisRange(const QVariantList &yAxisRange) {
	if (yAxisRange.size() == 2) {
		double minY = yAxisRange[0].toDouble();
		double maxY = yAxisRange[1].toDouble();
		plot->setAxisScale(QwtPlot::yLeft, minY, maxY);
		plot->replot();
	}
}

void ChartManager::updateWarningValue(const QVariantList &warningValue) {
	if (warningValue.size() == 2) {
		m_warningValueLower = warningValue[0].toDouble();
		m_warningValueUpper = warningValue[1].toDouble();
	}
}

void ChartManager::updateAlarmValue(const QVariantList &alarmValue) {
	if (alarmValue.size() == 2) {
		m_alarmValueLower = alarmValue[0].toDouble();
		m_alarmValueUpper = alarmValue[1].toDouble();
	}
}