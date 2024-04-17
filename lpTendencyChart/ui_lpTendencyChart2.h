/********************************************************************************
** Form generated from reading UI file 'ChartDemo.ui'
**
** Created by: Qt User Interface Compiler version 5.9.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CHARTDEMO_H
#define UI_CHARTDEMO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChartDemoClass
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QTableWidget *tableWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *Interval_PB;
    QWidget *Chartwidget;

    void setupUi(QWidget *ChartDemoClass)
    {
        if (ChartDemoClass->objectName().isEmpty())
            ChartDemoClass->setObjectName(QStringLiteral("ChartDemoClass"));
        ChartDemoClass->resize(1058, 653);
        gridLayout = new QGridLayout(ChartDemoClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        tableWidget = new QTableWidget(ChartDemoClass);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setMinimumSize(QSize(230, 0));
        tableWidget->setMaximumSize(QSize(300, 16777215));

        horizontalLayout_2->addWidget(tableWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        Interval_PB = new QPushButton(ChartDemoClass);
        Interval_PB->setObjectName(QStringLiteral("Interval_PB"));

        horizontalLayout->addWidget(Interval_PB);


        verticalLayout->addLayout(horizontalLayout);

        Chartwidget = new QWidget(ChartDemoClass);
        Chartwidget->setObjectName(QStringLiteral("Chartwidget"));
        Chartwidget->setMinimumSize(QSize(800, 600));
        Chartwidget->setSizeIncrement(QSize(8, 0));

        verticalLayout->addWidget(Chartwidget);


        horizontalLayout_2->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);


        retranslateUi(ChartDemoClass);

        QMetaObject::connectSlotsByName(ChartDemoClass);
    } // setupUi

    void retranslateUi(QWidget *ChartDemoClass)
    {
        ChartDemoClass->setWindowTitle(QApplication::translate("ChartDemoClass", "ChartDemo", Q_NULLPTR));
        Interval_PB->setText(QApplication::translate("ChartDemoClass", "\345\217\202\346\225\260\350\256\276\347\275\256", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ChartDemoClass: public Ui_ChartDemoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTDEMO_H
