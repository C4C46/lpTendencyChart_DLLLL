#include "lpDataScopeThread.h"

lpDataScopeThread::lpDataScopeThread()
{

}



lpDataScopeThread::~lpDataScopeThread()
{

	m_dataCache.clear();
}

void lpDataScopeThread::stopThread()
{
	QMutexLocker locker(&m_mutex);
	m_runFlag = false;
}

void lpDataScopeThread::process()
{
	while (m_runFlag)
	{
		if (m_processFlag)
		{
			QMutexLocker locker(&m_mutex);
			for (auto &curveName : m_dataCache.keys()) {
				for (auto &data : m_dataCache[curveName]) {
					double x = data.first;
					double y = data.second.first;
					QVariantList warningValue = data.second.second[0].toList();
					QVariantList alarmValue = data.second.second[1].toList();
					emit sgSendData(curveName, x, y, warningValue, alarmValue);

				}

			}
			m_dataCache.clear(); // 清空缓存
			m_processFlag = false;
		}
	}
}






void lpDataScopeThread::onDataCache(QMap<QString, QList<QPair<double, QPair<double, QVariantList>>>> dataCache)
{

	if (m_processFlag == true)
	{
		return;
	}
	m_dataCache = dataCache;
	m_processFlag = true;
}
