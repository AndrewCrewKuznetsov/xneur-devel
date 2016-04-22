#ifndef ADDRULES_H
#define ADDRULES_H

#include <QDialog>

namespace Ui
{
    class AddRules;
}

namespace kXneurApp
{
    class AddRules : public QDialog
    {
        Q_OBJECT
    public:
        explicit AddRules(QWidget *parent = 0);
        AddRules(QString word="", bool reg=false, QString condit="",  QWidget *parent = 0);
        ~AddRules();
        QString words;
        bool chkRegistr;
        bool edit;
        QString conditions;
    private:
        Ui::AddRules *ui;
        void settings();
    private slots:
        void saveData();
        void closeForm();
    };
}
#endif // ADDRULES_H
