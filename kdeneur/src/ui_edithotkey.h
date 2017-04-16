/********************************************************************************
** Form generated from reading UI file 'edithotkey.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EDITHOTKEY_H
#define UI_EDITHOTKEY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_EditHotKey
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QComboBox *comboAction;
    QLabel *label_2;
    QLabel *lblHotKey;
    QHBoxLayout *horizontalLayout;
    QPushButton *cmdSave;
    QPushButton *cmdCancel;

    void setupUi(QDialog *EditHotKey)
    {
        if (EditHotKey->objectName().isEmpty())
            EditHotKey->setObjectName(QString::fromUtf8("EditHotKey"));
        EditHotKey->resize(285, 130);
        EditHotKey->setMinimumSize(QSize(285, 130));
        EditHotKey->setMaximumSize(QSize(285, 130));
        verticalLayout = new QVBoxLayout(EditHotKey);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(EditHotKey);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        comboAction = new QComboBox(EditHotKey);
        comboAction->setObjectName(QString::fromUtf8("comboAction"));

        verticalLayout->addWidget(comboAction);

        label_2 = new QLabel(EditHotKey);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout->addWidget(label_2);

        lblHotKey = new QLabel(EditHotKey);
        lblHotKey->setObjectName(QString::fromUtf8("lblHotKey"));
        QFont font;
        font.setKerning(false);
        lblHotKey->setFont(font);
        lblHotKey->setFocusPolicy(Qt::WheelFocus);
        lblHotKey->setAutoFillBackground(false);
        lblHotKey->setFrameShape(QFrame::Panel);
        lblHotKey->setFrameShadow(QFrame::Plain);
        lblHotKey->setLineWidth(1);
        lblHotKey->setMidLineWidth(0);
        lblHotKey->setScaledContents(false);
        lblHotKey->setAlignment(Qt::AlignCenter);
        lblHotKey->setMargin(1);

        verticalLayout->addWidget(lblHotKey);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        cmdSave = new QPushButton(EditHotKey);
        cmdSave->setObjectName(QString::fromUtf8("cmdSave"));
        cmdSave->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(cmdSave);

        cmdCancel = new QPushButton(EditHotKey);
        cmdCancel->setObjectName(QString::fromUtf8("cmdCancel"));
        cmdCancel->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(cmdCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(EditHotKey);

        QMetaObject::connectSlotsByName(EditHotKey);
    } // setupUi

    void retranslateUi(QDialog *EditHotKey)
    {
        EditHotKey->setWindowTitle(QApplication::translate("EditHotKey", "Add action...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("EditHotKey", "Action name:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("EditHotKey", "Key bind:", 0, QApplication::UnicodeUTF8));
        lblHotKey->setText(QApplication::translate("EditHotKey", "TextLabel", 0, QApplication::UnicodeUTF8));
        cmdSave->setText(QApplication::translate("EditHotKey", "Save", 0, QApplication::UnicodeUTF8));
        cmdCancel->setText(QApplication::translate("EditHotKey", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class EditHotKey: public Ui_EditHotKey {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EDITHOTKEY_H
