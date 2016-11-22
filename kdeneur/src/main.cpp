//app header files
#include "kdeneur.h"

//KDE header files
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kglobal.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kstandarddirs.h>

//Qt header files
#include <QDebug>
#include <QTextCodec>
#include <QTranslator>
#include <QTime>
#include <QLibraryInfo>
#include <unistd.h>

static const KLocalizedString description =ki18n("kXneur (KDE X Neural Switcher) is xNeur front-end for KDE ( http://xneur.ru ).\nThis version work with XNeur v.0.19 only");
static const char version[] = "0.20.0";

int main(int argc, char *argv[])
{
  QTextCodec *codec = QTextCodec::codecForName("UTF-8");
  QTextCodec::setCodecForTr(codec);
  QTextCodec::setCodecForCStrings(codec);
  QTextCodec::setCodecForLocale(codec);
  KConfig conf("kdeneurrc");
 // sleep(conf.group("Properties").readEntry("WaiTime", 0));
  kXneurApp::kXneur neur(argc, argv);
  neur.setApplicationName("kdeNeur");
  neur.setWindowIcon(QIcon(":/icons/kdeneur.png"));
  kXneurApp::kXneur::setQuitOnLastWindowClosed(false);
  KAboutData about("kXneur",0, ki18n("kXneur Keyboard switcher") ,version,description,
                    KAboutData::License_GPL, ki18n("(C) 2012  Sergei Chystyakov"), ki18n(""), "http://xneur.ru","xneur@lists.net.ru");
  about.addAuthor(ki18n("Sergei Chystyakov"),ki18n(""), "klaider@yandex.ru", "http://xneur.ru");
  about.addCredit(ki18n("Andrew Crew Kuznetsov"), ki18n("Development X Neural Switcher"),"andrewcrew@rambler.ru","http://xneur.ru");
  about.addCredit(ki18n("Yankin Nickolay Valerevich"), ki18n("Site"),"web@softodrom.ru","http://xneur.ru");
  KCmdLineArgs::init(argc, argv, &about, KCmdLineArgs::CmdLineArgKDE);
  return neur.exec();

}
