﻿#include "qkruler.h"

#include <QApplication>
#include <QBitmap>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QMenu>
#include <QPainter>
#include <QSystemTrayIcon>
#include <QtDebug>
#include <cmath>
#include <QtMath>

static const int HANDLE_RADIUS = 4;
static const int HANDLE_MARGIN = 20;

static int _QPoint_length(const QPoint& p)
{
    double x = p.x();
    double y = p.y();
    double len = sqrt(x*x + y*y);
    return static_cast<int>(len + 0.5);
}

QkRuler::QkRuler(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint),
      m_draggingHandle(false),
      m_selectedTick(-1),
      m_cursorInHandleArea(false)
{
    setAttribute(Qt::WA_TranslucentBackground);

    _initTray();

    m_geoCalc.setRulerSize(QSize{600, 100});
    m_geoCalc.setRotation(0);
    resize(m_geoCalc.getWindowSize());

    _updateMask();

    setMouseTracking(true);
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


void QkRuler::_updateMask()
{
    QBitmap mask(m_geoCalc.getWindowSize());
    mask.clear();

    QPainter painter(&mask);
    painter.setTransform(m_geoCalc.getTransform());

    // Ruler rect
    painter.setBrush(Qt::color1);
    QRect rect(0, 0, m_geoCalc.getRulerSize().width(), m_geoCalc.getRulerSize().height());
    rect = rect.marginsAdded(QMargins(1, 1, 1, 1)); // Expand for AA
    painter.drawRect(rect);

    setMask(mask);
}

QBitmap QkRuler::_handleMask()
{
    QBitmap mask(m_geoCalc.getWindowSize());
    mask.clear();

    QPainter painter(&mask);
    painter.setBrush(Qt::color1);
    painter.drawRect(QRect{0, 0, m_geoCalc.getWindowSize().width(), m_geoCalc.getWindowSize().height()});

    // Hole at handle
    painter.setTransform(m_geoCalc.getTransform());
    painter.setBrush(Qt::color0);
    painter.setPen(Qt::NoPen);
    QPoint handleCenter = _handlePos();
    painter.drawEllipse(handleCenter, HANDLE_RADIUS, HANDLE_RADIUS);

    return mask;
}

QPoint QkRuler::_handlePos()
{
    QSize sz = m_geoCalc.getRulerSize();
    return {sz.width() - HANDLE_MARGIN, sz.height() / 2};
}


void QkRuler::paintEvent(QPaintEvent *)
{
    QSize rulerSize = m_geoCalc.getRulerSize();
    int w = rulerSize.width();
    int h = rulerSize.height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = painter.font();
    font.setPixelSize(11);
    painter.setFont(font);

    // Rect
    if (!m_cursorInHandleArea) {
        painter.setClipping(true);
        painter.setClipRegion(_handleMask());
    }
    painter.setTransform(m_geoCalc.getTransform()); // After setting clipper
    painter.setBrush(QColor(0xff, 0xff, 0xff, 0xc0));
    painter.drawRect(QRect{0, 0, w, h});
    painter.setClipping(false);

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

    // Handle
    QPoint handleCenter = _handlePos();
    QPen pen;
    pen.setWidth(2);
    pen.setBrush(m_cursorInHandleArea ? Qt::red : Qt::black);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(handleCenter, HANDLE_RADIUS, HANDLE_RADIUS);
}

bool QkRuler::_inHandleArea(QPoint pos)
{
    QPoint handlePos = m_geoCalc.transformPos(_handlePos());
    QRect handleArea(handlePos.x() - HANDLE_RADIUS * 2,
                         handlePos.y() - HANDLE_RADIUS * 2, HANDLE_RADIUS * 4, HANDLE_RADIUS * 4);
    return handleArea.contains(pos);
}

void QkRuler::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        m_draggingHandle = _inHandleArea(event->localPos().toPoint());
        m_hasDragged = false;
        event->accept();
    }
}

void QkRuler::mouseMoveEvent(QMouseEvent *event)
{
    bool inHandleArea = _inHandleArea(event->localPos().toPoint());
    if (m_cursorInHandleArea != inHandleArea) {
        m_cursorInHandleArea = inHandleArea;
        update();
    }

    if (event->buttons() & Qt::LeftButton) {
        if (event->globalPos() != m_dragPosition)
            m_hasDragged = true;

        if (m_draggingHandle) {
            QSize rulerSize = m_geoCalc.getRulerSize();
            QPoint origin = m_geoCalc.transformPos(QPoint{0, rulerSize.height() / 2});
            QPoint delta = event->localPos().toPoint() - origin;

            int len = _QPoint_length(delta) + HANDLE_MARGIN;
            m_geoCalc.setRulerSize(QSize{len, rulerSize.height()});

            qreal angle = qRadiansToDegrees(atan2(delta.y(), delta.x()));
            m_geoCalc.setRotation(angle);

            resize(m_geoCalc.getWindowSize());
            _updateMask();
        }
        else
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
        } else if (!m_hasDragged && !inTickArea) {
            m_selectedTick = -1;
            update();
            event->accept();
        }
    }
}

