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
	tcpServer->close();
}

void ChartUpdaterThread::run() {
	running = true;
	while (running) {
		QThread::msleep(100); // 调整为合适的延迟
	}
}

void ChartUpdaterThread::stopRunning() {

	running = false;
}

void ChartUpdaterThread::updateCurveNames(const QStringList &newCurveNames) {
	QMutexLocker locker(&mutex); // 确保线程安全
	curveNames = newCurveNames;
}


void ChartUpdaterThread::acceptConnection() {
	clientSocket = tcpServer->nextPendingConnection();
	connect(clientSocket, &QTcpSocket::readyRead, this, &ChartUpdaterThread::readData);
}

//void ChartUpdaterThread::readData() {
//	QDataStream in(clientSocket);
//	in.setVersion(QDataStream::Qt_DefaultCompiledVersion);
//
//	QString curveName;
//	double x, y;
//
//	while (!in.atEnd()) {
//		in >> curveName >> x >> y;
//		emit updateChart(curveName, x, y);
//	}
//}

void ChartUpdaterThread::readData() {
    if (!clientSocket) return;

    QByteArray data = clientSocket->readAll(); // 读取所有数据
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject json = doc.object();

    QString curveName = json["curveName"].toString(); // 获取曲线名称
    double x = json["x"].toDouble();
    double y = json["y"].toDouble();

    // 根据曲线名称和数据触发信号
    emit updateChart(curveName, x, y);
}