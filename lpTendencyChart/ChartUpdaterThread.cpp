#include "ChartUpdaterThread.h"


ChartUpdaterThread::ChartUpdaterThread(QObject *parent, const QStringList &curveNames)
	: QThread(parent), curveNames(curveNames)
{
	qsrand(QTime::currentTime().msec());

 	// 设置TCP服务器
	tcpServer = new QTcpServer(this); // 确保在使用tcpServer之前实例化它
	if (!tcpServer->listen(QHostAddress::Any, 12345)) {
		qDebug() << "Unable to start the server: " << tcpServer->errorString();
	}
	else {
		connect(tcpServer, &QTcpServer::newConnection, this, &ChartUpdaterThread::acceptConnection);
	}
}
ChartUpdaterThread::~ChartUpdaterThread() {
	stopRunning();
	wait(); // 等待线程安全地结束

	if (clientSocket) {
		clientSocket->disconnect();
		clientSocket->close();
		delete clientSocket;
		clientSocket = nullptr;
	}

	if (tcpServer) {
		tcpServer->close();
		delete tcpServer;
		tcpServer = nullptr;
	}
}

void ChartUpdaterThread::run() {
	qDebug() << "Running ChartUpdaterThread in thread:" << QThread::currentThreadId();
	running = true;
	QMap<double, QJsonObject> sortedData; // 使用 QMap 来自动排序数据
	qint64 lastProcessTime = QDateTime::currentMSecsSinceEpoch();

	while (running) {
		QMutexLocker locker(&mutex);
		if (dataBuffer.isEmpty()) {
			cond.wait(&mutex, 1000); // 等待数据或超时
		}

		// 将数据从缓存中移动到排序容器中
		while (!dataBuffer.isEmpty()) {
			QJsonObject json = dataBuffer.takeFirst();
			double x = json["x"].toDouble();
			qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
			if (sortedData.isEmpty() || x >= sortedData.lastKey() || (currentTime - lastProcessTime <= 2000 && x < sortedData.lastKey())) {
				sortedData.insert(x, json);
				QJsonDocument doc(json);
				QString strJson(doc.toJson(QJsonDocument::Compact)); 
			}
		}
		locker.unlock();

		// 处理排序后的数据
		auto it = sortedData.begin();
		while (it != sortedData.end()) {
			QString curveName = it.value()["curveName"].toString();
			double x = it.key();
			double y = it.value()["y"].toDouble();
			emit updateChart(curveName, x, y);
			it = sortedData.erase(it); // 移除已处理的数据
		}

		lastProcessTime = QDateTime::currentMSecsSinceEpoch();
	}
}

void ChartUpdaterThread::stopRunning() {

	QMutexLocker locker(&mutex);
	running = false;
	cond.wakeOne(); // 唤醒线程以便能够退出
}

void ChartUpdaterThread::updateCurveNames(const QStringList &newCurveNames) {
	qDebug() << "Running updateCurveNames in thread:" << QThread::currentThreadId();
	QMutexLocker locker(&mutex); // 确保线程安全
	curveNames = newCurveNames;
}


void ChartUpdaterThread::acceptConnection() {
	qDebug() << "Running acceptConnection in thread:" << QThread::currentThreadId();
	clientSocket = tcpServer->nextPendingConnection();
	connect(clientSocket, &QTcpSocket::readyRead, this, &ChartUpdaterThread::readData);
}

void ChartUpdaterThread::readData() {
	if (!clientSocket || !clientSocket->isValid()) return;  // 检查socket是否有效

	QByteArray data = clientSocket->readAll();
	if (data.isEmpty()) return;  // 检查是否有数据读取

	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (doc.isNull()) return;  // 检查JSON数据是否有效

	QJsonObject json = doc.object();

	QMutexLocker locker(&mutex);
	dataBuffer.append(json);  // 将数据添加到缓存
	cond.wakeOne();  // 唤醒处理线程
}