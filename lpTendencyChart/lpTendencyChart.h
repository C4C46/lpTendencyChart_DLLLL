#pragma once

#include "lptendencychart_global.h"
#include <QWidget>
#include "ChartManger.h"
#include "ConfigLoader.h"
#include "DataScope.h"

class Ui_lpTendencyClass;
class ChartManger;
class ConfigLoader;
class ChartUpdaterThread;
class DataScope;
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
	void updateData(const QString &curveName, double x, double y);
	void handleIntervalPBClicked();
	void toggleTableVisibility();
	void AlianPBClicked();

private:
	Ui_lpTendencyClass *ui{ nullptr };
	ChartManager *chartManager{ nullptr };
	ConfigLoader *configLoader{ nullptr };
	ChartUpdaterThread *chartUpdaterThread;
	DataScope *dataScope;

};
