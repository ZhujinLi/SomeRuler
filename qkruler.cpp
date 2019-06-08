#include "qkruler.h"

#include <QCloseEvent>
#include <QCoreApplication>
#include <QMenu>
#include <QSystemTrayIcon>

QkRuler::QkRuler(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
{
    _initTray();

    m_geoCalc.setRulerSize(QSize{400, 100});
    m_geoCalc.setRotation(30);
}

QkRuler::~QkRuler()
{

}

void QkRuler::_appear()
{
    show();
    raise();
    activateWindow();
}

void QkRuler::_initTray()
{
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/qkruler-icon.png"));

    QMenu* trayIconMenu = new QMenu(this);

    QAction* showAction = new QAction(tr("&Show"), this);
    connect(showAction, &QAction::triggered, this, &QkRuler::_appear);
    trayIconMenu->addAction(showAction);

    QAction* quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}


void QkRuler::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Escape:
        hide();
        break;
    default:
        break;
    }
}


QSize QkRuler::sizeHint() const
{
    return m_geoCalc.getWindowSize();
}
