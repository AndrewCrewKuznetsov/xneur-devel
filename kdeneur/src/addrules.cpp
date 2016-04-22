#include "addrules.h"
#include "ui_addrules.h"


kXneurApp::AddRules::AddRules(QWidget *parent) :  QDialog(parent), ui(new Ui::AddRules)
{
    ui->setupUi(this);
    edit=false;
    settings();
}

kXneurApp::AddRules::AddRules(QString word, bool reg,QString condit, QWidget *parent) : QDialog(parent), ui(new Ui::AddRules)
{
    ui->setupUi(this);
    edit=true;
    settings();
    ui->txtWords->setText(word);
    ui->chkInsensitive->setChecked(reg);
    if(condit==tr("contains"))
        ui->rdbComtaint->setChecked(true);
    else if(condit==tr("begins"))
        ui->rdbStart->setChecked(true);
    else if(condit==tr("ends"))
        ui->rdbEnd->setChecked(true);
    else
        ui->rdbCoincide->setChecked(true);
}

kXneurApp::AddRules::~AddRules()
{
    delete ui;
}

void kXneurApp::AddRules::saveData()
{
    done(Accepted);
    words = ui->txtWords->text();
    chkRegistr = ui->chkInsensitive->isChecked();
    if(ui->rdbComtaint->isChecked())
        conditions=tr("contains");
    else if(ui->rdbStart->isChecked())
        conditions=tr("begins");
    else if(ui->rdbEnd->isChecked())
        conditions=tr("ends");
    else
        conditions=tr("coincides");
    close();
}

void kXneurApp::AddRules::closeForm()
{
    done(Rejected);
    close();
}

void kXneurApp::AddRules::settings()
{
    chkRegistr=false;
    conditions=words="";
    connect(ui->cmdBox, SIGNAL(accepted()),SLOT(saveData()));
    connect(ui->cmdBox, SIGNAL(rejected()),SLOT(closeForm()));
}
