#pragma once
#include <qobject.h>
#include "QMap"
#include <QVariantList>
#include <QMutexLocker>
#include <QMutex>
#include <QDebug>
class lpDataScopeThread :public QObject
{
	Q_OBJECT
public:
	lpDataScopeThread();
	~lpDataScopeThread();
	void stopThread();

signals:
	void sgSendData(QString DataName, double xData, double yData, QVariantList warningValue, QVariantList AlarmingValue);//发送处理完成的数据内容

public slots:
	void onDataCache(QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> dataCache);//接收数据信息
	void process();//处理数据信息

private:
	QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> m_dataCache;//存储接收到的数据信息
	bool m_runFlag = true;//线程运行标志
	bool m_processFlag = false;//数据处理标志
	mutable QMutex m_mutex;
};

