#include "lpTendencyChart.h"
#include "ui_lpTendencyChart.h"

#pragma execution_character_set("utf-8")

lpTendencyChart::lpTendencyChart(QWidget *parent)
	: QWidget(parent), ui(new Ui_lpTendencyClass)
{
	ui->setupUi(this);
	init();

}

lpTendencyChart::~lpTendencyChart()
{
	

	if (chartManager)
		delete chartManager;
	if (configLoader)
	{
		configLoader->saveConfig("Config/Event.json");
		delete configLoader;
	}
		
	if (dataScope)
	{
		dataScope->saveSettingsToFile();
		delete dataScope;
	}
		

}

void lpTendencyChart::init()
{
	configLoader = new ConfigLoader(ui->treeWidget, this);
	configLoader->loadConfig("Config/Event.json");


	QStringList curveNames = configLoader->getCurveNames(); // 获取曲线名称
	QStringList allCurveNames = configLoader->getAllCurveNames(); // 获取所有曲线名称
	QStringList ChooseNames = configLoader->getCurveNames();
			// 创建一个线程来处理所有数据
	chartUpdaterThread = new ChartUpdaterThread(this, allCurveNames);

	chartManager = new ChartManager(this, ui->Chartwidget, curveNames, 
		configLoader, chartUpdaterThread);
	/*chartManager->start();*/

	// 创建数据显示区域
	if (!dataScope) {
		dataScope = new DataScope(ui->tableWidget, this);
	}
	dataScope->setColumnNames(ChooseNames);
	dataScope->loadSettingsFromFile();

	ui->Toggle_PB->setText("趋势指标勾选隐藏");
	connect(configLoader, &ConfigLoader::curveDisplayChanged, chartManager, &ChartManager::onCurveDisplayChanged);
	connect(chartUpdaterThread, &ChartUpdaterThread::updateChart, this, &lpTendencyChart::updateData);
	connect(ui->Interval_PB, &QPushButton::clicked, this, &lpTendencyChart::handleIntervalPBClicked);
	connect(ui->Toggle_PB, &QPushButton::clicked, this, &lpTendencyChart::toggleTableVisibility);
	connect(ui->Align_PB, &QPushButton::clicked, this, &lpTendencyChart::AlianPBClicked);
	connect(configLoader, &ConfigLoader::curveNamesChanged, dataScope, &DataScope::setColumnNames);
}



void lpTendencyChart::handleIntervalPBClicked() {
	if (chartManager) {
		chartManager->onIntervalPBClicked();//用于参数设置对话框
	}
}


void lpTendencyChart::toggleTableVisibility()
{
	ui->treeWidget->setVisible(!ui->treeWidget->isVisible());
	ui->Toggle_PB->setText(ui->treeWidget->isVisible() ? "趋势指标勾选隐藏" : "趋势指标勾选显示");
}

void lpTendencyChart::AlianPBClicked()
{
	if (chartManager)
	{
		chartManager->AlignPBClicked();//用于显示对齐度设置对话框
	}
}




void lpTendencyChart::processLithiumAllRegionInfo(const LithiumAllRegionInfo_Tag &info) {
	// 定义一个lambda函数来处理QMap数据
	auto processMap = [this](const QMap<qint64, QMap<QString, double>> &map) {
		for (auto it = map.begin(); it != map.end(); ++it) {
			qint64 x = it.key();  // 米数，对应X值
			const QMap<QString, double> &innerMap = it.value();
			for (auto innerIt = innerMap.begin(); innerIt != innerMap.end(); ++innerIt) {
				QString curveName = innerIt.key();  // 通道名称，作为curveName
				double y = innerIt.value();  // 通道宽度，对应Y值
				// 调用updateData方法更新数据
				updateData(curveName, x, y);
			}
		}
	};

	// 处理每个区域的数据
	processMap(info.firstcoorNameEregioninfowidth);
	processMap(info.secondcoorNameEregioninfowidth);
	processMap(info.firstcoorNameEregioninfowidth);
	processMap(info.secondcoorNameEregioninfowidth);
	processMap(info.firstcoorNameEregioninfowidth);
	processMap(info.secondcoorNameEregioninfowidth);
	processMap(info.firstcoorNamecalculatecenterinfowidth);
	processMap(info.secondcoorNamecalculatecenterinfowidth);
	processMap(info.contactNameAndAlignment);
}



void lpTendencyChart::updateData(const QString &curveName, double x, double y) {
	//qDebug() << "Updating data for" << curveName << "with X:" << x << "Y:" << y;

	// 获取当前曲线所属的父类名称
	QString parentName = configLoader->getParentNameForCurve(curveName);
	// 获取该父类的设置默认值
	QVariantMap settingDefaults = configLoader->getSettingDefaultValue(parentName);

	QVariantList warningValue, alarmValue;
	if (settingDefaults.contains("warningValue")) {
		warningValue = settingDefaults["warningValue"].toList();
	}
	if (settingDefaults.contains("alarmValue")) {
		alarmValue = settingDefaults["alarmValue"].toList();
	}

	// 使用已有的addData函数来添加数据到表格，并传递告警值和预警值范围
	dataScope->addData(curveName, x, y, warningValue, alarmValue);


	// 如果曲线被用户选中，则更新图表
	if (configLoader->getCurveNames().contains(curveName)) {
		chartManager->onChartUpdate(curveName, x, y);
	}
}


QwtPlot* lpTendencyChart::getPlot() const {
	return chartManager ? chartManager->GetPlot() : nullptr;
}

QTableWidget* lpTendencyChart::getTable() const {
	return chartManager ? chartManager->GetTable() : nullptr;
}



