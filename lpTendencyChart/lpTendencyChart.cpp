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
	configLoader->saveConfig("Config/Event.json");
	if (chartManager)
		delete chartManager;
	if (configLoader)
		delete configLoader;
}

void lpTendencyChart::init()
{
	configLoader = new ConfigLoader(ui->treeWidget, this);
	configLoader->loadConfig("Config/Event.json");


	QStringList curveNames = configLoader->getCurveNames(); // 获取曲线名称
	chartManager = new ChartManager(this, ui->Chartwidget, curveNames, configLoader);
	chartManager->start();
	connect(configLoader, &ConfigLoader::curveDisplayChanged, chartManager, &ChartManager::onCurveDisplayChanged);
	
	ui->Toggle_PB->setText("趋势指标勾选隐藏");
	connect(ui->Interval_PB, &QPushButton::clicked, this, &lpTendencyChart::handleIntervalPBClicked);
	connect(ui->Toggle_PB, &QPushButton::clicked, this, &lpTendencyChart::toggleTableVisibility);
}



void lpTendencyChart::handleIntervalPBClicked() {
	if (chartManager) {
		chartManager->onIntervalPBClicked();//用于显示间隔设置对话框
	}
}


void lpTendencyChart::toggleTableVisibility()
{
	ui->treeWidget->setVisible(!ui->treeWidget->isVisible());
	ui->Toggle_PB->setText(ui->treeWidget->isVisible() ? "趋势指标勾选隐藏" : "趋势指标勾选显示");
}

QwtPlot* lpTendencyChart::getPlot() const {
	return chartManager ? chartManager->GetPlot() : nullptr;
}

QTableWidget* lpTendencyChart::getTable() const {
	return chartManager ? chartManager->GetTable() : nullptr;
}



