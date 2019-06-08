#include "qkruler.h"

#include <QBitmap>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QMenu>
#include <QPainter>
#include <QSystemTrayIcon>

QkRuler::QkRuler(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint)
{
    _initTray();

    m_geoCalc.setRulerSize(QSize{400, 100});
    m_geoCalc.setRotation(30);

    _updateMask();
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

void QkRuler::_updateMask()
{
    QBitmap mask(m_geoCalc.getWindowSize());
    mask.clear();

    QPainter painter(&mask);
    painter.setTransform(m_geoCalc.getTransform());
    painter.setBrush(Qt::color1);
    painter.drawRect(QRect{0, 0, m_geoCalc.getRulerSize().width(), m_geoCalc.getRulerSize().height()});

    setMask(mask);
}


void QkRuler::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setBrush(QColor(0xff, 0, 0));
    painter.drawRect(QRect{0, 0, this->geometry().width(), this->geometry().height()});
}
