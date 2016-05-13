/********************************************************************************
** Form generated from reading UI file 'frmaddabbreviature.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMADDABBREVIATURE_H
#define UI_FRMADDABBREVIATURE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

QT_BEGIN_NAMESPACE

class Ui_frmAddAbbreviature
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *txtAbbrev;
    QLabel *label_2;
    QLineEdit *txtFullText;
    QDialogButtonBox *cmdBox;

    void setupUi(QDialog *frmAddAbbreviature)
    {
        if (frmAddAbbreviature->objectName().isEmpty())
            frmAddAbbreviature->setObjectName(QString::fromUtf8("frmAddAbbreviature"));
        frmAddAbbreviature->resize(335, 125);
        frmAddAbbreviature->setMinimumSize(QSize(335, 125));
        frmAddAbbreviature->setMaximumSize(QSize(335, 125));
        gridLayout = new QGridLayout(frmAddAbbreviature);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(frmAddAbbreviature);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        txtAbbrev = new QLineEdit(frmAddAbbreviature);
        txtAbbrev->setObjectName(QString::fromUtf8("txtAbbrev"));

        gridLayout->addWidget(txtAbbrev, 1, 0, 1, 1);

        label_2 = new QLabel(frmAddAbbreviature);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        txtFullText = new QLineEdit(frmAddAbbreviature);
        txtFullText->setObjectName(QString::fromUtf8("txtFullText"));

        gridLayout->addWidget(txtFullText, 3, 0, 1, 1);

        cmdBox = new QDialogButtonBox(frmAddAbbreviature);
        cmdBox->setObjectName(QString::fromUtf8("cmdBox"));
        cmdBox->setOrientation(Qt::Horizontal);
        cmdBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        gridLayout->addWidget(cmdBox, 4, 0, 1, 1);


        retranslateUi(frmAddAbbreviature);
        QObject::connect(cmdBox, SIGNAL(accepted()), frmAddAbbreviature, SLOT(accept()));
        QObject::connect(cmdBox, SIGNAL(rejected()), frmAddAbbreviature, SLOT(reject()));

        QMetaObject::connectSlotsByName(frmAddAbbreviature);
    } // setupUi

    void retranslateUi(QDialog *frmAddAbbreviature)
    {
        frmAddAbbreviature->setWindowTitle(QApplication::translate("frmAddAbbreviature", "Add Abbreviation...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("frmAddAbbreviature", "Abbreviation:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("frmAddAbbreviature", "Expansion text:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class frmAddAbbreviature: public Ui_frmAddAbbreviature {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMADDABBREVIATURE_H
