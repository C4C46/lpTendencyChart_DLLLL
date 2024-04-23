#pragma once

#include <QObject>
#include <QTableWidget>
#include <QTreeWidget>
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
	explicit ConfigLoader(QTreeWidget *treeWidget, QObject *parent = nullptr);
	QVariantMap getSettingDefaultValue(const QString & settingName);
	void loadConfig(const QString &filePath);
	void saveConfig(const QString &filePath); // 添加保存配置的声明
	QStringList getCurveNames() const; // 新增方法

	QStringList getParentCategoryNames() const;
signals:
	void curveDisplayChanged(const QString &curveName, bool display);

	void yAxisRangeChanged(const QVariantList &yAxisRange);
	void warningValueChanged(const QVariantList &warningValue);
	void alarmValueChanged(const QVariantList &alarmValue);
private:
	QTreeWidget *m_treeWidget;
	QStringList curveNames; // 存储曲线名称
	QButtonGroup *m_buttonGroup; // 管理单选按钮的按钮组

	QJsonDocument configDoc; // 添加configDoc成员变量


};

