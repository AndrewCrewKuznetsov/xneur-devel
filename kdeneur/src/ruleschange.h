#ifndef RULESCHANGE_H
#define RULESCHANGE_H

#include <QDialog>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
namespace Ui
{
    class RulesChange;
}
namespace kXneurApp
{
    class RulesChange : public QDialog
    {
        Q_OBJECT

    public:
        explicit RulesChange(QStringList text, QWidget *parent = 0);
        ~RulesChange();

    private:
        Ui::RulesChange *ui;
        QString savePath;
        QStringList conditions_names;
        void createConnect();
        void parserDict(QStringList);
    private slots:
        void closeForm();
        void addWords();
        void editWors();
        void delWords();
        void saveDict();

    };
}

#endif // RULESCHANGE_H
