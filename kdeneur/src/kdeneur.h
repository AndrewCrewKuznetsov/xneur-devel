#ifndef KXNEUR_H
#define KXNEUR_H

//app header files
#include "kxneurtray.h"
#include "xneurconfig.h"

//Qt header files
#include <QApplication>
#include <QProcess>

namespace kXneurApp
{
    class kXneur : public QApplication
    {
        Q_OBJECT

    public:
        kXneur(int&, char **);
        ~kXneur();
    public slots:

    private:
      bool running;
      kXneurTray *trayApp;
      xNeurConfig *cfgXneur;
      void settignsTray();
    signals:
      void changeIconTray(QString);
      void reLoadNeur();
    private slots:
      void layoutChanged(QString);
      void startStopNeur();
      void OpenSettings();
    };
}
#endif // KXNEUR_H
