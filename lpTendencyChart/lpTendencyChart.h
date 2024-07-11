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
	//QwtPlot* getPlot() const;
	//QTableWidget* getTable() const;

public slots:
	/*
	cureName:工位名称;
	x:米数;
	y:对齐度/宽度/居中度	
	*/
	void updateDataScope(const QString &curveName, double x, double y);//接收数据更新数据表格
	void updateDataChart(const QString &curveName, double x, double y);//接收数据更新趋势图
	void handleIntervalPBClicked();//参数设置
	void toggleTableVisibility();//趋势勾选指标控制是否隐藏
	void AlignPBClicked();//对齐度设置


private:
	Ui_lpTendencyClass *ui{ nullptr };
	ChartManager *chartManager{ nullptr };//趋势图管理
	ConfigLoader *configLoader{ nullptr };//配置文件加载
	ChartUpdaterThread *chartUpdaterThread;//数据更新工作线程
	DataScope *dataScope{nullptr};//数据表格

};
