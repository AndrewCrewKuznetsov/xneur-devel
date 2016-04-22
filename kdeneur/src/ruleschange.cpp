#include "ruleschange.h"
#include "ui_ruleschange.h"
#include "addrules.h"

kXneurApp::RulesChange::RulesChange(QStringList text, QWidget *parent) :   QDialog(parent),   ui(new Ui::RulesChange)
{
    ui->setupUi(this);
    createConnect();
    savePath = text.at(0);
    text.removeAt(0);
    conditions_names << tr("contains") <<tr("begins") << tr("ends") << tr("coincides");
    parserDict(text);
}

kXneurApp::RulesChange::~RulesChange()
{
    delete ui;
}

void kXneurApp::RulesChange::createConnect()
{
    connect(ui->cmdAdd, SIGNAL(clicked()),SLOT(addWords()));
    connect(ui->cmdEdit, SIGNAL(clicked()),SLOT(editWors()));
    connect(ui->cmdRemove, SIGNAL(clicked()), SLOT(delWords()));
    connect(ui->cmdClose, SIGNAL(clicked()), SLOT(closeForm()));
    connect(ui->cmdSave, SIGNAL(clicked()), SLOT(saveDict()));
}

void kXneurApp::RulesChange::addWords()
{
    kXneurApp::AddRules *frm = new kXneurApp::AddRules(0);
    if(frm->exec()==QDialog::Accepted)
    {
        int i = ui->listWords->rowCount();
        ui->listWords->insertRow(i);
        ui->listWords->setItem(i,0, new QTableWidgetItem(frm->words));
        ui->listWords->setItem(i,1, new QTableWidgetItem(frm->conditions));
        QTableWidgetItem *itm = new QTableWidgetItem();
        (frm->chkRegistr) ? itm->setCheckState(Qt::Checked) :itm->setCheckState(Qt::Unchecked);
        ui->listWords->setItem(i,2, itm);
    }
    delete frm;
}

void kXneurApp::RulesChange::editWors()
{
    int currentRow = ui->listWords->currentRow();
    if (currentRow < 0)
    {
        QMessageBox::information(0, tr("Warning...."), tr("You don't select rules"), QMessageBox::Ok);
    }
    else
    {
        QString word = ui->listWords->item(currentRow,0)->text();
        QString condit = ui->listWords->item(currentRow,1)->text();
        bool reg;
        (ui->listWords->item(currentRow,2)->checkState()==Qt::Unchecked)?
                    reg = false:
                    reg = true;
        kXneurApp::AddRules *frm = new kXneurApp::AddRules(word, reg, condit);
        if(frm->exec()==QDialog::Accepted)
        {
            ui->listWords->item(currentRow, 0)->setText(frm->words);
            ui->listWords->item(currentRow, 1)->setText(frm->conditions);
            (frm->chkRegistr)? ui->listWords->item(currentRow, 2)->setCheckState(Qt::Checked):
                               ui->listWords->item(currentRow, 2)->setCheckState(Qt::Unchecked);
        }
        delete frm;
    }

}

void kXneurApp::RulesChange::delWords()
{
    ui->listWords->removeRow(ui->listWords->currentRow());
}

void kXneurApp::RulesChange::closeForm()
{
    this->close();
}

void kXneurApp::RulesChange::saveDict()
{
    QFile fileDict(savePath);
    if(fileDict.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString chek, word, begins, ends;
        chek=word=begins=ends="";
        QTextStream dic(&fileDict);
        for(int i=0; i< ui->listWords->rowCount();++i)
        {
            word = ui->listWords->item(i,0)->text();
            if(ui->listWords->item(i,2)->checkState()==Qt::Checked)
            {
                chek="(?i)";
            }
            if(ui->listWords->item(i,1)->text()==conditions_names[1])
            {
                begins="^";
            }
            else if(ui->listWords->item(i,1)->text()==conditions_names[2])
            {
                ends="$";
            }
            else if(ui->listWords->item(i,1)->text()==conditions_names[3])
            {
                 ends="$";
                 begins="^";
            }
            dic << QString("%1%2%3%4\n").arg(chek).arg(begins).arg(word).arg(ends);
            chek=word=begins=ends="";
        }
        fileDict.close();
    }
    else
    {
        qDebug()<< "ERR: " << fileDict.errorString();
    }
    this->close();
}

void kXneurApp::RulesChange::parserDict(QStringList txt)
{
    ui->listWords->verticalHeader()->setDefaultSectionSize(22);
    ui->listWords->verticalHeader()->hide();
    ui->listWords->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
    ui->listWords->horizontalHeader()->setResizeMode(1,QHeaderView::ResizeToContents);
    ui->listWords->horizontalHeader()->setResizeMode(2,QHeaderView::ResizeToContents);
    ui->listWords->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWords->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listWords->setEditTriggers(QAbstractItemView::NoEditTriggers);
    if(txt.size()>0)
    {

        QTableWidgetItem * itm ;
        ui->listWords->setRowCount(txt.size());
        for(int i=0; i<txt.size();++i)
        {
            QString tmpStr = txt.at(i);
            itm = new QTableWidgetItem();
            if(tmpStr.startsWith("(?i)"))
            {
                itm->setCheckState(Qt::Checked);
                tmpStr.remove(0,4);
            }
            else
            {itm->setCheckState(Qt::Unchecked);}
            ui->listWords->setItem(i,2, itm);
            if(tmpStr.startsWith("^") && tmpStr.endsWith("$"))
            {
                tmpStr.remove(0,1);
                tmpStr.remove(tmpStr.length()-1,1);
                ui->listWords->setItem(i,1, new QTableWidgetItem(conditions_names.at(3)));
            }
            else if (tmpStr.startsWith("^"))
            {
                tmpStr.remove(0,1);
                ui->listWords->setItem(i,1, new QTableWidgetItem(conditions_names.at(1)));
            }
            else if(tmpStr.endsWith("$"))
            {
                tmpStr.remove(tmpStr.length()-1,1);
                ui->listWords->setItem(i,1, new QTableWidgetItem(conditions_names.at(2)));
            }
            else
            {
                ui->listWords->setItem(i,1, new QTableWidgetItem(conditions_names.at(0)));
            }
            ui->listWords->setItem(i,0, new QTableWidgetItem(tmpStr));
        }
    }
}
