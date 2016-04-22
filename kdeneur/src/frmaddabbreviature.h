#ifndef FRMADDABBREVIATURE_H
#define FRMADDABBREVIATURE_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui
{
class frmAddAbbreviature;
}
namespace kXneurApp
{
    class frmAddAbbreviature : public QDialog
    {
        Q_OBJECT

    public:
        explicit frmAddAbbreviature(QWidget *parent = 0);
        ~frmAddAbbreviature();
        QString abb;
        QString text;
    private slots:
        void clickbutton(QAbstractButton*);
    private:
        Ui::frmAddAbbreviature *ui;
    };
}
#endif // FRMADDABBREVIATURE_H
