#include "qkruler.h"

#include <QBitmap>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QMenu>
#include <QPainter>
#include <QSystemTrayIcon>
#include <QtDebug>

QkRuler::QkRuler(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint),
      m_selectedTick(-1)
{
    setAttribute(Qt::WA_TranslucentBackground);

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

    QRect rect(0, 0, m_geoCalc.getRulerSize().width(), m_geoCalc.getRulerSize().height());
    rect = rect.marginsAdded(QMargins(1, 1, 1, 1)); // Expand for AA
    painter.drawRect(rect);

    setMask(mask);
}


void QkRuler::paintEvent(QPaintEvent *)
{
    QSize rulerSize = m_geoCalc.getRulerSize();
    int w = rulerSize.width();
    int h = rulerSize.height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setTransform(m_geoCalc.getTransform());
    painter.setBrush(QColor(0xff, 0xff, 0xff, 0xc0));
    painter.drawRect(QRect{0, 0, w, h});

    QFont font = painter.font();
    font.setPixelSize(11);
    painter.setFont(font);

    // Ticks
    for (int tick = 0; tick < w; tick++) {
        if (tick % 2 == 0) {
            int len = tick % 100 == 0 ? 15 : tick % 10 == 0 ? 10 : 5;
            bool isSelected = tick == (m_selectedTick | 1) - 1;
            painter.setPen(isSelected ? Qt::red : Qt::black);
            painter.drawLine(tick, 0, tick, len);
            painter.drawLine(tick, h,tick, h - len);
        }
    }

    // Labels
    painter.setPen(Qt::black);
    for (int tick = 0; tick < w; tick++) {
        if (tick % 100 == 0) {
            QString label = QString::number(tick * devicePixelRatio());

            QRect upperRect(tick, 15, 100, 100);
            painter.drawText(upperRect, Qt::AlignLeft | Qt::AlignTop, label);

            QRect lowerRect(tick, h - 15 - 100, 100, 100);
            painter.drawText(lowerRect, Qt::AlignLeft | Qt::AlignBottom, label);
        }
    }

    // Info
    painter.setPen(m_selectedTick >= 0 ? Qt::red : Qt::black);
    int num2Show = (m_selectedTick >= 0 ? m_selectedTick : w) * devicePixelRatio();
    QRect infoRect(15, 0, 100, h);
    QString infoText = QString::number(num2Show) + " px";
    painter.drawText(infoRect, Qt::AlignLeft | Qt::AlignVCenter, infoText);
}


void QkRuler::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        m_hasDragged = false;
        event->accept();
    }
}

void QkRuler::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (event->globalPos() != m_dragPosition)
            m_hasDragged = true;
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}

void QkRuler::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint rawPos = m_geoCalc.inversePos(event->localPos().toPoint());
        bool inTickArea = rawPos.y() < 15 || rawPos.y() > m_geoCalc.getRulerSize().height() - 15;
        if (m_hasDragged && !inTickArea) {
            event->accept();
        } else if (!m_hasDragged && inTickArea) {
            m_selectedTick = rawPos.x();
            update();
            event->accept();
        } else if(!m_hasDragged && !inTickArea) {
            m_selectedTick = -1;
            update();
            event->accept();
        }
    }
}

