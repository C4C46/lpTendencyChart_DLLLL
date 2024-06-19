#pragma once

#include "lptendencychart_global.h"
#include <QWidget>
#include "ChartManger.h"
#include "ConfigLoader.h"
#include "DataScope.h"


struct LithiumAllRegionInfo_Tag
{

	//按照每米进行整合
	int msgId = -1;//消息ID

	//电浆
	QMap<qint64, QMap<QString, double>> firstcoorNameEregioninfowidth;//<米数, <通道, 通道宽度>>
	QMap<qint64, QMap<QString, double>> secondcoorNameEregioninfowidth;

	//陶瓷
	QMap<qint64, QMap<QString, double>> firstcoorNameceramicinfowidth;
	QMap<qint64, QMap<QString, double>> secondcoorNameceramicinfowidth;

	//极耳
	QMap<qint64, QMap<QString, double>> firstcoorNameelectrodetabinfowidth;
	QMap<qint64, QMap<QString, double>> secondcoorNameelectrodetabinfowidth;


	QMap<qint64, QMap<QString, double>> firstcoorNamecalculatecenterinfowidth;//<米数,居中度>
	QMap<qint64, QMap<QString, double>> secondcoorNamecalculatecenterinfowidth;

	QMap<qint64, QMap<QString, double>>contactNameAndAlignment;

};


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



	void processLithiumAllRegionInfo(const LithiumAllRegionInfo_Tag & info);

private:
	Ui_lpTendencyClass *ui{ nullptr };
	ChartManager *chartManager{ nullptr };
	ConfigLoader *configLoader{ nullptr };
	ChartUpdaterThread *chartUpdaterThread;
	DataScope *dataScope{nullptr};

};
