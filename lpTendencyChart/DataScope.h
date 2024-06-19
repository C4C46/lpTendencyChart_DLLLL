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
#include <qDebug>

class DataScope : public QObject {
	Q_OBJECT

public:
	DataScope(QTableWidget* tableWidget, QObject* parent = nullptr);
	void setColumnNames(const QStringList &names);
	void addData(const QString & curveName, double x, double y, const QVariantList & warningValue, const QVariantList & alarmValue);
	
	void saveTableSettings(const QStringList& identifiers);
	void loadTableSettings(const QStringList& identifiers);
	void loadSettingsFromFile();
	void saveSettingsToFile();
protected:
	bool eventFilter(QObject *obj, QEvent *event);



private:
	QTableWidget* data_tableWidget;
	QStringList m_columnNames;

	QMap<QString, QVariant> settingsCache;  // 缓存设置
	QMap<int, QPair<int, QHeaderView::ResizeMode>> columnSettings;
	bool autoScrollEnabled = true;  // 默认启用自动滚动
};


