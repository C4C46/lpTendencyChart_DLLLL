#pragma once
#include <qobject.h>
#include "QMap"
#include <QVariantList>
#include <QMutexLocker>
#include <QMutex>
class lpDataScopeThread :public QObject
{
	Q_OBJECT
public:
	lpDataScopeThread();
	~lpDataScopeThread();

	

signals:
	void sgSendData(QString DataName, double xData, double yData, QVariantList warningValue, QVariantList AlarmingValue);
public slots:
	void onDataCache(QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> dataCache);
	void process();
private:
	QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> m_dataCache;
	bool m_runFlag = true;

	bool m_processFlag = false;

	mutable QMutex m_mutex;
};

