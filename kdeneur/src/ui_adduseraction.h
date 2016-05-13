/********************************************************************************
** Form generated from reading UI file 'adduseraction.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDUSERACTION_H
#define UI_ADDUSERACTION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_addUserAction
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QLineEdit *txtActionName;
    QLabel *label_2;
    QLabel *lblHotKey;
    QLabel *label_3;
    QLineEdit *txtCommandAction;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *cmdSave;
    QPushButton *cmdCancel;

    void setupUi(QDialog *addUserAction)
    {
        if (addUserAction->objectName().isEmpty())
            addUserAction->setObjectName(QString::fromUtf8("addUserAction"));
        addUserAction->resize(340, 170);
        addUserAction->setMinimumSize(QSize(340, 170));
        addUserAction->setMaximumSize(QSize(340, 170));
        verticalLayout = new QVBoxLayout(addUserAction);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(addUserAction);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        txtActionName = new QLineEdit(addUserAction);
        txtActionName->setObjectName(QString::fromUtf8("txtActionName"));
        txtActionName->setMinimumSize(QSize(0, 25));
        txtActionName->setMaximumSize(QSize(16777215, 25));
        txtActionName->setFocusPolicy(Qt::StrongFocus);

        verticalLayout->addWidget(txtActionName);

        label_2 = new QLabel(addUserAction);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        lblHotKey = new QLabel(addUserAction);
        lblHotKey->setObjectName(QString::fromUtf8("lblHotKey"));
        lblHotKey->setMinimumSize(QSize(0, 22));
        lblHotKey->setFocusPolicy(Qt::WheelFocus);
        lblHotKey->setFrameShape(QFrame::Panel);
        lblHotKey->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lblHotKey);

        label_3 = new QLabel(addUserAction);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        txtCommandAction = new QLineEdit(addUserAction);
        txtCommandAction->setObjectName(QString::fromUtf8("txtCommandAction"));
        txtCommandAction->setMinimumSize(QSize(0, 25));
        txtCommandAction->setMaximumSize(QSize(16777215, 25));
        txtCommandAction->setFocusPolicy(Qt::StrongFocus);

        verticalLayout->addWidget(txtCommandAction);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        cmdSave = new QPushButton(addUserAction);
        cmdSave->setObjectName(QString::fromUtf8("cmdSave"));

        horizontalLayout->addWidget(cmdSave);

        cmdCancel = new QPushButton(addUserAction);
        cmdCancel->setObjectName(QString::fromUtf8("cmdCancel"));

        horizontalLayout->addWidget(cmdCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(addUserAction);

        QMetaObject::connectSlotsByName(addUserAction);
    } // setupUi

    void retranslateUi(QDialog *addUserAction)
    {
        addUserAction->setWindowTitle(QApplication::translate("addUserAction", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("addUserAction", "Action name:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("addUserAction", "Key bind:", 0, QApplication::UnicodeUTF8));
        lblHotKey->setText(QString());
        label_3->setText(QApplication::translate("addUserAction", "User action:", 0, QApplication::UnicodeUTF8));
        cmdSave->setText(QApplication::translate("addUserAction", "Save", 0, QApplication::UnicodeUTF8));
        cmdCancel->setText(QApplication::translate("addUserAction", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class addUserAction: public Ui_addUserAction {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDUSERACTION_H
