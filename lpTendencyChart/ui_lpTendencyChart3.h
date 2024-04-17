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
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ChartDemoClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QTableWidget *tableWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *Interval_PB;
    QWidget *Chartwidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *Toggle_PB;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *ChartDemoClass)
    {
        if (ChartDemoClass->objectName().isEmpty())
            ChartDemoClass->setObjectName(QStringLiteral("ChartDemoClass"));
        ChartDemoClass->resize(1058, 691);
        centralWidget = new QWidget(ChartDemoClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        tableWidget = new QTableWidget(centralWidget);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setMinimumSize(QSize(230, 0));
        tableWidget->setMaximumSize(QSize(300, 16777215));

        horizontalLayout_3->addWidget(tableWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        Interval_PB = new QPushButton(centralWidget);
        Interval_PB->setObjectName(QStringLiteral("Interval_PB"));

        horizontalLayout->addWidget(Interval_PB);


        verticalLayout->addLayout(horizontalLayout);

        Chartwidget = new QWidget(centralWidget);
        Chartwidget->setObjectName(QStringLiteral("Chartwidget"));
        Chartwidget->setMinimumSize(QSize(800, 550));
        Chartwidget->setSizeIncrement(QSize(8, 0));

        verticalLayout->addWidget(Chartwidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        Toggle_PB = new QPushButton(centralWidget);
        Toggle_PB->setObjectName(QStringLiteral("Toggle_PB"));

        horizontalLayout_2->addWidget(Toggle_PB);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        ChartDemoClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(ChartDemoClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1058, 23));
        ChartDemoClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(ChartDemoClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        ChartDemoClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(ChartDemoClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        ChartDemoClass->setStatusBar(statusBar);

        retranslateUi(ChartDemoClass);

        QMetaObject::connectSlotsByName(ChartDemoClass);
    } // setupUi

    void retranslateUi(QMainWindow *ChartDemoClass)
    {
        ChartDemoClass->setWindowTitle(QApplication::translate("ChartDemoClass", "ChartDemo", Q_NULLPTR));
        Interval_PB->setText(QApplication::translate("ChartDemoClass", "\345\217\202\346\225\260\350\256\276\347\275\256", Q_NULLPTR));
        Toggle_PB->setText(QApplication::translate("ChartDemoClass", "\350\266\213\345\212\277", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ChartDemoClass: public Ui_ChartDemoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTDEMO_H
