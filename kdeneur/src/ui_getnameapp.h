/********************************************************************************
** Form generated from reading UI file 'getnameapp.ui'
**
** Created: Thu Oct 17 12:29:17 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GETNAMEAPP_H
#define UI_GETNAMEAPP_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_getNameApp
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *txtNameApp;
    QPushButton *cmdGetApp;
    QDialogButtonBox *cmdBox;

    void setupUi(QDialog *getNameApp)
    {
        if (getNameApp->objectName().isEmpty())
            getNameApp->setObjectName(QString::fromUtf8("getNameApp"));
        getNameApp->resize(211, 115);
        verticalLayout = new QVBoxLayout(getNameApp);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(getNameApp);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(0, 20));

        verticalLayout->addWidget(label);

        txtNameApp = new QLineEdit(getNameApp);
        txtNameApp->setObjectName(QString::fromUtf8("txtNameApp"));
        txtNameApp->setMinimumSize(QSize(0, 25));

        verticalLayout->addWidget(txtNameApp);

        cmdGetApp = new QPushButton(getNameApp);
        cmdGetApp->setObjectName(QString::fromUtf8("cmdGetApp"));
        cmdGetApp->setMinimumSize(QSize(0, 25));

        verticalLayout->addWidget(cmdGetApp);

        cmdBox = new QDialogButtonBox(getNameApp);
        cmdBox->setObjectName(QString::fromUtf8("cmdBox"));
        cmdBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(cmdBox);


        retranslateUi(getNameApp);

        QMetaObject::connectSlotsByName(getNameApp);
    } // setupUi

    void retranslateUi(QDialog *getNameApp)
    {
        getNameApp->setWindowTitle(QApplication::translate("getNameApp", "Add application...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("getNameApp", "Application name:", 0, QApplication::UnicodeUTF8));
        cmdGetApp->setText(QApplication::translate("getNameApp", "Get application name...", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class getNameApp: public Ui_getNameApp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GETNAMEAPP_H
