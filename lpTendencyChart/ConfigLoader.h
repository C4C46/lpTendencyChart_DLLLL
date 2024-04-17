#pragma once

#include <QObject>
#include <QTableWidget>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QCoreApplication>

class ConfigLoader :
	public QObject
{
	Q_OBJECT

public:
	explicit ConfigLoader(QTableWidget *tableWidget, QObject *parent = nullptr);
	void loadConfig(const QString &filePath);
	void saveConfig(const QString &filePath); // 添加保存配置的声明
	QStringList getCurveNames() const; // 新增方法
signals:
	void curveDisplayChanged(const QString &curveName, bool display);
private:
	QTableWidget *m_tableWidget;
	QStringList curveNames; // 存储曲线名称
};

