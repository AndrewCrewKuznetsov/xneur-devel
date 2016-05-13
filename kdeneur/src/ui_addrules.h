/********************************************************************************
** Form generated from reading UI file 'addrules.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDRULES_H
#define UI_ADDRULES_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddRules
{
public:
    QDialogButtonBox *cmdBox;
    QLabel *label;
    QLineEdit *txtWords;
    QLabel *label_2;
    QGroupBox *groupBox;
    QLabel *label_3;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QRadioButton *rdbComtaint;
    QRadioButton *rdbStart;
    QRadioButton *rdbEnd;
    QRadioButton *rdbCoincide;
    QLabel *label_4;
    QCheckBox *chkInsensitive;

    void setupUi(QDialog *AddRules)
    {
        if (AddRules->objectName().isEmpty())
            AddRules->setObjectName(QString::fromUtf8("AddRules"));
        AddRules->resize(358, 375);
        AddRules->setMinimumSize(QSize(358, 375));
        AddRules->setMaximumSize(QSize(358, 375));
        cmdBox = new QDialogButtonBox(AddRules);
        cmdBox->setObjectName(QString::fromUtf8("cmdBox"));
        cmdBox->setGeometry(QRect(183, 340, 161, 25));
        cmdBox->setOrientation(Qt::Horizontal);
        cmdBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        label = new QLabel(AddRules);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, -1, 341, 51));
        label->setWordWrap(true);
        txtWords = new QLineEdit(AddRules);
        txtWords->setObjectName(QString::fromUtf8("txtWords"));
        txtWords->setGeometry(QRect(10, 43, 340, 25));
        txtWords->setMinimumSize(QSize(0, 25));
        txtWords->setMaximumSize(QSize(16777215, 25));
        label_2 = new QLabel(AddRules);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 73, 341, 51));
        label_2->setWordWrap(true);
        groupBox = new QGroupBox(AddRules);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 117, 340, 221));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 20, 321, 31));
        label_3->setMinimumSize(QSize(300, 0));
        verticalLayoutWidget = new QWidget(groupBox);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(20, 50, 311, 101));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        rdbComtaint = new QRadioButton(verticalLayoutWidget);
        rdbComtaint->setObjectName(QString::fromUtf8("rdbComtaint"));

        verticalLayout->addWidget(rdbComtaint);

        rdbStart = new QRadioButton(verticalLayoutWidget);
        rdbStart->setObjectName(QString::fromUtf8("rdbStart"));

        verticalLayout->addWidget(rdbStart);

        rdbEnd = new QRadioButton(verticalLayoutWidget);
        rdbEnd->setObjectName(QString::fromUtf8("rdbEnd"));

        verticalLayout->addWidget(rdbEnd);

        rdbCoincide = new QRadioButton(verticalLayoutWidget);
        rdbCoincide->setObjectName(QString::fromUtf8("rdbCoincide"));

        verticalLayout->addWidget(rdbCoincide);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(14, 161, 380, 15));
        chkInsensitive = new QCheckBox(groupBox);
        chkInsensitive->setObjectName(QString::fromUtf8("chkInsensitive"));
        chkInsensitive->setGeometry(QRect(43, 180, 281, 21));

        retranslateUi(AddRules);
        QObject::connect(cmdBox, SIGNAL(accepted()), AddRules, SLOT(accept()));
        QObject::connect(cmdBox, SIGNAL(rejected()), AddRules, SLOT(reject()));

        QMetaObject::connectSlotsByName(AddRules);
    } // setupUi

    void retranslateUi(QDialog *AddRules)
    {
        AddRules->setWindowTitle(QApplication::translate("AddRules", "Add your rule change...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("AddRules", "The new rule change to that language. Enter a combination of letters and set the condition for its treatment:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("AddRules", "NOTE: Special characters [] \\\\ ^ $. |? * + () {} must be preceded by a \\ (backslash).", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("AddRules", "Condition", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("AddRules", "Typed on the keyboard word must:", 0, QApplication::UnicodeUTF8));
        rdbComtaint->setText(QApplication::translate("AddRules", "contain that sequence of letters", 0, QApplication::UnicodeUTF8));
        rdbStart->setText(QApplication::translate("AddRules", "start with these letters", 0, QApplication::UnicodeUTF8));
        rdbEnd->setText(QApplication::translate("AddRules", "to end these letters", 0, QApplication::UnicodeUTF8));
        rdbCoincide->setText(QApplication::translate("AddRules", "coincide with the combination of letters", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("AddRules", "More:", 0, QApplication::UnicodeUTF8));
        chkInsensitive->setText(QApplication::translate("AddRules", "Insensitive", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class AddRules: public Ui_AddRules {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDRULES_H
