#pragma once

#include <QObject>
#include <QTableWidget>
#include <QList>
#include <QHeaderView>
#include <QEvent>
#include <QScrollBar>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>
#include <qDebug>
#include <QThread>
#include "lpDataScopeThread.h"
class DataScope : public QObject {
	Q_OBJECT

public:
	DataScope(QTableWidget* tableWidget, QObject* parent = nullptr);
	DataScope::~DataScope();
	void setColumnNames(const QStringList &names);
	void addData(const QString & curveName, double x, double y, const QVariantList & warningValue, const QVariantList & alarmValue);
	
	void saveTableSettings(const QStringList& identifiers);
	void loadTableSettings(const QStringList& identifiers);
	void loadSettingsFromFile();
	void saveSettingsToFile();

	//void batchUpdateTable();

signals:
	void sgDataCache(QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> dataCache);
		

public slots:
	void onSendData(QString DataName, double xData, double yData, QVariantList warningValue, QVariantList AlarmingValue);
protected:
	bool eventFilter(QObject *obj, QEvent *event);



private:
	QTableWidget* data_tableWidget;
	QStringList m_columnNames;

	QMap<QString, QVariant> settingsCache;  // 缓存设置
	QMap<int, QPair<int, QHeaderView::ResizeMode>> columnSettings;
	bool autoScrollEnabled = true;  // 默认启用自动滚动

	int reloadCount = 0; // 记录表格重新加载次数的成员变量
	QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> dataCache; // 数据缓存
	QTimer *updateTimer; // 定时器
	bool hasNewData = false;

	lpDataScopeThread* m_dataScopeThread;
	QThread *m_thread;
};


