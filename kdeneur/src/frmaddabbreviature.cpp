#include "frmaddabbreviature.h"
#include "ui_frmaddabbreviature.h"

kXneurApp::frmAddAbbreviature::frmAddAbbreviature(QWidget *parent) :    QDialog(parent),    ui(new Ui::frmAddAbbreviature)
{
    ui->setupUi(this);
    connect(ui->cmdBox, SIGNAL(clicked(QAbstractButton*)),SLOT(clickbutton(QAbstractButton*)));
}

kXneurApp::frmAddAbbreviature::~frmAddAbbreviature()
{
    delete ui;
}

void kXneurApp::frmAddAbbreviature::clickbutton(QAbstractButton *but)
{
    if(ui->cmdBox->standardButton(but) == QDialogButtonBox::Ok)
    {
      done(QDialog::Accepted);
      if(!ui->txtAbbrev->text().isEmpty() && !ui->txtFullText->text().isEmpty())
      {
          abb=ui->txtAbbrev->text();
          text=ui->txtFullText->text();
          this->close();
      }
    }
    else
    {
        this->close();
    }
}

