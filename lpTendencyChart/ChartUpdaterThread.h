#pragma once
#include <QThread>
#include <QObject>
#include <QMutex>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QStringList>
#include <QTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QWaitCondition>
class ChartUpdaterThread : public QThread{
	Q_OBJECT

public:
	 ChartUpdaterThread(QObject *parent, const QStringList &curveNames);
	~ChartUpdaterThread();
	void run() override;//处理数据信息
	void updateCurveNames(const QStringList &newCurveNames); // 增加新曲线

signals:
	void updateChart(const QString &curveName, double x, double y);//发送新的数据信息（工位名称，米数，对齐度/宽度/居中度）

public slots:
	void stopRunning();
	void acceptConnection();
	void readData();//读取解析加载接收到的数据

private:
	QStringList curveNames; // 存储曲线名称
	QMutex mutex; // 用于线程安全地更新曲线名称列表
	bool running = true;
	QTcpServer *tcpServer;
	QTcpSocket *clientSocket = nullptr;
	QWaitCondition cond;//唤醒线程
	QList<QJsonObject> dataBuffer; // 缓存接收到的数据
};

