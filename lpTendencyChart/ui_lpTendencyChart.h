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
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_lpTendencyClass
{
public:
    QGridLayout *gridLayout_3;
    QSplitter *splitter;
    QWidget *Top_widget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QTreeWidget *treeWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *Align_PB;
    QPushButton *Interval_PB;
    QWidget *Chartwidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *Toggle_PB;
    QWidget *Down_widget;
    QGridLayout *gridLayout_2;
    QTableWidget *tableWidget;

    void setupUi(QWidget *lpTendencyClass)
    {
        if (lpTendencyClass->objectName().isEmpty())
            lpTendencyClass->setObjectName(QStringLiteral("lpTendencyClass"));
        lpTendencyClass->resize(1173, 852);
        gridLayout_3 = new QGridLayout(lpTendencyClass);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        splitter = new QSplitter(lpTendencyClass);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        Top_widget = new QWidget(splitter);
        Top_widget->setObjectName(QStringLiteral("Top_widget"));
        gridLayout = new QGridLayout(Top_widget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        treeWidget = new QTreeWidget(Top_widget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        treeWidget->setHeaderItem(__qtreewidgetitem);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setMinimumSize(QSize(240, 0));
        treeWidget->setMaximumSize(QSize(300, 16777215));

        horizontalLayout_3->addWidget(treeWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        Align_PB = new QPushButton(Top_widget);
        Align_PB->setObjectName(QStringLiteral("Align_PB"));

        horizontalLayout->addWidget(Align_PB);

        Interval_PB = new QPushButton(Top_widget);
        Interval_PB->setObjectName(QStringLiteral("Interval_PB"));

        horizontalLayout->addWidget(Interval_PB);


        verticalLayout->addLayout(horizontalLayout);

        Chartwidget = new QWidget(Top_widget);
        Chartwidget->setObjectName(QStringLiteral("Chartwidget"));
        Chartwidget->setMinimumSize(QSize(0, 0));
        Chartwidget->setSizeIncrement(QSize(8, 0));

        verticalLayout->addWidget(Chartwidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        Toggle_PB = new QPushButton(Top_widget);
        Toggle_PB->setObjectName(QStringLiteral("Toggle_PB"));

        horizontalLayout_2->addWidget(Toggle_PB);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);

        splitter->addWidget(Top_widget);
        Down_widget = new QWidget(splitter);
        Down_widget->setObjectName(QStringLiteral("Down_widget"));
        gridLayout_2 = new QGridLayout(Down_widget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        tableWidget = new QTableWidget(Down_widget);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));

        gridLayout_2->addWidget(tableWidget, 0, 0, 1, 1);

        splitter->addWidget(Down_widget);

        gridLayout_3->addWidget(splitter, 0, 0, 1, 1);


        retranslateUi(lpTendencyClass);

        QMetaObject::connectSlotsByName(lpTendencyClass);
    } // setupUi

    void retranslateUi(QWidget *lpTendencyClass)
    {
        lpTendencyClass->setWindowTitle(QApplication::translate("lpTendencyClass", "ChartDemo", Q_NULLPTR));
        Align_PB->setText(QApplication::translate("lpTendencyClass", "\345\257\271\351\275\220\345\272\246\350\256\276\347\275\256", Q_NULLPTR));
        Interval_PB->setText(QApplication::translate("lpTendencyClass", "\345\217\202\346\225\260\350\256\276\347\275\256", Q_NULLPTR));
        Toggle_PB->setText(QApplication::translate("lpTendencyClass", "\350\266\213\345\212\277\346\214\207\346\240\207\351\232\220\350\227\217", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class lpTendencyClass: public Ui_lpTendencyClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTDEMO_H
