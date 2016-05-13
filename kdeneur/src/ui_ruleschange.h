/********************************************************************************
** Form generated from reading UI file 'ruleschange.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RULESCHANGE_H
#define UI_RULESCHANGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RulesChange
{
public:
    QVBoxLayout *verticalLayout;
    QTableWidget *listWords;
    QHBoxLayout *horizontalLayout;
    QPushButton *cmdAdd;
    QPushButton *cmdEdit;
    QPushButton *cmdRemove;
    QPushButton *cmdSave;
    QPushButton *cmdClose;

    void setupUi(QDialog *RulesChange)
    {
        if (RulesChange->objectName().isEmpty())
            RulesChange->setObjectName(QString::fromUtf8("RulesChange"));
        RulesChange->resize(373, 401);
        verticalLayout = new QVBoxLayout(RulesChange);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        listWords = new QTableWidget(RulesChange);
        if (listWords->columnCount() < 3)
            listWords->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        listWords->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        listWords->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        listWords->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        listWords->setObjectName(QString::fromUtf8("listWords"));

        verticalLayout->addWidget(listWords);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cmdAdd = new QPushButton(RulesChange);
        cmdAdd->setObjectName(QString::fromUtf8("cmdAdd"));

        horizontalLayout->addWidget(cmdAdd);

        cmdEdit = new QPushButton(RulesChange);
        cmdEdit->setObjectName(QString::fromUtf8("cmdEdit"));

        horizontalLayout->addWidget(cmdEdit);

        cmdRemove = new QPushButton(RulesChange);
        cmdRemove->setObjectName(QString::fromUtf8("cmdRemove"));

        horizontalLayout->addWidget(cmdRemove);

        cmdSave = new QPushButton(RulesChange);
        cmdSave->setObjectName(QString::fromUtf8("cmdSave"));

        horizontalLayout->addWidget(cmdSave);

        cmdClose = new QPushButton(RulesChange);
        cmdClose->setObjectName(QString::fromUtf8("cmdClose"));

        horizontalLayout->addWidget(cmdClose);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(RulesChange);

        QMetaObject::connectSlotsByName(RulesChange);
    } // setupUi

    void retranslateUi(QDialog *RulesChange)
    {
        RulesChange->setWindowTitle(QApplication::translate("RulesChange", "Rules change...", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = listWords->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("RulesChange", "Combination of letters", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = listWords->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("RulesChange", "Condition", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = listWords->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("RulesChange", "Insensitive", 0, QApplication::UnicodeUTF8));
        cmdAdd->setText(QApplication::translate("RulesChange", "Add", 0, QApplication::UnicodeUTF8));
        cmdEdit->setText(QApplication::translate("RulesChange", "Edit", 0, QApplication::UnicodeUTF8));
        cmdRemove->setText(QApplication::translate("RulesChange", "Remove", 0, QApplication::UnicodeUTF8));
        cmdSave->setText(QApplication::translate("RulesChange", "Save", 0, QApplication::UnicodeUTF8));
        cmdClose->setText(QApplication::translate("RulesChange", "Close", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RulesChange: public Ui_RulesChange {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RULESCHANGE_H
