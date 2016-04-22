/********************************************************************************
** Form generated from reading UI file 'frmabout.ui'
**
** Created: Thu Oct 17 12:29:17 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMABOUT_H
#define UI_FRMABOUT_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_frmAbout
{
public:
    QPushButton *cmdClose;
    QFrame *line;
    QLabel *lblAppName;
    QLabel *lblDescription;
    QLabel *lblAuthors;
    QLabel *lblAuthors_kXneur;
    QLabel *lblAuthors_xNeur;
    QLabel *lblLogo;
    QLabel *lblCurrentVersion;

    void setupUi(QDialog *frmAbout)
    {
        if (frmAbout->objectName().isEmpty())
            frmAbout->setObjectName(QString::fromUtf8("frmAbout"));
        frmAbout->resize(325, 325);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/kdeneur.png"), QSize(), QIcon::Normal, QIcon::Off);
        frmAbout->setWindowIcon(icon);
        frmAbout->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        cmdClose = new QPushButton(frmAbout);
        cmdClose->setObjectName(QString::fromUtf8("cmdClose"));
        cmdClose->setGeometry(QRect(227, 296, 91, 23));
        line = new QFrame(frmAbout);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 278, 311, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        lblAppName = new QLabel(frmAbout);
        lblAppName->setObjectName(QString::fromUtf8("lblAppName"));
        lblAppName->setGeometry(QRect(0, 10, 321, 20));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        lblAppName->setFont(font);
        lblAppName->setAlignment(Qt::AlignCenter);
        lblDescription = new QLabel(frmAbout);
        lblDescription->setObjectName(QString::fromUtf8("lblDescription"));
        lblDescription->setGeometry(QRect(0, 30, 321, 20));
        lblDescription->setAlignment(Qt::AlignCenter);
        lblAuthors = new QLabel(frmAbout);
        lblAuthors->setObjectName(QString::fromUtf8("lblAuthors"));
        lblAuthors->setGeometry(QRect(0, 57, 321, 20));
        lblAuthors->setAlignment(Qt::AlignCenter);
        lblAuthors_kXneur = new QLabel(frmAbout);
        lblAuthors_kXneur->setObjectName(QString::fromUtf8("lblAuthors_kXneur"));
        lblAuthors_kXneur->setGeometry(QRect(0, 76, 321, 31));
        lblAuthors_kXneur->setAlignment(Qt::AlignCenter);
        lblAuthors_xNeur = new QLabel(frmAbout);
        lblAuthors_xNeur->setObjectName(QString::fromUtf8("lblAuthors_xNeur"));
        lblAuthors_xNeur->setGeometry(QRect(0, 100, 321, 61));
        lblAuthors_xNeur->setAlignment(Qt::AlignCenter);
        lblLogo = new QLabel(frmAbout);
        lblLogo->setObjectName(QString::fromUtf8("lblLogo"));
        lblLogo->setGeometry(QRect(116, 160, 91, 91));
        lblLogo->setPixmap(QPixmap(QString::fromUtf8(":/icons/kdeneur.png")));
        lblLogo->setScaledContents(true);
        lblCurrentVersion = new QLabel(frmAbout);
        lblCurrentVersion->setObjectName(QString::fromUtf8("lblCurrentVersion"));
        lblCurrentVersion->setGeometry(QRect(11, 265, 301, 20));
        lblCurrentVersion->setAlignment(Qt::AlignCenter);

        retranslateUi(frmAbout);

        QMetaObject::connectSlotsByName(frmAbout);
    } // setupUi

    void retranslateUi(QDialog *frmAbout)
    {
        frmAbout->setWindowTitle(QApplication::translate("frmAbout", "kXneur About...", 0, QApplication::UnicodeUTF8));
        cmdClose->setText(QApplication::translate("frmAbout", "Close", 0, QApplication::UnicodeUTF8));
        lblAppName->setText(QApplication::translate("frmAbout", "X Neural Switch for Kde", 0, QApplication::UnicodeUTF8));
        lblDescription->setText(QApplication::translate("frmAbout", "Kde frontend for xNeur", 0, QApplication::UnicodeUTF8));
        lblAuthors->setText(QApplication::translate("frmAbout", "AUTHORS:", 0, QApplication::UnicodeUTF8));
        lblAuthors_kXneur->setText(QApplication::translate("frmAbout", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Droid Sans Japanese'; font-weight:600;\">kdeNeur</span><span style=\" font-family:'Droid Sans Japanese';\"> </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Droid Sans Japanese';\">Sergei Chystyakov</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        lblAuthors_xNeur->setText(QApplication::translate("frmAbout", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Droid Sans Japanese'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">Xneur </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Andrew Crew Kuznetsov</p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Nikolay Yankin</p></body></html>", 0, QApplication::UnicodeUTF8));
        lblLogo->setText(QString());
        lblCurrentVersion->setText(QApplication::translate("frmAbout", "Current Version:%1", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class frmAbout: public Ui_frmAbout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMABOUT_H
