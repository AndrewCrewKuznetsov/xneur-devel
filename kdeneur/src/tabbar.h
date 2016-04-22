#ifndef TABBAR_H
#define TABBAR_H
#include <QTabBar>
#include <QStylePainter>
#include <QStyleOptionTabV3>
#include <QPainter>
#include <QIcon>
#include <QString>

class TabBar : public QTabBar
{
public:

    explicit TabBar(QWidget* parent=0) : QTabBar(parent)
    {
        setIconSize(QSize(18, 18));
    }

protected:
    QSize tabSizeHint(int) const
    {
        return QSize(150, 30);
    }
    void paintEvent(QPaintEvent *)
    {
        QStylePainter p(this);
        for (int index = 0; index < count(); index++)
        {
            QStyleOptionTabV3 tab;
            initStyleOption(&tab, index);
            QIcon tempIcon = tab.icon;
            QString tempText = tab.text;
            tab.icon = QIcon();
            tab.text = QString();
            p.drawControl(QStyle::CE_TabBarTab, tab);
            QPainter painter;
            painter.begin(this);
            QRect tabrect = tabRect(index);
            tabrect.adjust(0, 8, 0, -8);
            painter.drawText(tabrect, Qt::AlignBottom | Qt::AlignHCenter, tempText);
            tempIcon.paint(&painter, 0, tabrect.top(), tab.iconSize.width(), tab.iconSize.height(), Qt::AlignTop | Qt::AlignHCenter);
            painter.end();
        }
    }
};

class TabWidget : public QTabWidget
{
public:
    explicit TabWidget(QWidget *parent = 0) : QTabWidget(parent)
    {
        setTabBar(new TabBar());
    }
};

#endif //TABBAR_H
