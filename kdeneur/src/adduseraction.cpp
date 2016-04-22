#include "adduseraction.h"
#include "ui_adduseraction.h"
#include "xneurconfig.h"

extern "C"
{
#include <X11/XKBlib.h>
}

kXneurApp::addUserAction::addUserAction(QString nm, QString key, QString cmd, QWidget *parent) :QDialog(parent),ui(new Ui::addUserAction)
{
    ui->setupUi(this);
    ui->txtActionName->setText(nm);
    ui->lblHotKey->setText(key);
    ui->txtCommandAction->setText(cmd);
    connect(ui->cmdSave, SIGNAL(clicked()), SLOT(saveAction()));
    connect(ui->cmdCancel,SIGNAL(clicked()),SLOT(cancelAction()));
}


kXneurApp::addUserAction::~addUserAction()
{
    delete ui;
}

void kXneurApp::addUserAction::saveAction()
{
    if(!ui->txtActionName->text().trimmed().isEmpty() && !ui->lblHotKey->text().trimmed().isEmpty() && !ui->txtCommandAction->text().trimmed().isEmpty())
    {
        done(Accepted);
        name=ui->txtActionName->text();
        hot_key= ui->lblHotKey->text();
        command = ui->txtCommandAction->text();
        this->close();
    }
}

void kXneurApp::addUserAction::cancelAction()
{
    done(Rejected);
    this->close();
}

void kXneurApp::addUserAction::keyPressEvent(QKeyEvent *event)
{
    QString key;
    QString md_key;
    key = QString("%1").arg(XKeysymToString(XkbKeycodeToKeysym(kXneurApp::xNeurConfig::dpy, event->nativeScanCode(),0,0)));
    md_key = modif(event->modifiers());
    if(key.endsWith("_L") ||  key.endsWith("_R") )
        key.remove(key.length()-2,2);
    ui->lblHotKey->setText(QString("%1%2").arg(md_key).arg(key));
}

void kXneurApp::addUserAction::keyReleaseEvent(QKeyEvent *)
{
    QStringList lst;
    QString keys;
    lst = ui->lblHotKey->text().replace(" ","").split("+");
    lst.removeDuplicates();
    for(int i=0; i<lst.size();++i)
    {
        keys+=QString("%1 + ").arg(lst.at(i));
    }
    ui->lblHotKey->clear();
    ui->lblHotKey->setText(keys.trimmed().remove(keys.length()-2,2));
}

QString kXneurApp::addUserAction::modif(int value)
{
    QString key="";
    switch(value)
    {
    case Qt::ShiftModifier:
        key = QString("Shift + ");
       break;
    case Qt::ControlModifier:
        key = QString("Control + ");
       break;
    case Qt::AltModifier:
        key = QString("Alt + ");
       break;
    case Qt::MetaModifier:
        key = QString("Super + ");
       break;
    case Qt::ControlModifier+Qt::ShiftModifier:
        key = QString("Control + Shift + ");
        break;
    case Qt::ControlModifier+Qt::AltModifier:
        key = QString("Control + Alt + ");
        break;
    case Qt::ControlModifier+Qt::MetaModifier:
        key = QString("Control + Super + ");
        break;
    case Qt::AltModifier+Qt::ShiftModifier:
        key = QString("Shift + Alt + ");
        break;
    case Qt::AltModifier+Qt::MetaModifier:
        key = QString("Super + Alt + ");
        break;
    case Qt::ShiftModifier+Qt::MetaModifier:
        key = QString("Shift + Super + ");
        break;
    case Qt::ControlModifier+Qt::ShiftModifier+Qt::AltModifier:
        key = QString("Control + Shift + Alt + ");
        break;
    case Qt::MetaModifier+Qt::ShiftModifier+Qt::AltModifier:
        key = QString("Super + Shift + Alt + ");
        break;
    case Qt::MetaModifier+Qt::ControlModifier+Qt::AltModifier:
        key = QString("Super + Control + Alt + ");
        break;
    case Qt::ControlModifier+Qt::ShiftModifier+Qt::MetaModifier:
        key = QString("Super + Shift + Control + ");
        break;
    case Qt::ControlModifier+Qt::ShiftModifier+Qt::MetaModifier+Qt::AltModifier:
        key = QString("Super + Shift + Control + Alt + ");
        break;
    }
    return key;
}

