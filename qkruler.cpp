#include "qkruler.h"

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
#include <QDialog>
#include <QStyle>
#include <QScreen>
#include <QLayout>
#include <QWindow>
#include <ui_about.h>

namespace Ui {
class About;
}

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
    : QWidget(parent,Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint),
      m_selectedTick(-1),
      m_handleHighlighted(false),
      m_dragState(DragState_idle)
{
    setAttribute(Qt::WA_TranslucentBackground);

    m_geoCalc.setPaddings(5);	// For anti-aliasing

    _initTray();

    _reset();

    setMouseTracking(true);

    _appear();
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
    trayIcon->setIcon(QIcon(":/images/app.png"));

    QMenu* trayIconMenu = new QMenu(this);

    QAction* showAction = new QAction(tr("&Show"), this);
    connect(showAction, &QAction::triggered, this, &QkRuler::_appear);
    trayIconMenu->addAction(showAction);

    QAction* aboutAction = new QAction(tr("&About..."), this);
    connect(aboutAction, &QAction::triggered, this, &QkRuler::_about);
    trayIconMenu->addAction(aboutAction);

    QAction* quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &QkRuler::iconActivated);

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
    QRectF rulerRect = QRectF{.5, .5, w+.0, h+.0};
    if (!m_handleHighlighted) {
        painter.setClipping(true);
        painter.setClipRegion(_handleMask());
    }
    painter.setTransform(m_geoCalc.getTransform()); // After setting clipper
    painter.setBrush(QColor(0xff, 0xff, 0xff, 0xc0));
    painter.setPen(Qt::black);
    painter.drawRect(rulerRect);
    painter.setClipping(false);

    // Ticks
    for (int tick = 0; tick < w; tick++) {
        if (tick % 2 == 0) {
            double len = tick % 100 == 0 ? 15 : tick % 10 == 0 ? 10 : 5;
            bool isSelected = tick == (m_selectedTick | 1) - 1;
            painter.setPen(isSelected ? Qt::red : Qt::black);
            painter.drawLine(QPointF{tick+.5, .5}, QPointF{tick+.5, .5+len});
            painter.drawLine(QPointF{tick+.5, h+.5}, QPointF{tick+.5, h+.5-len});
        }
    }

    // Labels
    painter.setPen(Qt::black);
    for (int tick = 0; tick < w; tick++) {
        if (tick % 100 == 0) {
            QString label = QString::number(tick * devicePixelRatio());

            QRectF upperRect(tick+1.5, 15, 100, 100);
            upperRect &= rulerRect;
            painter.drawText(upperRect, Qt::AlignLeft | Qt::AlignTop, label);

            QRectF lowerRect(tick+1.5, h+.5-15-100, 100, 100);
            lowerRect &= rulerRect;
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
    pen.setBrush(m_handleHighlighted ? Qt::red : Qt::black);
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

void QkRuler::_highlightHandle(bool in)
{
    if (in != m_handleHighlighted) {
        m_handleHighlighted = in;
        update();
    }
}

void QkRuler::_updateWindowGeometry()
{
    QSize newSize = m_geoCalc.getWindowSize();

    if (m_geoCalc.getRotationMode() == RotationMode_up) {
        QPoint oldTopLeft = frameGeometry().topLeft();
        QSize oldSize = frameGeometry().size();
        QPoint newTopLeft(oldTopLeft.x(), oldTopLeft.y() + oldSize.height() - newSize.height());
        move(newTopLeft);
    }

    resize(newSize);

    _updateMask();
}

void QkRuler::_reset()
{
    m_geoCalc.setRulerLength(600);
    m_geoCalc.setRotation(0);
    _updateWindowGeometry();

    m_selectedTick = -1;
    m_handleHighlighted = false;
    update();
}

void QkRuler::_about()
{
    QDialog* dialog = new QDialog(this);
    Ui::About aboutUi;
    aboutUi.setupUi(dialog);
    dialog->show();

    QSize size = dialog->size();
    qreal h = (aboutUi.textBrowser->document()->size().height() + dialog->layout()->margin() * 2) * 1.2;
    size.setWidth(static_cast<int>(h * size.width() / size.height()));
    size.setHeight(static_cast<int>(h));
    dialog->setFixedSize(size);

    QScreen* screen = dialog->window()->windowHandle()->screen();
    dialog->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            dialog->size(),
            screen->geometry()
        )
    );
}

void QkRuler::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        _appear();
        break;
    default:
        ;
    }
}

void QkRuler::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        _highlightHandle(_inHandleArea(event->localPos().toPoint()));
        m_dragState = DragState_recognizing;
        event->accept();
    }
}

void QkRuler::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (event->localPos() != m_dragPosition) {
            if (m_dragState == DragState_recognizing) {
                if (!m_handleHighlighted)
                    m_dragState = DragState_moving;
                else {
                    QPoint diff = m_geoCalc.inversePos(event->localPos().toPoint()) -
                            m_geoCalc.inversePos(m_dragPosition);
                    if (qAbs(diff.x()) > qAbs(diff.y()) + 5)
                        m_dragState = DragState_resizing;
                    else if (qAbs(diff.x()) < qAbs(diff.y()) - 5)
                        m_dragState = DragState_rotating;
                }
            }
        }

        QSize rulerSize = m_geoCalc.getRulerSize();
        QPoint origin = m_geoCalc.transformPos(QPoint{0, rulerSize.height() / 2});
        QPoint delta = event->localPos().toPoint() - origin;

        switch (m_dragState) {
        case DragState_moving:
            move(event->globalPos() - m_dragPosition);
            break;
        case DragState_resizing:
        {
            int len = _QPoint_length(delta) + HANDLE_MARGIN;
            m_geoCalc.setRulerLength(len);
            _updateWindowGeometry();
            break;
        }
        case DragState_rotating:
        {
            qreal angle = qRadiansToDegrees(atan2(delta.y(), delta.x()));
            m_geoCalc.setRotation(angle);
            _updateWindowGeometry();
            break;
        }
        default:
            break;
        }

        event->accept();
    } else {
        _highlightHandle(_inHandleArea(event->localPos().toPoint()));
    }

}

void QkRuler::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint rawPos = m_geoCalc.inversePos(event->localPos().toPoint());
        bool inTickArea = rawPos.y() < 15 || rawPos.y() > m_geoCalc.getRulerSize().height() - 15;
        bool hasDragged = m_dragState > DragState_recognizing;

        if (m_dragState == DragState_rotating) {
            if (m_geoCalc.getRotation() < 0)
                m_geoCalc.setRotationMode(RotationMode_up);
            else if (m_geoCalc.getRotation() > 0)
                m_geoCalc.setRotationMode(RotationMode_down);
            else
                m_geoCalc.setRotationMode(RotationMode_both);
            event->accept();
        }
        else if (hasDragged && !inTickArea) {
            event->accept();
        } else if (!hasDragged && inTickArea) {
            if (m_dragState == DragState_recognizing) {
                m_selectedTick = rawPos.x();
                update();
            }
            event->accept();
        } else if (!hasDragged && !inTickArea) {
            m_selectedTick = -1;
            update();
            event->accept();
        }

        m_dragState = DragState_idle;
        _highlightHandle(_inHandleArea(event->localPos().toPoint()));
    }
}

void QkRuler::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (_inHandleArea(event->localPos().toPoint())) {
        _reset();

        m_dragState = DragState_idle;

        event->accept();
    }
}
