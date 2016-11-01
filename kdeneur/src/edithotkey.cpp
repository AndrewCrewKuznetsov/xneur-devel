#include "edithotkey.h"
#include "ui_edithotkey.h"
#include "xneurconfig.h"
#include "kdeneur.h"

extern "C"
{
#include <X11/XKBlib.h>
}


kXneurApp::EditHotKey::EditHotKey(QWidget *parent, QString action, QString key) : QDialog(parent), ui(new Ui::EditHotKey)
{
    ui->setupUi(this);

    if (action != NULL){};
    connect(ui->cmdCancel, SIGNAL(clicked()),SLOT(closeForm()));
    connect(ui->cmdSave,SIGNAL(clicked()),SLOT(save()));
    xNeurConfig *cfgXneur = new kXneurApp::xNeurConfig();
    for (int i = 0; i < cfgXneur->lstCommand_hotKey.size(); i++)
    {
      ui->comboAction->addItem(cfgXneur->lstCommand_hotKey[i]); ;//->setText(action);
    }
    ui->comboAction->setCurrentIndex(ui->comboAction->findText(action));
    ui->lblHotKey->setText(key);
    ui->lblHotKey->setFocus();
}

kXneurApp::EditHotKey::~EditHotKey()
{
    delete ui;
}

void kXneurApp::EditHotKey::keyPressEvent(QKeyEvent *event)
{
    QString key;
    QString md_key;
    key = QString("%1").arg(XKeysymToString(XkbKeycodeToKeysym(kXneurApp::xNeurConfig::dpy, event->nativeScanCode(),0,0)));
    md_key = modif(event->modifiers());
    if(key.endsWith("_L") ||  key.endsWith("_R") )
        key.remove(key.length()-2,2);
    ui->lblHotKey->setText(QString("%1%2").arg(md_key).arg(key));
}

void kXneurApp::EditHotKey::keyReleaseEvent(QKeyEvent *)
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

QString kXneurApp::EditHotKey::modif(int value)
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

void kXneurApp::EditHotKey::save()
{
    done(Accepted);
    hot_keys = ui->lblHotKey->text();
    action_name = ui->comboAction->currentText();
    this->close();
}

void kXneurApp::EditHotKey::closeForm()
{
    done(Rejected);
    this->close();
}
