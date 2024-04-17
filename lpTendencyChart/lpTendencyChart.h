#pragma once

#include "lptendencychart_global.h"
#include <QWidget>
#include "ChartManger.h"
#include "ConfigLoader.h"

class Ui_ChartDemoClass;
class ChartManger;
class ConfigLoader;

class LPTENDENCYCHART_EXPORT ChartDemo : public QWidget
{
	Q_OBJECT

public:
	ChartDemo(QWidget *parent = nullptr);
	~ChartDemo();

	void init();


public slots:
	void handleIntervalPBClicked();
	void toggleTableVisibility();

private:
	Ui_ChartDemoClass *ui{ nullptr };
	ChartManager *chartManager{ nullptr };
	ConfigLoader *configLoader{ nullptr };

};
