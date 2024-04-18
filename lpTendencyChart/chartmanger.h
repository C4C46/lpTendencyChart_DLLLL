#pragma once

#include <QObject>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_div.h>
#include <QVBoxLayout>
#include <QTime>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleValidator>
#include <QStringList>
#include <QSplitter> 
#include <qwt_plot_magnifier.h>
#include <qwt_text.h>
#include <qwt_legend.h>
#include <qwt_legend_label.h>
#include <QMouseEvent>
#include "ConfigLoader.h"
#include "ChartUpdaterThread.h"
class ChartManager : public QObject {
    Q_OBJECT

public:
    ChartManager(QObject *parent, QWidget*parentWidget, const QStringList &curveNames);
	~ChartManager();
    void start();
	QWidget* getWidget();

	QwtPlot* GetPlot() const { return plot;}
	QTableWidget* GetTable() const { return table;}


protected:
	bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
	void onChartUpdate(const QString &curveName, int x, qreal y);
	void onIntervalPBClicked();
	void addCurve(const QString &curveName, const QColor &color); 
	void onLegendClicked(const QVariant &itemInfo, int index);
	void onCurveDisplayChanged(const QString &curveName, bool display); // 添加这个槽函数


private:
	QWidget *m_widget; 
	QwtPlot *plot; 
	ChartUpdaterThread *updaterThread;
	QVector<QwtPlotCurve *> curves; // 支持多条曲线
	QMap<QString, QVector<double>> xDataMap, yDataMap; // 存储每条曲线的数据
	QStringList curveNames;
	double xInterval = 10; // 默认x间隔值
	double yInterval = 10; // 默认y间隔值
	QTableWidget *table; // 添加表格成员变量

	bool autoScrollEnabled = true; // 默认启用自动滚动

	void resetCurvesOpacity(); // 添加成员变量存储曲线名称
	void installEventFilters();//恢复所有曲线显示
};

