#ifndef GETNAMEAPP_H
#define GETNAMEAPP_H

#include <QDialog>
#include <QProcess>
#include <QAbstractButton>
#include <QMessageBox>
namespace Ui
{
    class getNameApp;
}
namespace kXneurApp
{
    class getNameApp : public QDialog
    {
        Q_OBJECT

    public:
        explicit getNameApp(QWidget *parent = 0);
        ~getNameApp();
        QString appName;
    private slots:
        void Clicked(QAbstractButton*);
        void getApp();
        void readOutput();
    private:
        QProcess *Xprop;
        Ui::getNameApp *ui;
    };
}
#endif // GETNAMEAPP_H
