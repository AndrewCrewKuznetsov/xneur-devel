#include "frmabout.h"
#include "ui_frmabout.h"
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <QDebug>
#include <kglobal.h>
kXneurApp::frmAbout::frmAbout(QWidget *parent) : QDialog(parent),  ui(new Ui::frmAbout)
{
  ui->setupUi(this);
  setAttribute( Qt::WA_DeleteOnClose, true);
  connect(ui->cmdClose, SIGNAL(clicked()), SLOT(closeForm()));
  ui->lblCurrentVersion->setText(ui->lblCurrentVersion->text().replace("%1", KCmdLineArgs::aboutData()->version()));
}

kXneurApp::frmAbout::~frmAbout()
{
  delete ui;
}
void kXneurApp::frmAbout::closeForm()
{
  close();
}
