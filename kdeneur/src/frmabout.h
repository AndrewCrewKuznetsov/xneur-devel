#ifndef FRMABOUT_H
#define FRMABOUT_H

#include <QDialog>

namespace Ui
{
  class frmAbout;
}
namespace kXneurApp
{
  class frmAbout : public QDialog
  {
    Q_OBJECT

  public:
    explicit frmAbout(QWidget *parent = 0);
    ~frmAbout();

  private:
    Ui::frmAbout *ui;
  private slots:
    void closeForm();
  };
}
#endif // FRMABOUT_H
