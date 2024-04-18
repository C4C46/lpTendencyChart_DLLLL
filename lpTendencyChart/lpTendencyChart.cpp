#include "lpTendencyChart.h"
#include "ui_lpTendencyChart.h"

#pragma execution_character_set("utf-8")

ChartDemo::ChartDemo(QWidget *parent)
	: QWidget(parent), ui(new Ui_ChartDemoClass)
{
	ui->setupUi(this);
	init();

}

ChartDemo::~ChartDemo()
{
	configLoader->saveConfig("Config/Event.json");
	if (chartManager)
		delete chartManager;
	if (configLoader)
		delete configLoader;
}

void ChartDemo::init()
{
	configLoader = new ConfigLoader(ui->tableWidget, this);
	configLoader->loadConfig("Config/Event.json");


	QStringList curveNames = configLoader->getCurveNames(); // 获取曲线名称
	chartManager = new ChartManager(this, ui->Chartwidget, curveNames);
	chartManager->start();
	connect(configLoader, &ConfigLoader::curveDisplayChanged, chartManager, &ChartManager::onCurveDisplayChanged);
	
	ui->Toggle_PB->setText("趋势指标勾选隐藏");
	connect(ui->Interval_PB, &QPushButton::clicked, this, &ChartDemo::handleIntervalPBClicked);
	connect(ui->Toggle_PB, &QPushButton::clicked, this, &ChartDemo::toggleTableVisibility);
}



void ChartDemo::handleIntervalPBClicked() {
	if (chartManager) {
		chartManager->onIntervalPBClicked();//用于显示间隔设置对话框
	}
}


void ChartDemo::toggleTableVisibility()
{
	ui->tableWidget->setVisible(!ui->tableWidget->isVisible());
	ui->Toggle_PB->setText(ui->tableWidget->isVisible() ? "趋势指标勾选隐藏" : "趋势指标勾选显示");
}

QwtPlot* ChartDemo::getPlot() const {
	return chartManager ? chartManager->GetPlot() : nullptr;
}

QTableWidget* ChartDemo::getTable() const {
	return chartManager ? chartManager->GetTable() : nullptr;
}



