#pragma once

#include <QObject>
#include <QTableWidget>
#include <QList>
#include <QHeaderView>
#include <QEvent>
#include <QScrollBar>

class DataScope : public QObject {
	Q_OBJECT

public:
	DataScope(QTableWidget* tableWidget, QObject* parent = nullptr);
	void setColumnNames(const QStringList &names);
	void addData(const QString & curveName, double x, double y, const QVariantList & warningValue, const QVariantList & alarmValue);

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	QTableWidget* data_tableWidget;
	QStringList m_columnNames;
	bool autoScrollEnabled = true;  // 默认启用自动滚动
};


