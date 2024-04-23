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
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_lpTendencyClass
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_3;
    QTreeWidget *treeWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *Interval_PB;
    QWidget *Chartwidget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *Toggle_PB;

    void setupUi(QWidget *lpTendencyClass)
    {
        if (lpTendencyClass->objectName().isEmpty())
            lpTendencyClass->setObjectName(QStringLiteral("lpTendencyClass"));
        lpTendencyClass->resize(1048, 648);
        gridLayout = new QGridLayout(lpTendencyClass);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        treeWidget = new QTreeWidget(lpTendencyClass);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        treeWidget->setMinimumSize(QSize(220, 0));
        treeWidget->setMaximumSize(QSize(220, 16777215));

        horizontalLayout_3->addWidget(treeWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        Interval_PB = new QPushButton(lpTendencyClass);
        Interval_PB->setObjectName(QStringLiteral("Interval_PB"));

        horizontalLayout->addWidget(Interval_PB);


        verticalLayout->addLayout(horizontalLayout);

        Chartwidget = new QWidget(lpTendencyClass);
        Chartwidget->setObjectName(QStringLiteral("Chartwidget"));
        Chartwidget->setMinimumSize(QSize(800, 550));
        Chartwidget->setSizeIncrement(QSize(8, 0));

        verticalLayout->addWidget(Chartwidget);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        Toggle_PB = new QPushButton(lpTendencyClass);
        Toggle_PB->setObjectName(QStringLiteral("Toggle_PB"));

        horizontalLayout_2->addWidget(Toggle_PB);


        verticalLayout->addLayout(horizontalLayout_2);


        horizontalLayout_3->addLayout(verticalLayout);


        gridLayout->addLayout(horizontalLayout_3, 0, 0, 1, 1);


        retranslateUi(lpTendencyClass);

        QMetaObject::connectSlotsByName(lpTendencyClass);
    } // setupUi

    void retranslateUi(QWidget *lpTendencyClass)
    {
        lpTendencyClass->setWindowTitle(QApplication::translate("lpTendencyClass", "ChartDemo", Q_NULLPTR));
        Interval_PB->setText(QApplication::translate("lpTendencyClass", "\345\217\202\346\225\260\350\256\276\347\275\256", Q_NULLPTR));
        Toggle_PB->setText(QApplication::translate("lpTendencyClass", "\350\266\213\345\212\277\346\214\207\346\240\207\351\232\220\350\227\217", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class lpTendencyClass: public Ui_lpTendencyClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CHARTDEMO_H
