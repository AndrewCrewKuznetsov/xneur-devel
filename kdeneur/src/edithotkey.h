#ifndef EDITHOTKEY_H
#define EDITHOTKEY_H

#include <QDialog>
#include <QKeyEvent>
#include <QFlags>

#include <QDebug>
namespace Ui
{
    class EditHotKey;
}

namespace kXneurApp
{
    class EditHotKey : public QDialog
    {
        Q_OBJECT

    public:
        explicit EditHotKey(QWidget *parent = 0, QString action="", QString key="");
        QString hot_keys;
	QString action_name;
        ~EditHotKey();
    protected:
        virtual void keyPressEvent(QKeyEvent *event);
        virtual void keyReleaseEvent(QKeyEvent * event);

    private:
        Ui::EditHotKey *ui;
        QString modif(int);
        //QString md_key;

    private slots:
        void closeForm();
        void save();
    };
}
#endif // EDITHOTKEY_H
