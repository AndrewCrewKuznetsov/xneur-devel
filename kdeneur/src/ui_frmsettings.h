/********************************************************************************
** Form generated from reading UI file 'frmsettings.ui'
**
** Created: Thu Oct 17 12:29:17 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRMSETTINGS_H
#define UI_FRMSETTINGS_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "tabbar.h"

QT_BEGIN_NAMESPACE

class Ui_frmSettings
{
public:
    QVBoxLayout *verticalLayout;
    TabWidget *tabWidget;
    TabBar *tabGeneral;
    QGridLayout *gridLayout;
    QTabWidget *tbWidGeneral;
    QWidget *tabMain;
    QGridLayout *gridLayout_2;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *chkGenMain_ManualSwitch;
    QCheckBox *chkGenMain_AutoLearning;
    QCheckBox *chkGenMain_KeepSelect;
    QCheckBox *chkGenMain_RotateLayout;
    QCheckBox *chkGenMain_CheckLang;
    QCheckBox *chkGenMain_CheckSimilar;
    QSpacerItem *verticalSpacer;
    QWidget *tabTipographics;
    QGridLayout *gridLayout_3;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *chkGenTipograph_CeorrectCAPS;
    QCheckBox *chkGenTipograph_DisableCaps;
    QCheckBox *chkGenTipograph_CorrectTwoCaps;
    QCheckBox *chkGenTipograph_CorrectSpace;
    QCheckBox *chkGenTipograph_CorrectSmallLitter;
    QCheckBox *chkGenTipograph_CorrectTwoSpase;
    QCheckBox *chkGenTipograph_CorrectTwoMinus;
    QCheckBox *chkGenTipograph_CorrectDash;
    QCheckBox *chkGenTipograph_Correct_c_;
    QCheckBox *chkGenTipograph_Correct_tm_;
    QCheckBox *chkGenTipograph_Correct_r_;
    QCheckBox *chkGenTipograph_Correct_Three_Point;
    QCheckBox *chkGenTipograph_Correct_Misprint;
    QSpacerItem *verticalSpacer_2;
    TabBar *tabLayouts;
    QVBoxLayout *verticalLayout_17;
    QGridLayout *gridLayout_4;
    QTableWidget *Layout_lstLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *Layout_lblLayoutNomber;
    QSpinBox *Layout_spbLayoutNumber;
    QSpacerItem *horizontalSpacer_10;
    QPushButton *Layout_cmdRulesChange;
    QFrame *line_2;
    QCheckBox *Layout_chkRememberKbLayout;
    QGroupBox *groupBox_12;
    QHBoxLayout *horizontalLayout;
    QListWidget *Layout_lstListApplicationOneKbLayout;
    QVBoxLayout *verticalLayout_16;
    QSpacerItem *verticalSpacer_8;
    QPushButton *Layout_AddApp;
    QPushButton *Layout_DelApp;
    TabBar *tabHotKeys;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *tabHotKey_lstHotKey;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *tabHotKeys_EditHotKey;
    QPushButton *tabHotKeys_ClearHotKey;
    QSpacerItem *horizontalSpacer;
    QTableWidget *tabHotKey_lstUserActions;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *tabHotKeys_UserActionsAdd;
    QPushButton *tabHotKeys_UserActionsEdit;
    QPushButton *tabHotKeys_UserActionsDel;
    QSpacerItem *horizontalSpacer_2;
    TabBar *tabAutocompletion;
    QGridLayout *gridLayout_6;
    QCheckBox *tabAutocompletion_chkAddSpace;
    QFrame *line;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_5;
    QListWidget *tabAutocompletion_lstApp;
    QVBoxLayout *verticalLayout_5;
    QSpacerItem *verticalSpacer_13;
    QPushButton *tabAutocompletion_cmdAddApp;
    QPushButton *tabAutocompletion_cmdDelApp;
    QCheckBox *tabAutocompletion_chkTrueAutocomplit;
    TabBar *tabApplications;
    QGridLayout *gridLayout_5;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_21;
    QListWidget *taApplication_lstAppNotUsed;
    QVBoxLayout *verticalLayout_8;
    QSpacerItem *verticalSpacer_3;
    QPushButton *taApplication_cmdAdd_NotUsed;
    QPushButton *taApplication_cmdDel_NotUsed;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_23;
    QListWidget *taApplication_lstAppAutoMode;
    QVBoxLayout *verticalLayout_7;
    QSpacerItem *verticalSpacer_4;
    QPushButton *taApplication_cmdAdd_AutoMode;
    QPushButton *taApplication_cmdDel_AutoMode;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_22;
    QListWidget *taApplication_lstAppManualMode;
    QVBoxLayout *verticalLayout_6;
    QSpacerItem *verticalSpacer_5;
    QPushButton *taApplication_cmdAdd_ManualMode;
    QPushButton *taApplication_cmdDel_ManualMode;
    QSpacerItem *horizontalSpacer_4;
    TabBar *tabNotifications;
    QGridLayout *gridLayout_7;
    QTabWidget *tbWidNotifications;
    QWidget *tabSound;
    QGridLayout *gridLayout_8;
    QCheckBox *tabSound_chkEnableSound;
    QHBoxLayout *horizontalLayout_6;
    QLabel *lblSoundVolume;
    QSpinBox *tabSound_spbSoundVolume;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_13;
    QPushButton *tabSound_cmdEdit;
    QSpacerItem *horizontalSpacer_9;
    QTableWidget *tabSound_lstListSound;
    QWidget *tabOSD;
    QVBoxLayout *verticalLayout_9;
    QCheckBox *tabOSD_chkEnableOSD;
    QHBoxLayout *horizontalLayout_7;
    QLabel *lblFontOSD;
    QLineEdit *tabOSD_txtFontOSD;
    QLabel *label_4;
    QTableWidget *tabOSD_lstListOSD;
    QWidget *tabPopupMessage;
    QGridLayout *gridLayout_9;
    QCheckBox *tabPopupMessage_chkShowPopupMessage;
    QHBoxLayout *horizontalLayout_8;
    QLabel *lblInterval;
    QSpinBox *tabPopupMessage_spbIntervalPopup;
    QTableWidget *tabPopupMessage_lstListPopupMessage;
    TabBar *tabAbbreviations;
    QGridLayout *gridLayout_10;
    QCheckBox *tabAbbreviations_chkIgnoreKeyLayout;
    QTableWidget *tabAbbreviations_lstListAbbreviations;
    QHBoxLayout *horizontalLayout_9;
    QPushButton *tabAbbreviations_cmdAdd;
    QPushButton *tabAbbreviations_cmdDel;
    QSpacerItem *horizontalSpacer_6;
    TabBar *tabLog;
    QCheckBox *tabLog_chkEnableLog;
    QWidget *layoutWidget_1;
    QGridLayout *gridLayout_11;
    QLabel *lblSizeLog;
    QSpinBox *tabLog_spbSizeLog;
    QLabel *lbSendLogEmail;
    QLineEdit *tabLog_txtSendLogEmail;
    QLabel *lblSendLogHost;
    QLineEdit *tabLog_txtSendLogHost;
    QLabel *lblSendLogPort;
    QSpinBox *tabLog_spbSendLogPort;
    QLabel *lblOpenLogIn;
    QComboBox *tabLog_cmbOpenLongIn;
    QPushButton *tabLog_cmdOpenLog;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *verticalSpacer_6;
    TabBar *tabTroubleshooting;
    QVBoxLayout *verticalLayout_12;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_12;
    QVBoxLayout *verticalLayout_10;
    QCheckBox *tabTroubleshooting_chkBackspace;
    QCheckBox *tabTroubleshooting_chkLeftArrow;
    QCheckBox *tabTroubleshooting_chkRightArrow;
    QCheckBox *tabTroubleshooting_chkUpArrow;
    QCheckBox *tabTroubleshooting_chkDownArrow;
    QCheckBox *tabTroubleshooting_chkDelete;
    QCheckBox *tabTroubleshooting_chkEnter;
    QCheckBox *tabTroubleshooting_chkTab;
    QCheckBox *tabTroubleshooting_chkChangeLayout;
    QCheckBox *tabTroubleshooting_chkFullScreen;
    QLabel *lblProperties;
    QSpacerItem *verticalSpacer_7;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_13;
    QVBoxLayout *verticalLayout_11;
    QCheckBox *tabTroubleshooting_chkFlushInterBuffEscape;
    QCheckBox *tabTroubleshooting_chkFlushInterBuffEnterTab;
    QCheckBox *tabTroubleshooting_chkCompatCompletion;
    QGroupBox *groupBox_7;
    QGridLayout *gridLayout_15;
    QGridLayout *gridLayout_14;
    QCheckBox *tabTroubleshooting_chkMonitorInput;
    QLabel *lblProperties_2;
    QCheckBox *tabTroubleshooting_chkMonitorMouse;
    QLabel *lblProperties_3;
    QSpacerItem *verticalSpacer_11;
    TabBar *tabAdvanced;
    QVBoxLayout *verticalLayout_18;
    QGridLayout *gridLayout_16;
    QLabel *lblDelay;
    QSpinBox *tabAdvanced_spbDelay;
    QLabel *lblLogLevel;
    QComboBox *tabAdvanced_cmbLogLevel;
    QLabel *lblKeyRelease;
    QListWidget *tabAdvanced_tabAppList;
    QHBoxLayout *horizontalLayout_14;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *tabAdvanced_cmdAddApp;
    QPushButton *tabAdvanced_cmdRemoveApp;
    TabBar *tabPlugins;
    QGridLayout *gridLayout_17;
    QTableWidget *tabPlugins_lstListPlugins;
    TabBar *tabProperties;
    QVBoxLayout *verticalLayout_15;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout_18;
    QVBoxLayout *verticalLayout_13;
    QCheckBox *tabProperties_chkEnableAutostart;
    QHBoxLayout *horizontalLayout_10;
    QLabel *lblDelayStartApp;
    QSpinBox *tabProperties_spbDelayStartApp;
    QSpacerItem *horizontalSpacer_8;
    QGroupBox *groupBox_9;
    QHBoxLayout *horizontalLayout_12;
    QVBoxLayout *verticalLayout_14;
    QComboBox *tabProperties_cmbTypeIconTray;
    QGroupBox *tabProperties_grpFolderIcon;
    QGridLayout *gridLayout_19;
    QLineEdit *tabProperties_txtPathIconTray;
    QPushButton *tabProperties_cmdBrowseIconTray;
    QSpacerItem *verticalSpacer_9;
    QGroupBox *groupBox_11;
    QGridLayout *gridLayout_20;
    QHBoxLayout *horizontalLayout_11;
    QLabel *lblKeyboardCommand;
    QLineEdit *tabProperties_txtKeyboardCommad;
    QPushButton *tabProperties_cmdEditKeyCommand;
    QPushButton *tabProperties_cmdRecoverKeyCommand;
    QSpacerItem *verticalSpacer_12;
    QDialogButtonBox *cmdBox;

    void setupUi(QDialog *frmSettings)
    {
        if (frmSettings->objectName().isEmpty())
            frmSettings->setObjectName(QString::fromUtf8("frmSettings"));
        frmSettings->resize(752, 640);
        frmSettings->setMaximumSize(QSize(760, 640));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/icons/kdeneur.png"), QSize(), QIcon::Normal, QIcon::Off);
        frmSettings->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(frmSettings);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new TabWidget(frmSettings);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        tabWidget->setTabPosition(QTabWidget::West);
        tabGeneral = new TabBar();
        tabGeneral->setObjectName(QString::fromUtf8("tabGeneral"));
        tabGeneral->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout = new QGridLayout(tabGeneral);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tbWidGeneral = new QTabWidget(tabGeneral);
        tbWidGeneral->setObjectName(QString::fromUtf8("tbWidGeneral"));
        tabMain = new QWidget();
        tabMain->setObjectName(QString::fromUtf8("tabMain"));
        gridLayout_2 = new QGridLayout(tabMain);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        chkGenMain_ManualSwitch = new QCheckBox(tabMain);
        chkGenMain_ManualSwitch->setObjectName(QString::fromUtf8("chkGenMain_ManualSwitch"));

        verticalLayout_2->addWidget(chkGenMain_ManualSwitch);

        chkGenMain_AutoLearning = new QCheckBox(tabMain);
        chkGenMain_AutoLearning->setObjectName(QString::fromUtf8("chkGenMain_AutoLearning"));

        verticalLayout_2->addWidget(chkGenMain_AutoLearning);

        chkGenMain_KeepSelect = new QCheckBox(tabMain);
        chkGenMain_KeepSelect->setObjectName(QString::fromUtf8("chkGenMain_KeepSelect"));

        verticalLayout_2->addWidget(chkGenMain_KeepSelect);

        chkGenMain_RotateLayout = new QCheckBox(tabMain);
        chkGenMain_RotateLayout->setObjectName(QString::fromUtf8("chkGenMain_RotateLayout"));

        verticalLayout_2->addWidget(chkGenMain_RotateLayout);

        chkGenMain_CheckLang = new QCheckBox(tabMain);
        chkGenMain_CheckLang->setObjectName(QString::fromUtf8("chkGenMain_CheckLang"));

        verticalLayout_2->addWidget(chkGenMain_CheckLang);

        chkGenMain_CheckSimilar = new QCheckBox(tabMain);
        chkGenMain_CheckSimilar->setObjectName(QString::fromUtf8("chkGenMain_CheckSimilar"));

        verticalLayout_2->addWidget(chkGenMain_CheckSimilar);


        gridLayout_2->addLayout(verticalLayout_2, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 1, 0, 1, 1);

        tbWidGeneral->addTab(tabMain, QString());
        tabTipographics = new QWidget();
        tabTipographics->setObjectName(QString::fromUtf8("tabTipographics"));
        gridLayout_3 = new QGridLayout(tabTipographics);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        chkGenTipograph_CeorrectCAPS = new QCheckBox(tabTipographics);
        chkGenTipograph_CeorrectCAPS->setObjectName(QString::fromUtf8("chkGenTipograph_CeorrectCAPS"));

        verticalLayout_3->addWidget(chkGenTipograph_CeorrectCAPS);

        chkGenTipograph_DisableCaps = new QCheckBox(tabTipographics);
        chkGenTipograph_DisableCaps->setObjectName(QString::fromUtf8("chkGenTipograph_DisableCaps"));

        verticalLayout_3->addWidget(chkGenTipograph_DisableCaps);

        chkGenTipograph_CorrectTwoCaps = new QCheckBox(tabTipographics);
        chkGenTipograph_CorrectTwoCaps->setObjectName(QString::fromUtf8("chkGenTipograph_CorrectTwoCaps"));

        verticalLayout_3->addWidget(chkGenTipograph_CorrectTwoCaps);

        chkGenTipograph_CorrectSpace = new QCheckBox(tabTipographics);
        chkGenTipograph_CorrectSpace->setObjectName(QString::fromUtf8("chkGenTipograph_CorrectSpace"));

        verticalLayout_3->addWidget(chkGenTipograph_CorrectSpace);

        chkGenTipograph_CorrectSmallLitter = new QCheckBox(tabTipographics);
        chkGenTipograph_CorrectSmallLitter->setObjectName(QString::fromUtf8("chkGenTipograph_CorrectSmallLitter"));

        verticalLayout_3->addWidget(chkGenTipograph_CorrectSmallLitter);

        chkGenTipograph_CorrectTwoSpase = new QCheckBox(tabTipographics);
        chkGenTipograph_CorrectTwoSpase->setObjectName(QString::fromUtf8("chkGenTipograph_CorrectTwoSpase"));

        verticalLayout_3->addWidget(chkGenTipograph_CorrectTwoSpase);

        chkGenTipograph_CorrectTwoMinus = new QCheckBox(tabTipographics);
        chkGenTipograph_CorrectTwoMinus->setObjectName(QString::fromUtf8("chkGenTipograph_CorrectTwoMinus"));

        verticalLayout_3->addWidget(chkGenTipograph_CorrectTwoMinus);

        chkGenTipograph_CorrectDash = new QCheckBox(tabTipographics);
        chkGenTipograph_CorrectDash->setObjectName(QString::fromUtf8("chkGenTipograph_CorrectDash"));

        verticalLayout_3->addWidget(chkGenTipograph_CorrectDash);

        chkGenTipograph_Correct_c_ = new QCheckBox(tabTipographics);
        chkGenTipograph_Correct_c_->setObjectName(QString::fromUtf8("chkGenTipograph_Correct_c_"));

        verticalLayout_3->addWidget(chkGenTipograph_Correct_c_);

        chkGenTipograph_Correct_tm_ = new QCheckBox(tabTipographics);
        chkGenTipograph_Correct_tm_->setObjectName(QString::fromUtf8("chkGenTipograph_Correct_tm_"));

        verticalLayout_3->addWidget(chkGenTipograph_Correct_tm_);

        chkGenTipograph_Correct_r_ = new QCheckBox(tabTipographics);
        chkGenTipograph_Correct_r_->setObjectName(QString::fromUtf8("chkGenTipograph_Correct_r_"));

        verticalLayout_3->addWidget(chkGenTipograph_Correct_r_);

        chkGenTipograph_Correct_Three_Point = new QCheckBox(tabTipographics);
        chkGenTipograph_Correct_Three_Point->setObjectName(QString::fromUtf8("chkGenTipograph_Correct_Three_Point"));

        verticalLayout_3->addWidget(chkGenTipograph_Correct_Three_Point);

        chkGenTipograph_Correct_Misprint = new QCheckBox(tabTipographics);
        chkGenTipograph_Correct_Misprint->setObjectName(QString::fromUtf8("chkGenTipograph_Correct_Misprint"));

        verticalLayout_3->addWidget(chkGenTipograph_Correct_Misprint);


        gridLayout_3->addLayout(verticalLayout_3, 0, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer_2, 1, 0, 1, 1);

        tbWidGeneral->addTab(tabTipographics, QString());

        gridLayout->addWidget(tbWidGeneral, 1, 0, 1, 1);

        tabWidget->addTab(tabGeneral, QString());
        tabLayouts = new TabBar();
        tabLayouts->setObjectName(QString::fromUtf8("tabLayouts"));
        tabLayouts->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_17 = new QVBoxLayout(tabLayouts);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        Layout_lstLayout = new QTableWidget(tabLayouts);
        Layout_lstLayout->setObjectName(QString::fromUtf8("Layout_lstLayout"));
        Layout_lstLayout->setProperty("showDropIndicator", QVariant(false));
        Layout_lstLayout->setSelectionMode(QAbstractItemView::SingleSelection);
        Layout_lstLayout->setSelectionBehavior(QAbstractItemView::SelectRows);
        Layout_lstLayout->setWordWrap(false);
        Layout_lstLayout->horizontalHeader()->setMinimumSectionSize(35);
        Layout_lstLayout->verticalHeader()->setVisible(false);
        Layout_lstLayout->verticalHeader()->setDefaultSectionSize(22);

        gridLayout_4->addWidget(Layout_lstLayout, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        Layout_lblLayoutNomber = new QLabel(tabLayouts);
        Layout_lblLayoutNomber->setObjectName(QString::fromUtf8("Layout_lblLayoutNomber"));
        Layout_lblLayoutNomber->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(Layout_lblLayoutNomber);

        Layout_spbLayoutNumber = new QSpinBox(tabLayouts);
        Layout_spbLayoutNumber->setObjectName(QString::fromUtf8("Layout_spbLayoutNumber"));
        Layout_spbLayoutNumber->setMaximumSize(QSize(50, 16777215));
        Layout_spbLayoutNumber->setAlignment(Qt::AlignCenter);
        Layout_spbLayoutNumber->setMinimum(1);
        Layout_spbLayoutNumber->setMaximum(4);
        Layout_spbLayoutNumber->setValue(4);

        horizontalLayout_2->addWidget(Layout_spbLayoutNumber);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_10);


        gridLayout_4->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        Layout_cmdRulesChange = new QPushButton(tabLayouts);
        Layout_cmdRulesChange->setObjectName(QString::fromUtf8("Layout_cmdRulesChange"));

        gridLayout_4->addWidget(Layout_cmdRulesChange, 1, 1, 1, 1);


        verticalLayout_17->addLayout(gridLayout_4);

        line_2 = new QFrame(tabLayouts);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setMinimumSize(QSize(0, 15));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_17->addWidget(line_2);

        Layout_chkRememberKbLayout = new QCheckBox(tabLayouts);
        Layout_chkRememberKbLayout->setObjectName(QString::fromUtf8("Layout_chkRememberKbLayout"));

        verticalLayout_17->addWidget(Layout_chkRememberKbLayout);

        groupBox_12 = new QGroupBox(tabLayouts);
        groupBox_12->setObjectName(QString::fromUtf8("groupBox_12"));
        horizontalLayout = new QHBoxLayout(groupBox_12);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Layout_lstListApplicationOneKbLayout = new QListWidget(groupBox_12);
        Layout_lstListApplicationOneKbLayout->setObjectName(QString::fromUtf8("Layout_lstListApplicationOneKbLayout"));

        horizontalLayout->addWidget(Layout_lstListApplicationOneKbLayout);

        verticalLayout_16 = new QVBoxLayout();
        verticalLayout_16->setObjectName(QString::fromUtf8("verticalLayout_16"));
        verticalSpacer_8 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_16->addItem(verticalSpacer_8);

        Layout_AddApp = new QPushButton(groupBox_12);
        Layout_AddApp->setObjectName(QString::fromUtf8("Layout_AddApp"));

        verticalLayout_16->addWidget(Layout_AddApp);

        Layout_DelApp = new QPushButton(groupBox_12);
        Layout_DelApp->setObjectName(QString::fromUtf8("Layout_DelApp"));

        verticalLayout_16->addWidget(Layout_DelApp);


        horizontalLayout->addLayout(verticalLayout_16);


        verticalLayout_17->addWidget(groupBox_12);

        tabWidget->addTab(tabLayouts, QString());
        tabHotKeys = new TabBar();
        tabHotKeys->setObjectName(QString::fromUtf8("tabHotKeys"));
        tabHotKeys->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_4 = new QVBoxLayout(tabHotKeys);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tabHotKey_lstHotKey = new QTableWidget(tabHotKeys);
        if (tabHotKey_lstHotKey->columnCount() < 2)
            tabHotKey_lstHotKey->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tabHotKey_lstHotKey->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tabHotKey_lstHotKey->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tabHotKey_lstHotKey->setObjectName(QString::fromUtf8("tabHotKey_lstHotKey"));

        verticalLayout_4->addWidget(tabHotKey_lstHotKey);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        tabHotKeys_EditHotKey = new QPushButton(tabHotKeys);
        tabHotKeys_EditHotKey->setObjectName(QString::fromUtf8("tabHotKeys_EditHotKey"));

        horizontalLayout_3->addWidget(tabHotKeys_EditHotKey);

        tabHotKeys_ClearHotKey = new QPushButton(tabHotKeys);
        tabHotKeys_ClearHotKey->setObjectName(QString::fromUtf8("tabHotKeys_ClearHotKey"));

        horizontalLayout_3->addWidget(tabHotKeys_ClearHotKey);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_4->addLayout(horizontalLayout_3);

        tabHotKey_lstUserActions = new QTableWidget(tabHotKeys);
        if (tabHotKey_lstUserActions->columnCount() < 3)
            tabHotKey_lstUserActions->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tabHotKey_lstUserActions->setHorizontalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tabHotKey_lstUserActions->setHorizontalHeaderItem(1, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tabHotKey_lstUserActions->setHorizontalHeaderItem(2, __qtablewidgetitem4);
        tabHotKey_lstUserActions->setObjectName(QString::fromUtf8("tabHotKey_lstUserActions"));

        verticalLayout_4->addWidget(tabHotKey_lstUserActions);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        tabHotKeys_UserActionsAdd = new QPushButton(tabHotKeys);
        tabHotKeys_UserActionsAdd->setObjectName(QString::fromUtf8("tabHotKeys_UserActionsAdd"));

        horizontalLayout_4->addWidget(tabHotKeys_UserActionsAdd);

        tabHotKeys_UserActionsEdit = new QPushButton(tabHotKeys);
        tabHotKeys_UserActionsEdit->setObjectName(QString::fromUtf8("tabHotKeys_UserActionsEdit"));

        horizontalLayout_4->addWidget(tabHotKeys_UserActionsEdit);

        tabHotKeys_UserActionsDel = new QPushButton(tabHotKeys);
        tabHotKeys_UserActionsDel->setObjectName(QString::fromUtf8("tabHotKeys_UserActionsDel"));

        horizontalLayout_4->addWidget(tabHotKeys_UserActionsDel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout_4->addLayout(horizontalLayout_4);

        tabWidget->addTab(tabHotKeys, QString());
        tabAutocompletion = new TabBar();
        tabAutocompletion->setObjectName(QString::fromUtf8("tabAutocompletion"));
        tabAutocompletion->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout_6 = new QGridLayout(tabAutocompletion);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        tabAutocompletion_chkAddSpace = new QCheckBox(tabAutocompletion);
        tabAutocompletion_chkAddSpace->setObjectName(QString::fromUtf8("tabAutocompletion_chkAddSpace"));

        gridLayout_6->addWidget(tabAutocompletion_chkAddSpace, 1, 0, 1, 1);

        line = new QFrame(tabAutocompletion);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        gridLayout_6->addWidget(line, 2, 0, 1, 1);

        groupBox = new QGroupBox(tabAutocompletion);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setLayoutDirection(Qt::LeftToRight);
        groupBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        horizontalLayout_5 = new QHBoxLayout(groupBox);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        tabAutocompletion_lstApp = new QListWidget(groupBox);
        tabAutocompletion_lstApp->setObjectName(QString::fromUtf8("tabAutocompletion_lstApp"));
        tabAutocompletion_lstApp->setMaximumSize(QSize(166666, 16777215));

        horizontalLayout_5->addWidget(tabAutocompletion_lstApp);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalSpacer_13 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer_13);

        tabAutocompletion_cmdAddApp = new QPushButton(groupBox);
        tabAutocompletion_cmdAddApp->setObjectName(QString::fromUtf8("tabAutocompletion_cmdAddApp"));

        verticalLayout_5->addWidget(tabAutocompletion_cmdAddApp);

        tabAutocompletion_cmdDelApp = new QPushButton(groupBox);
        tabAutocompletion_cmdDelApp->setObjectName(QString::fromUtf8("tabAutocompletion_cmdDelApp"));

        verticalLayout_5->addWidget(tabAutocompletion_cmdDelApp);


        horizontalLayout_5->addLayout(verticalLayout_5);


        gridLayout_6->addWidget(groupBox, 3, 0, 1, 1);

        tabAutocompletion_chkTrueAutocomplit = new QCheckBox(tabAutocompletion);
        tabAutocompletion_chkTrueAutocomplit->setObjectName(QString::fromUtf8("tabAutocompletion_chkTrueAutocomplit"));

        gridLayout_6->addWidget(tabAutocompletion_chkTrueAutocomplit, 0, 0, 1, 1);

        tabWidget->addTab(tabAutocompletion, QString());
        tabApplications = new TabBar();
        tabApplications->setObjectName(QString::fromUtf8("tabApplications"));
        tabApplications->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout_5 = new QGridLayout(tabApplications);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        groupBox_2 = new QGroupBox(tabApplications);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(335, 0));
        groupBox_2->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_21 = new QGridLayout(groupBox_2);
        gridLayout_21->setObjectName(QString::fromUtf8("gridLayout_21"));
        taApplication_lstAppNotUsed = new QListWidget(groupBox_2);
        taApplication_lstAppNotUsed->setObjectName(QString::fromUtf8("taApplication_lstAppNotUsed"));

        gridLayout_21->addWidget(taApplication_lstAppNotUsed, 0, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_2, 0, 0, 1, 1);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setSizeConstraint(QLayout::SetMaximumSize);
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_3);

        taApplication_cmdAdd_NotUsed = new QPushButton(tabApplications);
        taApplication_cmdAdd_NotUsed->setObjectName(QString::fromUtf8("taApplication_cmdAdd_NotUsed"));
        taApplication_cmdAdd_NotUsed->setMaximumSize(QSize(9999, 23));

        verticalLayout_8->addWidget(taApplication_cmdAdd_NotUsed);

        taApplication_cmdDel_NotUsed = new QPushButton(tabApplications);
        taApplication_cmdDel_NotUsed->setObjectName(QString::fromUtf8("taApplication_cmdDel_NotUsed"));
        taApplication_cmdDel_NotUsed->setMaximumSize(QSize(9999, 23));

        verticalLayout_8->addWidget(taApplication_cmdDel_NotUsed);


        gridLayout_5->addLayout(verticalLayout_8, 0, 1, 1, 1);

        groupBox_4 = new QGroupBox(tabApplications);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setMinimumSize(QSize(335, 0));
        groupBox_4->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_23 = new QGridLayout(groupBox_4);
        gridLayout_23->setObjectName(QString::fromUtf8("gridLayout_23"));
        taApplication_lstAppAutoMode = new QListWidget(groupBox_4);
        taApplication_lstAppAutoMode->setObjectName(QString::fromUtf8("taApplication_lstAppAutoMode"));

        gridLayout_23->addWidget(taApplication_lstAppAutoMode, 0, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_4, 1, 0, 1, 1);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setSizeConstraint(QLayout::SetMaximumSize);
        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_4);

        taApplication_cmdAdd_AutoMode = new QPushButton(tabApplications);
        taApplication_cmdAdd_AutoMode->setObjectName(QString::fromUtf8("taApplication_cmdAdd_AutoMode"));
        taApplication_cmdAdd_AutoMode->setMaximumSize(QSize(9999, 23));

        verticalLayout_7->addWidget(taApplication_cmdAdd_AutoMode);

        taApplication_cmdDel_AutoMode = new QPushButton(tabApplications);
        taApplication_cmdDel_AutoMode->setObjectName(QString::fromUtf8("taApplication_cmdDel_AutoMode"));
        taApplication_cmdDel_AutoMode->setMaximumSize(QSize(9999, 23));

        verticalLayout_7->addWidget(taApplication_cmdDel_AutoMode);


        gridLayout_5->addLayout(verticalLayout_7, 1, 1, 1, 1);

        groupBox_3 = new QGroupBox(tabApplications);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(335, 0));
        groupBox_3->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_22 = new QGridLayout(groupBox_3);
        gridLayout_22->setObjectName(QString::fromUtf8("gridLayout_22"));
        taApplication_lstAppManualMode = new QListWidget(groupBox_3);
        taApplication_lstAppManualMode->setObjectName(QString::fromUtf8("taApplication_lstAppManualMode"));

        gridLayout_22->addWidget(taApplication_lstAppManualMode, 0, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_3, 2, 0, 1, 1);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_5);

        taApplication_cmdAdd_ManualMode = new QPushButton(tabApplications);
        taApplication_cmdAdd_ManualMode->setObjectName(QString::fromUtf8("taApplication_cmdAdd_ManualMode"));
        taApplication_cmdAdd_ManualMode->setMaximumSize(QSize(9999, 23));

        verticalLayout_6->addWidget(taApplication_cmdAdd_ManualMode);

        taApplication_cmdDel_ManualMode = new QPushButton(tabApplications);
        taApplication_cmdDel_ManualMode->setObjectName(QString::fromUtf8("taApplication_cmdDel_ManualMode"));
        taApplication_cmdDel_ManualMode->setMaximumSize(QSize(9999, 23));

        verticalLayout_6->addWidget(taApplication_cmdDel_ManualMode);


        gridLayout_5->addLayout(verticalLayout_6, 2, 1, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_4, 3, 0, 1, 1);

        tabWidget->addTab(tabApplications, QString());
        tabNotifications = new TabBar();
        tabNotifications->setObjectName(QString::fromUtf8("tabNotifications"));
        tabNotifications->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout_7 = new QGridLayout(tabNotifications);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        tbWidNotifications = new QTabWidget(tabNotifications);
        tbWidNotifications->setObjectName(QString::fromUtf8("tbWidNotifications"));
        tabSound = new QWidget();
        tabSound->setObjectName(QString::fromUtf8("tabSound"));
        gridLayout_8 = new QGridLayout(tabSound);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        tabSound_chkEnableSound = new QCheckBox(tabSound);
        tabSound_chkEnableSound->setObjectName(QString::fromUtf8("tabSound_chkEnableSound"));

        gridLayout_8->addWidget(tabSound_chkEnableSound, 0, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        lblSoundVolume = new QLabel(tabSound);
        lblSoundVolume->setObjectName(QString::fromUtf8("lblSoundVolume"));

        horizontalLayout_6->addWidget(lblSoundVolume);

        tabSound_spbSoundVolume = new QSpinBox(tabSound);
        tabSound_spbSoundVolume->setObjectName(QString::fromUtf8("tabSound_spbSoundVolume"));
        tabSound_spbSoundVolume->setMaximumSize(QSize(71, 25));
        tabSound_spbSoundVolume->setAlignment(Qt::AlignCenter);
        tabSound_spbSoundVolume->setMaximum(100);

        horizontalLayout_6->addWidget(tabSound_spbSoundVolume);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);


        gridLayout_8->addLayout(horizontalLayout_6, 1, 0, 1, 1);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        tabSound_cmdEdit = new QPushButton(tabSound);
        tabSound_cmdEdit->setObjectName(QString::fromUtf8("tabSound_cmdEdit"));
        tabSound_cmdEdit->setMaximumSize(QSize(16999, 16777215));

        horizontalLayout_13->addWidget(tabSound_cmdEdit);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_9);


        gridLayout_8->addLayout(horizontalLayout_13, 3, 0, 1, 1);

        tabSound_lstListSound = new QTableWidget(tabSound);
        if (tabSound_lstListSound->columnCount() < 3)
            tabSound_lstListSound->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tabSound_lstListSound->setHorizontalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tabSound_lstListSound->setHorizontalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        __qtablewidgetitem7->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
        tabSound_lstListSound->setHorizontalHeaderItem(2, __qtablewidgetitem7);
        tabSound_lstListSound->setObjectName(QString::fromUtf8("tabSound_lstListSound"));
        tabSound_lstListSound->horizontalHeader()->setStretchLastSection(false);
        tabSound_lstListSound->verticalHeader()->setStretchLastSection(false);

        gridLayout_8->addWidget(tabSound_lstListSound, 2, 0, 1, 1);

        tbWidNotifications->addTab(tabSound, QString());
        tabOSD = new QWidget();
        tabOSD->setObjectName(QString::fromUtf8("tabOSD"));
        verticalLayout_9 = new QVBoxLayout(tabOSD);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        tabOSD_chkEnableOSD = new QCheckBox(tabOSD);
        tabOSD_chkEnableOSD->setObjectName(QString::fromUtf8("tabOSD_chkEnableOSD"));

        verticalLayout_9->addWidget(tabOSD_chkEnableOSD);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        lblFontOSD = new QLabel(tabOSD);
        lblFontOSD->setObjectName(QString::fromUtf8("lblFontOSD"));

        horizontalLayout_7->addWidget(lblFontOSD);

        tabOSD_txtFontOSD = new QLineEdit(tabOSD);
        tabOSD_txtFontOSD->setObjectName(QString::fromUtf8("tabOSD_txtFontOSD"));

        horizontalLayout_7->addWidget(tabOSD_txtFontOSD);


        verticalLayout_9->addLayout(horizontalLayout_7);

        label_4 = new QLabel(tabOSD);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout_9->addWidget(label_4);

        tabOSD_lstListOSD = new QTableWidget(tabOSD);
        if (tabOSD_lstListOSD->columnCount() < 3)
            tabOSD_lstListOSD->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tabOSD_lstListOSD->setHorizontalHeaderItem(0, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tabOSD_lstListOSD->setHorizontalHeaderItem(1, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tabOSD_lstListOSD->setHorizontalHeaderItem(2, __qtablewidgetitem10);
        tabOSD_lstListOSD->setObjectName(QString::fromUtf8("tabOSD_lstListOSD"));
        tabOSD_lstListOSD->horizontalHeader()->setStretchLastSection(false);

        verticalLayout_9->addWidget(tabOSD_lstListOSD);

        tbWidNotifications->addTab(tabOSD, QString());
        tabPopupMessage = new QWidget();
        tabPopupMessage->setObjectName(QString::fromUtf8("tabPopupMessage"));
        gridLayout_9 = new QGridLayout(tabPopupMessage);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        tabPopupMessage_chkShowPopupMessage = new QCheckBox(tabPopupMessage);
        tabPopupMessage_chkShowPopupMessage->setObjectName(QString::fromUtf8("tabPopupMessage_chkShowPopupMessage"));
        tabPopupMessage_chkShowPopupMessage->setChecked(false);

        gridLayout_9->addWidget(tabPopupMessage_chkShowPopupMessage, 0, 0, 1, 1);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        lblInterval = new QLabel(tabPopupMessage);
        lblInterval->setObjectName(QString::fromUtf8("lblInterval"));
        lblInterval->setMaximumSize(QSize(19999, 19999));

        horizontalLayout_8->addWidget(lblInterval);

        tabPopupMessage_spbIntervalPopup = new QSpinBox(tabPopupMessage);
        tabPopupMessage_spbIntervalPopup->setObjectName(QString::fromUtf8("tabPopupMessage_spbIntervalPopup"));
        tabPopupMessage_spbIntervalPopup->setMaximumSize(QSize(71, 25));
        tabPopupMessage_spbIntervalPopup->setAlignment(Qt::AlignCenter);
        tabPopupMessage_spbIntervalPopup->setMaximum(60000);
        tabPopupMessage_spbIntervalPopup->setValue(1000);

        horizontalLayout_8->addWidget(tabPopupMessage_spbIntervalPopup);


        gridLayout_9->addLayout(horizontalLayout_8, 1, 0, 1, 1);

        tabPopupMessage_lstListPopupMessage = new QTableWidget(tabPopupMessage);
        if (tabPopupMessage_lstListPopupMessage->columnCount() < 3)
            tabPopupMessage_lstListPopupMessage->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tabPopupMessage_lstListPopupMessage->setHorizontalHeaderItem(0, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tabPopupMessage_lstListPopupMessage->setHorizontalHeaderItem(1, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tabPopupMessage_lstListPopupMessage->setHorizontalHeaderItem(2, __qtablewidgetitem13);
        tabPopupMessage_lstListPopupMessage->setObjectName(QString::fromUtf8("tabPopupMessage_lstListPopupMessage"));
        tabPopupMessage_lstListPopupMessage->horizontalHeader()->setStretchLastSection(false);

        gridLayout_9->addWidget(tabPopupMessage_lstListPopupMessage, 2, 0, 1, 1);

        tbWidNotifications->addTab(tabPopupMessage, QString());

        gridLayout_7->addWidget(tbWidNotifications, 0, 0, 1, 1);

        tabWidget->addTab(tabNotifications, QString());
        tabAbbreviations = new TabBar();
        tabAbbreviations->setObjectName(QString::fromUtf8("tabAbbreviations"));
        tabAbbreviations->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout_10 = new QGridLayout(tabAbbreviations);
        gridLayout_10->setObjectName(QString::fromUtf8("gridLayout_10"));
        tabAbbreviations_chkIgnoreKeyLayout = new QCheckBox(tabAbbreviations);
        tabAbbreviations_chkIgnoreKeyLayout->setObjectName(QString::fromUtf8("tabAbbreviations_chkIgnoreKeyLayout"));

        gridLayout_10->addWidget(tabAbbreviations_chkIgnoreKeyLayout, 0, 0, 1, 1);

        tabAbbreviations_lstListAbbreviations = new QTableWidget(tabAbbreviations);
        if (tabAbbreviations_lstListAbbreviations->columnCount() < 2)
            tabAbbreviations_lstListAbbreviations->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tabAbbreviations_lstListAbbreviations->setHorizontalHeaderItem(0, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tabAbbreviations_lstListAbbreviations->setHorizontalHeaderItem(1, __qtablewidgetitem15);
        tabAbbreviations_lstListAbbreviations->setObjectName(QString::fromUtf8("tabAbbreviations_lstListAbbreviations"));

        gridLayout_10->addWidget(tabAbbreviations_lstListAbbreviations, 1, 0, 1, 1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        tabAbbreviations_cmdAdd = new QPushButton(tabAbbreviations);
        tabAbbreviations_cmdAdd->setObjectName(QString::fromUtf8("tabAbbreviations_cmdAdd"));

        horizontalLayout_9->addWidget(tabAbbreviations_cmdAdd);

        tabAbbreviations_cmdDel = new QPushButton(tabAbbreviations);
        tabAbbreviations_cmdDel->setObjectName(QString::fromUtf8("tabAbbreviations_cmdDel"));

        horizontalLayout_9->addWidget(tabAbbreviations_cmdDel);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_6);


        gridLayout_10->addLayout(horizontalLayout_9, 2, 0, 1, 1);

        tabWidget->addTab(tabAbbreviations, QString());
        tabLog = new TabBar();
        tabLog->setObjectName(QString::fromUtf8("tabLog"));
        tabLog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        tabLog_chkEnableLog = new QCheckBox(tabLog);
        tabLog_chkEnableLog->setObjectName(QString::fromUtf8("tabLog_chkEnableLog"));
        tabLog_chkEnableLog->setGeometry(QRect(10, 4, 521, 21));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(tabLog_chkEnableLog->sizePolicy().hasHeightForWidth());
        tabLog_chkEnableLog->setSizePolicy(sizePolicy);
        tabLog_chkEnableLog->setMaximumSize(QSize(16777215, 16777215));
        layoutWidget_1 = new QWidget(tabLog);
        layoutWidget_1->setObjectName(QString::fromUtf8("layoutWidget_1"));
        layoutWidget_1->setGeometry(QRect(10, 30, 521, 281));
        gridLayout_11 = new QGridLayout(layoutWidget_1);
        gridLayout_11->setObjectName(QString::fromUtf8("gridLayout_11"));
        lblSizeLog = new QLabel(layoutWidget_1);
        lblSizeLog->setObjectName(QString::fromUtf8("lblSizeLog"));
        lblSizeLog->setMaximumSize(QSize(9999, 22));

        gridLayout_11->addWidget(lblSizeLog, 0, 0, 1, 1);

        tabLog_spbSizeLog = new QSpinBox(layoutWidget_1);
        tabLog_spbSizeLog->setObjectName(QString::fromUtf8("tabLog_spbSizeLog"));
        tabLog_spbSizeLog->setMaximumSize(QSize(143, 25));
        tabLog_spbSizeLog->setMaximum(999999999);
        tabLog_spbSizeLog->setValue(1048576);

        gridLayout_11->addWidget(tabLog_spbSizeLog, 0, 1, 1, 1);

        lbSendLogEmail = new QLabel(layoutWidget_1);
        lbSendLogEmail->setObjectName(QString::fromUtf8("lbSendLogEmail"));
        lbSendLogEmail->setMaximumSize(QSize(9999, 16777215));

        gridLayout_11->addWidget(lbSendLogEmail, 1, 0, 1, 1);

        tabLog_txtSendLogEmail = new QLineEdit(layoutWidget_1);
        tabLog_txtSendLogEmail->setObjectName(QString::fromUtf8("tabLog_txtSendLogEmail"));
        tabLog_txtSendLogEmail->setMaximumSize(QSize(143, 25));

        gridLayout_11->addWidget(tabLog_txtSendLogEmail, 1, 1, 1, 1);

        lblSendLogHost = new QLabel(layoutWidget_1);
        lblSendLogHost->setObjectName(QString::fromUtf8("lblSendLogHost"));
        lblSendLogHost->setMaximumSize(QSize(9999, 16777215));

        gridLayout_11->addWidget(lblSendLogHost, 2, 0, 1, 1);

        tabLog_txtSendLogHost = new QLineEdit(layoutWidget_1);
        tabLog_txtSendLogHost->setObjectName(QString::fromUtf8("tabLog_txtSendLogHost"));
        tabLog_txtSendLogHost->setMaximumSize(QSize(143, 25));

        gridLayout_11->addWidget(tabLog_txtSendLogHost, 2, 1, 1, 1);

        lblSendLogPort = new QLabel(layoutWidget_1);
        lblSendLogPort->setObjectName(QString::fromUtf8("lblSendLogPort"));
        lblSendLogPort->setMaximumSize(QSize(9999, 16777215));

        gridLayout_11->addWidget(lblSendLogPort, 3, 0, 1, 1);

        tabLog_spbSendLogPort = new QSpinBox(layoutWidget_1);
        tabLog_spbSendLogPort->setObjectName(QString::fromUtf8("tabLog_spbSendLogPort"));
        tabLog_spbSendLogPort->setMaximumSize(QSize(143, 25));
        tabLog_spbSendLogPort->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        tabLog_spbSendLogPort->setMaximum(65536);
        tabLog_spbSendLogPort->setValue(25);

        gridLayout_11->addWidget(tabLog_spbSendLogPort, 3, 1, 1, 1);

        lblOpenLogIn = new QLabel(layoutWidget_1);
        lblOpenLogIn->setObjectName(QString::fromUtf8("lblOpenLogIn"));
        lblOpenLogIn->setMaximumSize(QSize(9999, 16777215));

        gridLayout_11->addWidget(lblOpenLogIn, 4, 0, 1, 1);

        tabLog_cmbOpenLongIn = new QComboBox(layoutWidget_1);
        tabLog_cmbOpenLongIn->setObjectName(QString::fromUtf8("tabLog_cmbOpenLongIn"));
        tabLog_cmbOpenLongIn->setMinimumSize(QSize(143, 0));
        tabLog_cmbOpenLongIn->setMaximumSize(QSize(143, 16777215));

        gridLayout_11->addWidget(tabLog_cmbOpenLongIn, 4, 1, 1, 1);

        tabLog_cmdOpenLog = new QPushButton(layoutWidget_1);
        tabLog_cmdOpenLog->setObjectName(QString::fromUtf8("tabLog_cmdOpenLog"));

        gridLayout_11->addWidget(tabLog_cmdOpenLog, 5, 1, 1, 1);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_11->addItem(horizontalSpacer_7, 5, 0, 1, 1);

        verticalSpacer_6 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_11->addItem(verticalSpacer_6, 6, 1, 1, 1);

        tabWidget->addTab(tabLog, QString());
        tabTroubleshooting = new TabBar();
        tabTroubleshooting->setObjectName(QString::fromUtf8("tabTroubleshooting"));
        tabTroubleshooting->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_12 = new QVBoxLayout(tabTroubleshooting);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        groupBox_5 = new QGroupBox(tabTroubleshooting);
        groupBox_5->setObjectName(QString::fromUtf8("groupBox_5"));
        gridLayout_12 = new QGridLayout(groupBox_5);
        gridLayout_12->setObjectName(QString::fromUtf8("gridLayout_12"));
        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        tabTroubleshooting_chkBackspace = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkBackspace->setObjectName(QString::fromUtf8("tabTroubleshooting_chkBackspace"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkBackspace);

        tabTroubleshooting_chkLeftArrow = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkLeftArrow->setObjectName(QString::fromUtf8("tabTroubleshooting_chkLeftArrow"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkLeftArrow);

        tabTroubleshooting_chkRightArrow = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkRightArrow->setObjectName(QString::fromUtf8("tabTroubleshooting_chkRightArrow"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkRightArrow);

        tabTroubleshooting_chkUpArrow = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkUpArrow->setObjectName(QString::fromUtf8("tabTroubleshooting_chkUpArrow"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkUpArrow);

        tabTroubleshooting_chkDownArrow = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkDownArrow->setObjectName(QString::fromUtf8("tabTroubleshooting_chkDownArrow"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkDownArrow);

        tabTroubleshooting_chkDelete = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkDelete->setObjectName(QString::fromUtf8("tabTroubleshooting_chkDelete"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkDelete);

        tabTroubleshooting_chkEnter = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkEnter->setObjectName(QString::fromUtf8("tabTroubleshooting_chkEnter"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkEnter);

        tabTroubleshooting_chkTab = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkTab->setObjectName(QString::fromUtf8("tabTroubleshooting_chkTab"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkTab);

        tabTroubleshooting_chkChangeLayout = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkChangeLayout->setObjectName(QString::fromUtf8("tabTroubleshooting_chkChangeLayout"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkChangeLayout);

        tabTroubleshooting_chkFullScreen = new QCheckBox(groupBox_5);
        tabTroubleshooting_chkFullScreen->setObjectName(QString::fromUtf8("tabTroubleshooting_chkFullScreen"));

        verticalLayout_10->addWidget(tabTroubleshooting_chkFullScreen);

        lblProperties = new QLabel(groupBox_5);
        lblProperties->setObjectName(QString::fromUtf8("lblProperties"));

        verticalLayout_10->addWidget(lblProperties);

        verticalSpacer_7 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout_10->addItem(verticalSpacer_7);


        gridLayout_12->addLayout(verticalLayout_10, 0, 0, 1, 1);


        verticalLayout_12->addWidget(groupBox_5);

        groupBox_6 = new QGroupBox(tabTroubleshooting);
        groupBox_6->setObjectName(QString::fromUtf8("groupBox_6"));
        gridLayout_13 = new QGridLayout(groupBox_6);
        gridLayout_13->setObjectName(QString::fromUtf8("gridLayout_13"));
        verticalLayout_11 = new QVBoxLayout();
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        tabTroubleshooting_chkFlushInterBuffEscape = new QCheckBox(groupBox_6);
        tabTroubleshooting_chkFlushInterBuffEscape->setObjectName(QString::fromUtf8("tabTroubleshooting_chkFlushInterBuffEscape"));

        verticalLayout_11->addWidget(tabTroubleshooting_chkFlushInterBuffEscape);

        tabTroubleshooting_chkFlushInterBuffEnterTab = new QCheckBox(groupBox_6);
        tabTroubleshooting_chkFlushInterBuffEnterTab->setObjectName(QString::fromUtf8("tabTroubleshooting_chkFlushInterBuffEnterTab"));

        verticalLayout_11->addWidget(tabTroubleshooting_chkFlushInterBuffEnterTab);

        tabTroubleshooting_chkCompatCompletion = new QCheckBox(groupBox_6);
        tabTroubleshooting_chkCompatCompletion->setObjectName(QString::fromUtf8("tabTroubleshooting_chkCompatCompletion"));

        verticalLayout_11->addWidget(tabTroubleshooting_chkCompatCompletion);


        gridLayout_13->addLayout(verticalLayout_11, 0, 0, 1, 1);


        verticalLayout_12->addWidget(groupBox_6);

        groupBox_7 = new QGroupBox(tabTroubleshooting);
        groupBox_7->setObjectName(QString::fromUtf8("groupBox_7"));
        gridLayout_15 = new QGridLayout(groupBox_7);
        gridLayout_15->setObjectName(QString::fromUtf8("gridLayout_15"));
        gridLayout_14 = new QGridLayout();
        gridLayout_14->setObjectName(QString::fromUtf8("gridLayout_14"));
        tabTroubleshooting_chkMonitorInput = new QCheckBox(groupBox_7);
        tabTroubleshooting_chkMonitorInput->setObjectName(QString::fromUtf8("tabTroubleshooting_chkMonitorInput"));

        gridLayout_14->addWidget(tabTroubleshooting_chkMonitorInput, 0, 0, 1, 1);

        lblProperties_2 = new QLabel(groupBox_7);
        lblProperties_2->setObjectName(QString::fromUtf8("lblProperties_2"));

        gridLayout_14->addWidget(lblProperties_2, 1, 0, 1, 1);

        tabTroubleshooting_chkMonitorMouse = new QCheckBox(groupBox_7);
        tabTroubleshooting_chkMonitorMouse->setObjectName(QString::fromUtf8("tabTroubleshooting_chkMonitorMouse"));

        gridLayout_14->addWidget(tabTroubleshooting_chkMonitorMouse, 2, 0, 1, 1);

        lblProperties_3 = new QLabel(groupBox_7);
        lblProperties_3->setObjectName(QString::fromUtf8("lblProperties_3"));
        lblProperties_3->setWordWrap(true);

        gridLayout_14->addWidget(lblProperties_3, 3, 0, 1, 1);


        gridLayout_15->addLayout(gridLayout_14, 0, 0, 1, 1);


        verticalLayout_12->addWidget(groupBox_7);

        verticalSpacer_11 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_12->addItem(verticalSpacer_11);

        tabWidget->addTab(tabTroubleshooting, QString());
        tabAdvanced = new TabBar();
        tabAdvanced->setObjectName(QString::fromUtf8("tabAdvanced"));
        tabAdvanced->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_18 = new QVBoxLayout(tabAdvanced);
        verticalLayout_18->setObjectName(QString::fromUtf8("verticalLayout_18"));
        gridLayout_16 = new QGridLayout();
        gridLayout_16->setObjectName(QString::fromUtf8("gridLayout_16"));
        lblDelay = new QLabel(tabAdvanced);
        lblDelay->setObjectName(QString::fromUtf8("lblDelay"));

        gridLayout_16->addWidget(lblDelay, 0, 0, 1, 1);

        tabAdvanced_spbDelay = new QSpinBox(tabAdvanced);
        tabAdvanced_spbDelay->setObjectName(QString::fromUtf8("tabAdvanced_spbDelay"));
        tabAdvanced_spbDelay->setMaximumSize(QSize(191, 25));
        tabAdvanced_spbDelay->setMaximum(50);

        gridLayout_16->addWidget(tabAdvanced_spbDelay, 0, 1, 1, 1);

        lblLogLevel = new QLabel(tabAdvanced);
        lblLogLevel->setObjectName(QString::fromUtf8("lblLogLevel"));

        gridLayout_16->addWidget(lblLogLevel, 1, 0, 1, 1);

        tabAdvanced_cmbLogLevel = new QComboBox(tabAdvanced);
        tabAdvanced_cmbLogLevel->setObjectName(QString::fromUtf8("tabAdvanced_cmbLogLevel"));
        tabAdvanced_cmbLogLevel->setMaximumSize(QSize(191, 25));

        gridLayout_16->addWidget(tabAdvanced_cmbLogLevel, 1, 1, 1, 1);


        verticalLayout_18->addLayout(gridLayout_16);

        lblKeyRelease = new QLabel(tabAdvanced);
        lblKeyRelease->setObjectName(QString::fromUtf8("lblKeyRelease"));

        verticalLayout_18->addWidget(lblKeyRelease);

        tabAdvanced_tabAppList = new QListWidget(tabAdvanced);
        tabAdvanced_tabAppList->setObjectName(QString::fromUtf8("tabAdvanced_tabAppList"));

        verticalLayout_18->addWidget(tabAdvanced_tabAppList);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_3);

        tabAdvanced_cmdAddApp = new QPushButton(tabAdvanced);
        tabAdvanced_cmdAddApp->setObjectName(QString::fromUtf8("tabAdvanced_cmdAddApp"));

        horizontalLayout_14->addWidget(tabAdvanced_cmdAddApp);

        tabAdvanced_cmdRemoveApp = new QPushButton(tabAdvanced);
        tabAdvanced_cmdRemoveApp->setObjectName(QString::fromUtf8("tabAdvanced_cmdRemoveApp"));

        horizontalLayout_14->addWidget(tabAdvanced_cmdRemoveApp);


        verticalLayout_18->addLayout(horizontalLayout_14);

        tabWidget->addTab(tabAdvanced, QString());
        tabPlugins = new TabBar();
        tabPlugins->setObjectName(QString::fromUtf8("tabPlugins"));
        tabPlugins->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        gridLayout_17 = new QGridLayout(tabPlugins);
        gridLayout_17->setObjectName(QString::fromUtf8("gridLayout_17"));
        tabPlugins_lstListPlugins = new QTableWidget(tabPlugins);
        if (tabPlugins_lstListPlugins->columnCount() < 3)
            tabPlugins_lstListPlugins->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tabPlugins_lstListPlugins->setHorizontalHeaderItem(0, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tabPlugins_lstListPlugins->setHorizontalHeaderItem(1, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tabPlugins_lstListPlugins->setHorizontalHeaderItem(2, __qtablewidgetitem18);
        tabPlugins_lstListPlugins->setObjectName(QString::fromUtf8("tabPlugins_lstListPlugins"));

        gridLayout_17->addWidget(tabPlugins_lstListPlugins, 0, 0, 1, 1);

        tabWidget->addTab(tabPlugins, QString());
        tabProperties = new TabBar();
        tabProperties->setObjectName(QString::fromUtf8("tabProperties"));
        tabProperties->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_15 = new QVBoxLayout(tabProperties);
        verticalLayout_15->setObjectName(QString::fromUtf8("verticalLayout_15"));
        groupBox_8 = new QGroupBox(tabProperties);
        groupBox_8->setObjectName(QString::fromUtf8("groupBox_8"));
        gridLayout_18 = new QGridLayout(groupBox_8);
        gridLayout_18->setObjectName(QString::fromUtf8("gridLayout_18"));
        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        tabProperties_chkEnableAutostart = new QCheckBox(groupBox_8);
        tabProperties_chkEnableAutostart->setObjectName(QString::fromUtf8("tabProperties_chkEnableAutostart"));

        verticalLayout_13->addWidget(tabProperties_chkEnableAutostart);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        lblDelayStartApp = new QLabel(groupBox_8);
        lblDelayStartApp->setObjectName(QString::fromUtf8("lblDelayStartApp"));
        lblDelayStartApp->setMinimumSize(QSize(300, 0));

        horizontalLayout_10->addWidget(lblDelayStartApp);

        tabProperties_spbDelayStartApp = new QSpinBox(groupBox_8);
        tabProperties_spbDelayStartApp->setObjectName(QString::fromUtf8("tabProperties_spbDelayStartApp"));
        tabProperties_spbDelayStartApp->setEnabled(false);
        tabProperties_spbDelayStartApp->setMaximumSize(QSize(61, 25));
        tabProperties_spbDelayStartApp->setAlignment(Qt::AlignCenter);
        tabProperties_spbDelayStartApp->setMaximum(6000);
        tabProperties_spbDelayStartApp->setValue(15);

        horizontalLayout_10->addWidget(tabProperties_spbDelayStartApp);


        verticalLayout_13->addLayout(horizontalLayout_10);


        gridLayout_18->addLayout(verticalLayout_13, 0, 0, 1, 1);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_18->addItem(horizontalSpacer_8, 0, 1, 1, 1);


        verticalLayout_15->addWidget(groupBox_8);

        groupBox_9 = new QGroupBox(tabProperties);
        groupBox_9->setObjectName(QString::fromUtf8("groupBox_9"));
        horizontalLayout_12 = new QHBoxLayout(groupBox_9);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        tabProperties_cmbTypeIconTray = new QComboBox(groupBox_9);
        tabProperties_cmbTypeIconTray->setObjectName(QString::fromUtf8("tabProperties_cmbTypeIconTray"));
        tabProperties_cmbTypeIconTray->setMinimumSize(QSize(240, 25));
        tabProperties_cmbTypeIconTray->setMaximumSize(QSize(240, 25));

        verticalLayout_14->addWidget(tabProperties_cmbTypeIconTray);

        tabProperties_grpFolderIcon = new QGroupBox(groupBox_9);
        tabProperties_grpFolderIcon->setObjectName(QString::fromUtf8("tabProperties_grpFolderIcon"));
        tabProperties_grpFolderIcon->setEnabled(false);
        gridLayout_19 = new QGridLayout(tabProperties_grpFolderIcon);
        gridLayout_19->setObjectName(QString::fromUtf8("gridLayout_19"));
        tabProperties_txtPathIconTray = new QLineEdit(tabProperties_grpFolderIcon);
        tabProperties_txtPathIconTray->setObjectName(QString::fromUtf8("tabProperties_txtPathIconTray"));

        gridLayout_19->addWidget(tabProperties_txtPathIconTray, 0, 0, 1, 1);

        tabProperties_cmdBrowseIconTray = new QPushButton(tabProperties_grpFolderIcon);
        tabProperties_cmdBrowseIconTray->setObjectName(QString::fromUtf8("tabProperties_cmdBrowseIconTray"));
        tabProperties_cmdBrowseIconTray->setMinimumSize(QSize(40, 25));
        tabProperties_cmdBrowseIconTray->setMaximumSize(QSize(40, 25));
        tabProperties_cmdBrowseIconTray->setText(QString::fromUtf8("..."));

        gridLayout_19->addWidget(tabProperties_cmdBrowseIconTray, 0, 1, 1, 1);


        verticalLayout_14->addWidget(tabProperties_grpFolderIcon);

        verticalSpacer_9 = new QSpacerItem(20, 25, QSizePolicy::Minimum, QSizePolicy::Minimum);

        verticalLayout_14->addItem(verticalSpacer_9);


        horizontalLayout_12->addLayout(verticalLayout_14);


        verticalLayout_15->addWidget(groupBox_9);

        groupBox_11 = new QGroupBox(tabProperties);
        groupBox_11->setObjectName(QString::fromUtf8("groupBox_11"));
        gridLayout_20 = new QGridLayout(groupBox_11);
        gridLayout_20->setObjectName(QString::fromUtf8("gridLayout_20"));
        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        lblKeyboardCommand = new QLabel(groupBox_11);
        lblKeyboardCommand->setObjectName(QString::fromUtf8("lblKeyboardCommand"));

        horizontalLayout_11->addWidget(lblKeyboardCommand);

        tabProperties_txtKeyboardCommad = new QLineEdit(groupBox_11);
        tabProperties_txtKeyboardCommad->setObjectName(QString::fromUtf8("tabProperties_txtKeyboardCommad"));

        horizontalLayout_11->addWidget(tabProperties_txtKeyboardCommad);

        tabProperties_cmdEditKeyCommand = new QPushButton(groupBox_11);
        tabProperties_cmdEditKeyCommand->setObjectName(QString::fromUtf8("tabProperties_cmdEditKeyCommand"));

        horizontalLayout_11->addWidget(tabProperties_cmdEditKeyCommand);

        tabProperties_cmdRecoverKeyCommand = new QPushButton(groupBox_11);
        tabProperties_cmdRecoverKeyCommand->setObjectName(QString::fromUtf8("tabProperties_cmdRecoverKeyCommand"));

        horizontalLayout_11->addWidget(tabProperties_cmdRecoverKeyCommand);


        gridLayout_20->addLayout(horizontalLayout_11, 0, 0, 1, 1);


        verticalLayout_15->addWidget(groupBox_11);

        verticalSpacer_12 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_15->addItem(verticalSpacer_12);

        tabWidget->addTab(tabProperties, QString());

        verticalLayout->addWidget(tabWidget);

        cmdBox = new QDialogButtonBox(frmSettings);
        cmdBox->setObjectName(QString::fromUtf8("cmdBox"));
        cmdBox->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        cmdBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(cmdBox);


        retranslateUi(frmSettings);

        tabWidget->setCurrentIndex(9);
        tbWidGeneral->setCurrentIndex(1);
        tbWidNotifications->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(frmSettings);
    } // setupUi

    void retranslateUi(QDialog *frmSettings)
    {
        frmSettings->setWindowTitle(QApplication::translate("frmSettings", "XNeur Settings", 0, QApplication::UnicodeUTF8));
        chkGenMain_ManualSwitch->setText(QApplication::translate("frmSettings", "Use manual switching mode", 0, QApplication::UnicodeUTF8));
        chkGenMain_AutoLearning->setText(QApplication::translate("frmSettings", "Enable auto-learning mode", 0, QApplication::UnicodeUTF8));
        chkGenMain_KeepSelect->setText(QApplication::translate("frmSettings", "Keep selection after changing selected text", 0, QApplication::UnicodeUTF8));
        chkGenMain_RotateLayout->setText(QApplication::translate("frmSettings", "Rotate layout after changing selected text", 0, QApplication::UnicodeUTF8));
        chkGenMain_CheckLang->setText(QApplication::translate("frmSettings", "Check language on input process", 0, QApplication::UnicodeUTF8));
        chkGenMain_CheckSimilar->setText(QApplication::translate("frmSettings", "Check similar words", 0, QApplication::UnicodeUTF8));
        tbWidGeneral->setTabText(tbWidGeneral->indexOf(tabMain), QApplication::translate("frmSettings", "Main", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_CeorrectCAPS->setText(QApplication::translate("frmSettings", "Correct aCCIDENTAL caps", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_DisableCaps->setText(QApplication::translate("frmSettings", "Disable CapsLock use", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_CorrectTwoCaps->setText(QApplication::translate("frmSettings", "Correct TWo INitial caps", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_CorrectSpace->setText(QApplication::translate("frmSettings", "Correct spaces with punctuation", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_CorrectSmallLitter->setText(QApplication::translate("frmSettings", "\320\241orrect small letter to capital letter after dot", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_CorrectTwoSpase->setText(QApplication::translate("frmSettings", "\320\241orrect two space with a comma and a space", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_CorrectTwoMinus->setText(QApplication::translate("frmSettings", "\320\241orrect two minus with a dash", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_CorrectDash->setText(QApplication::translate("frmSettings", "\320\241orrect dash with a emdash", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_Correct_c_->setText(QApplication::translate("frmSettings", "\320\241orrect (c) with a copyright sign", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_Correct_tm_->setText(QApplication::translate("frmSettings", "\320\241orrect (tm) with a trademark sign", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_Correct_r_->setText(QApplication::translate("frmSettings", "\320\241orrect (r) with a registered sign", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_Correct_Three_Point->setText(QApplication::translate("frmSettings", "\320\241orrect three points with a ellipsis sign", 0, QApplication::UnicodeUTF8));
        chkGenTipograph_Correct_Misprint->setText(QApplication::translate("frmSettings", "Correct misprint", 0, QApplication::UnicodeUTF8));
        tbWidGeneral->setTabText(tbWidGeneral->indexOf(tabTipographics), QApplication::translate("frmSettings", "Tipographics", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabGeneral), QApplication::translate("frmSettings", "General", 0, QApplication::UnicodeUTF8));
        Layout_lblLayoutNomber->setText(QApplication::translate("frmSettings", "Initial layout number:", 0, QApplication::UnicodeUTF8));
        Layout_cmdRulesChange->setText(QApplication::translate("frmSettings", "Rules change...", 0, QApplication::UnicodeUTF8));
        Layout_chkRememberKbLayout->setText(QApplication::translate("frmSettings", "Remember keyboard layout for each application instance", 0, QApplication::UnicodeUTF8));
        groupBox_12->setTitle(QApplication::translate("frmSettings", "Exceptions (One keyboard layout for all application instances)", 0, QApplication::UnicodeUTF8));
        Layout_AddApp->setText(QApplication::translate("frmSettings", "Add", 0, QApplication::UnicodeUTF8));
        Layout_DelApp->setText(QApplication::translate("frmSettings", "Remove", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabLayouts), QApplication::translate("frmSettings", "Layouts", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tabHotKey_lstHotKey->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("frmSettings", "Actions", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tabHotKey_lstHotKey->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("frmSettings", "HotKeys", 0, QApplication::UnicodeUTF8));
        tabHotKeys_EditHotKey->setText(QApplication::translate("frmSettings", "Edit", 0, QApplication::UnicodeUTF8));
        tabHotKeys_ClearHotKey->setText(QApplication::translate("frmSettings", "Clear", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tabHotKey_lstUserActions->horizontalHeaderItem(0);
        ___qtablewidgetitem2->setText(QApplication::translate("frmSettings", "Action", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tabHotKey_lstUserActions->horizontalHeaderItem(1);
        ___qtablewidgetitem3->setText(QApplication::translate("frmSettings", "HotKeys", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem4 = tabHotKey_lstUserActions->horizontalHeaderItem(2);
        ___qtablewidgetitem4->setText(QApplication::translate("frmSettings", "User Actions", 0, QApplication::UnicodeUTF8));
        tabHotKeys_UserActionsAdd->setText(QApplication::translate("frmSettings", "Add", 0, QApplication::UnicodeUTF8));
        tabHotKeys_UserActionsEdit->setText(QApplication::translate("frmSettings", "Edit", 0, QApplication::UnicodeUTF8));
        tabHotKeys_UserActionsDel->setText(QApplication::translate("frmSettings", "Remove", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabHotKeys), QApplication::translate("frmSettings", "HotKeys", 0, QApplication::UnicodeUTF8));
        tabAutocompletion_chkAddSpace->setText(QApplication::translate("frmSettings", "Add space after autocompletion", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("frmSettings", "Disable the autocompletion for these applications", 0, QApplication::UnicodeUTF8));
        tabAutocompletion_cmdAddApp->setText(QApplication::translate("frmSettings", "Add", 0, QApplication::UnicodeUTF8));
        tabAutocompletion_cmdDelApp->setText(QApplication::translate("frmSettings", "Remove", 0, QApplication::UnicodeUTF8));
        tabAutocompletion_chkTrueAutocomplit->setText(QApplication::translate("frmSettings", "Enable autocompletion", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabAutocompletion), QApplication::translate("frmSettings", "Autocompletion", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("frmSettings", "Ignore these applications", 0, QApplication::UnicodeUTF8));
        taApplication_cmdAdd_NotUsed->setText(QApplication::translate("frmSettings", "Add", 0, QApplication::UnicodeUTF8));
        taApplication_cmdDel_NotUsed->setText(QApplication::translate("frmSettings", "Remove", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("frmSettings", "Force automatic mode for these applications", 0, QApplication::UnicodeUTF8));
        taApplication_cmdAdd_AutoMode->setText(QApplication::translate("frmSettings", "Add", 0, QApplication::UnicodeUTF8));
        taApplication_cmdDel_AutoMode->setText(QApplication::translate("frmSettings", "Remove", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("frmSettings", "Force manual mode for these applications", 0, QApplication::UnicodeUTF8));
        taApplication_cmdAdd_ManualMode->setText(QApplication::translate("frmSettings", "Add", 0, QApplication::UnicodeUTF8));
        taApplication_cmdDel_ManualMode->setText(QApplication::translate("frmSettings", "Remove", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabApplications), QApplication::translate("frmSettings", "Applications", 0, QApplication::UnicodeUTF8));
        tabSound_chkEnableSound->setText(QApplication::translate("frmSettings", "Enable sounds", 0, QApplication::UnicodeUTF8));
        lblSoundVolume->setText(QApplication::translate("frmSettings", "Sound volume:  ", 0, QApplication::UnicodeUTF8));
        tabSound_cmdEdit->setText(QApplication::translate("frmSettings", "Edit", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem5 = tabSound_lstListSound->horizontalHeaderItem(0);
        ___qtablewidgetitem5->setText(QApplication::translate("frmSettings", "Action", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem6 = tabSound_lstListSound->horizontalHeaderItem(1);
        ___qtablewidgetitem6->setText(QApplication::translate("frmSettings", "Sound", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem7 = tabSound_lstListSound->horizontalHeaderItem(2);
        ___qtablewidgetitem7->setText(QApplication::translate("frmSettings", "Enabled", 0, QApplication::UnicodeUTF8));
        tbWidNotifications->setTabText(tbWidNotifications->indexOf(tabSound), QApplication::translate("frmSettings", "Sounds", 0, QApplication::UnicodeUTF8));
        tabOSD_chkEnableOSD->setText(QApplication::translate("frmSettings", "Show OSD", 0, QApplication::UnicodeUTF8));
        lblFontOSD->setText(QApplication::translate("frmSettings", "OSD Font:  ", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("frmSettings", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Use command <span style=\" font-weight:600;\">xfontsel -print</span> to get new font.</p></body></html>", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem8 = tabOSD_lstListOSD->horizontalHeaderItem(0);
        ___qtablewidgetitem8->setText(QApplication::translate("frmSettings", "Action", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem9 = tabOSD_lstListOSD->horizontalHeaderItem(1);
        ___qtablewidgetitem9->setText(QApplication::translate("frmSettings", "OSD text", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem10 = tabOSD_lstListOSD->horizontalHeaderItem(2);
        ___qtablewidgetitem10->setText(QApplication::translate("frmSettings", "Enabled", 0, QApplication::UnicodeUTF8));
        tbWidNotifications->setTabText(tbWidNotifications->indexOf(tabOSD), QApplication::translate("frmSettings", "OSD", 0, QApplication::UnicodeUTF8));
        tabPopupMessage_chkShowPopupMessage->setText(QApplication::translate("frmSettings", "Show popup messages", 0, QApplication::UnicodeUTF8));
        lblInterval->setText(QApplication::translate("frmSettings", "Popup expiration interval (in milliseconds)", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem11 = tabPopupMessage_lstListPopupMessage->horizontalHeaderItem(0);
        ___qtablewidgetitem11->setText(QApplication::translate("frmSettings", "Action", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem12 = tabPopupMessage_lstListPopupMessage->horizontalHeaderItem(1);
        ___qtablewidgetitem12->setText(QApplication::translate("frmSettings", "Popup message text", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem13 = tabPopupMessage_lstListPopupMessage->horizontalHeaderItem(2);
        ___qtablewidgetitem13->setText(QApplication::translate("frmSettings", "Enabled", 0, QApplication::UnicodeUTF8));
        tbWidNotifications->setTabText(tbWidNotifications->indexOf(tabPopupMessage), QApplication::translate("frmSettings", "Popup messages", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabNotifications), QApplication::translate("frmSettings", "Notifications", 0, QApplication::UnicodeUTF8));
        tabAbbreviations_chkIgnoreKeyLayout->setText(QApplication::translate("frmSettings", "Ignore keyboard layout for abbreviations", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem14 = tabAbbreviations_lstListAbbreviations->horizontalHeaderItem(0);
        ___qtablewidgetitem14->setText(QApplication::translate("frmSettings", "Abbreviation", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem15 = tabAbbreviations_lstListAbbreviations->horizontalHeaderItem(1);
        ___qtablewidgetitem15->setText(QApplication::translate("frmSettings", "Expansion text", 0, QApplication::UnicodeUTF8));
        tabAbbreviations_cmdAdd->setText(QApplication::translate("frmSettings", "Add", 0, QApplication::UnicodeUTF8));
        tabAbbreviations_cmdDel->setText(QApplication::translate("frmSettings", "Remove", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabAbbreviations), QApplication::translate("frmSettings", "Abbreviations", 0, QApplication::UnicodeUTF8));
        tabLog_chkEnableLog->setText(QApplication::translate("frmSettings", "Enable keyboard logging", 0, QApplication::UnicodeUTF8));
        lblSizeLog->setText(QApplication::translate("frmSettings", "Log file size in bytes", 0, QApplication::UnicodeUTF8));
        lbSendLogEmail->setText(QApplication::translate("frmSettings", "Send log to e-mail", 0, QApplication::UnicodeUTF8));
        lblSendLogHost->setText(QApplication::translate("frmSettings", "Send log via host (name or IP)", 0, QApplication::UnicodeUTF8));
        lblSendLogPort->setText(QApplication::translate("frmSettings", "Send log via port", 0, QApplication::UnicodeUTF8));
        lblOpenLogIn->setText(QApplication::translate("frmSettings", "Open log file in", 0, QApplication::UnicodeUTF8));
        tabLog_cmbOpenLongIn->clear();
        tabLog_cmbOpenLongIn->insertItems(0, QStringList()
         << QApplication::translate("frmSettings", "Open in browser", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Opening in the internal viewer", 0, QApplication::UnicodeUTF8)
        );
        tabLog_cmdOpenLog->setText(QApplication::translate("frmSettings", "View log...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabLog), QApplication::translate("frmSettings", "Log", 0, QApplication::UnicodeUTF8));
        groupBox_5->setTitle(QApplication::translate("frmSettings", "Don't switch layout if before input where pressed", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkBackspace->setText(QApplication::translate("frmSettings", "Backspace", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkLeftArrow->setText(QApplication::translate("frmSettings", "Left arrow", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkRightArrow->setText(QApplication::translate("frmSettings", "Right arrow", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkUpArrow->setText(QApplication::translate("frmSettings", "Up arrow", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkDownArrow->setText(QApplication::translate("frmSettings", "Down arrow", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkDelete->setText(QApplication::translate("frmSettings", "Delete", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkEnter->setText(QApplication::translate("frmSettings", "Enter", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkTab->setText(QApplication::translate("frmSettings", "Tab", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkChangeLayout->setText(QApplication::translate("frmSettings", "You changed layout", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkFullScreen->setText(QApplication::translate("frmSettings", "Application in full screen mode", 0, QApplication::UnicodeUTF8));
        lblProperties->setText(QApplication::translate("frmSettings", "This will help avoid switching layout when editing parts previously introduced word.", 0, QApplication::UnicodeUTF8));
        groupBox_6->setTitle(QApplication::translate("frmSettings", "Additional", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkFlushInterBuffEscape->setText(QApplication::translate("frmSettings", "Flush internal buffer on Escape", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkFlushInterBuffEnterTab->setText(QApplication::translate("frmSettings", "Flush internal buffer on Enter or Tab", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkCompatCompletion->setText(QApplication::translate("frmSettings", "Compatibility with the completion", 0, QApplication::UnicodeUTF8));
        groupBox_7->setTitle(QApplication::translate("frmSettings", "Tracking input", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkMonitorInput->setText(QApplication::translate("frmSettings", "Monitor the input", 0, QApplication::UnicodeUTF8));
        lblProperties_2->setText(QApplication::translate("frmSettings", "Disabling this option will add any application to the list of excluded applications.", 0, QApplication::UnicodeUTF8));
        tabTroubleshooting_chkMonitorMouse->setText(QApplication::translate("frmSettings", "Monitor the mouse", 0, QApplication::UnicodeUTF8));
        lblProperties_3->setText(QApplication::translate("frmSettings", "Disabling this option will disable mouse tracking. Reset buffer when clicking the mouse will not.", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabTroubleshooting), QApplication::translate("frmSettings", "Troubleshooting", 0, QApplication::UnicodeUTF8));
        lblDelay->setText(QApplication::translate("frmSettings", "Delay before sending events (in milliseconds)", 0, QApplication::UnicodeUTF8));
        lblLogLevel->setText(QApplication::translate("frmSettings", "Log verbosity level", 0, QApplication::UnicodeUTF8));
        tabAdvanced_cmbLogLevel->clear();
        tabAdvanced_cmbLogLevel->insertItems(0, QStringList()
         << QApplication::translate("frmSettings", "None", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Error", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Warning", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Log", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Debug", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Trace", 0, QApplication::UnicodeUTF8)
        );
        lblKeyRelease->setText(QApplication::translate("frmSettings", "Don't send event KeyRelease in these applications:", 0, QApplication::UnicodeUTF8));
        tabAdvanced_cmdAddApp->setText(QApplication::translate("frmSettings", "Add", 0, QApplication::UnicodeUTF8));
        tabAdvanced_cmdRemoveApp->setText(QApplication::translate("frmSettings", "Remove", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabAdvanced), QApplication::translate("frmSettings", "Advanced", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem16 = tabPlugins_lstListPlugins->horizontalHeaderItem(0);
        ___qtablewidgetitem16->setText(QApplication::translate("frmSettings", "Enabled", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem17 = tabPlugins_lstListPlugins->horizontalHeaderItem(1);
        ___qtablewidgetitem17->setText(QApplication::translate("frmSettings", "Description", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem18 = tabPlugins_lstListPlugins->horizontalHeaderItem(2);
        ___qtablewidgetitem18->setText(QApplication::translate("frmSettings", "File name", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabPlugins), QApplication::translate("frmSettings", "Plugins", 0, QApplication::UnicodeUTF8));
        groupBox_8->setTitle(QApplication::translate("frmSettings", "Startup", 0, QApplication::UnicodeUTF8));
        tabProperties_chkEnableAutostart->setText(QApplication::translate("frmSettings", "Autostartup", 0, QApplication::UnicodeUTF8));
        lblDelayStartApp->setText(QApplication::translate("frmSettings", "Delay before startup", 0, QApplication::UnicodeUTF8));
        groupBox_9->setTitle(QApplication::translate("frmSettings", "UI", 0, QApplication::UnicodeUTF8));
        tabProperties_cmbTypeIconTray->clear();
        tabProperties_cmbTypeIconTray->insertItems(0, QStringList()
         << QApplication::translate("frmSettings", "Show flag", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Show user flag", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Show text", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("frmSettings", "Show icon", 0, QApplication::UnicodeUTF8)
        );
        tabProperties_grpFolderIcon->setTitle(QApplication::translate("frmSettings", "Folder icon indicator", 0, QApplication::UnicodeUTF8));
        groupBox_11->setTitle(QApplication::translate("frmSettings", "System", 0, QApplication::UnicodeUTF8));
        lblKeyboardCommand->setText(QApplication::translate("frmSettings", "Keyboard properties command: ", 0, QApplication::UnicodeUTF8));
        tabProperties_txtKeyboardCommad->setText(QApplication::translate("frmSettings", "/usr/bin/kcmshell4 --args=--tab=layouts kcm_keyboard", 0, QApplication::UnicodeUTF8));
        tabProperties_cmdEditKeyCommand->setText(QApplication::translate("frmSettings", "Edit", 0, QApplication::UnicodeUTF8));
        tabProperties_cmdRecoverKeyCommand->setText(QApplication::translate("frmSettings", "Recover", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tabProperties), QApplication::translate("frmSettings", "Properties", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class frmSettings: public Ui_frmSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRMSETTINGS_H
