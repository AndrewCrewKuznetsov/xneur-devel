#ifndef ADDUSERACTION_H
#define ADDUSERACTION_H

#include <QDialog>
#include <QKeyEvent>
#include <QFlags>
namespace Ui
{
    class addUserAction;
}

namespace kXneurApp
{
    class addUserAction : public QDialog
    {
        Q_OBJECT

    public:
        explicit addUserAction(QString nm="", QString key="", QString cmd="", QWidget *parent = 0);
        ~addUserAction();
        QString name, hot_key, command;
    private slots:
        void saveAction();
        void cancelAction();
    private:
        QString modif(int);
        Ui::addUserAction *ui;
    protected:
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent * event);
    };
}

#endif // ADDUSERACTION_H
