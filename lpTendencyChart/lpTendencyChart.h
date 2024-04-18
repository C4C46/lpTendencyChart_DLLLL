#pragma once

#include "lptendencychart_global.h"
#include <QWidget>
#include "ChartManger.h"
#include "ConfigLoader.h"

class Ui_lpTendencyClass;
class ChartManger;
class ConfigLoader;

class LPTENDENCYCHART_EXPORT lpTendencyChart : public QWidget
{
	Q_OBJECT

public:
	lpTendencyChart(QWidget *parent = nullptr);
	~lpTendencyChart();

	void init();
	QwtPlot* getPlot() const;
	QTableWidget* getTable() const;

public slots:
	void handleIntervalPBClicked();
	void toggleTableVisibility();

private:
	Ui_lpTendencyClass *ui{ nullptr };
	ChartManager *chartManager{ nullptr };
	ConfigLoader *configLoader{ nullptr };

};
