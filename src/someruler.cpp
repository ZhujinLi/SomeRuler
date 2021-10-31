#include "someruler.h"
#include "ui_about.h"
#include <QApplication>
#include <QBitmap>
#include <QCoreApplication>
#include <QDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QPainter>
#include <QScreen>
#include <QStyle>
#include <QWindow>
#include <QtDebug>
#include <QtMath>

static const int HANDLE_RADIUS = 4;
static const int HANDLE_MARGIN = 20;
static const int HANDLE_DETECT_RADIUS = HANDLE_RADIUS * 2;
static const int HANDLE_MOVE_THRESHOLD = HANDLE_DETECT_RADIUS + 1;

SomeRuler::SomeRuler(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint), m_selectedTick(-1),
      m_handleHighlighted(false), m_dragState(DragState_idle) {
    setAttribute(Qt::WA_TranslucentBackground);

#ifdef Q_OS_MACOS
    setAttribute(Qt::WA_MacAlwaysShowToolWindow);
#endif

    m_geoCalc.setPaddings(5); // For anti-aliasing

    _initTray();

    setMouseTracking(true);

    _appear();

    _reset();
}

SomeRuler::~SomeRuler() {}

void SomeRuler::_appear() {
    show();
    raise();
    activateWindow();
}

void SomeRuler::_initTray() {
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/app.png"));

    QMenu *trayIconMenu = new QMenu(this);

    QAction *showAction = new QAction(tr("&Show"), this);
    connect(showAction, &QAction::triggered, this, &SomeRuler::_appear);
    trayIconMenu->addAction(showAction);

    QAction *aboutAction = new QAction(tr("&About..."), this);
    connect(aboutAction, &QAction::triggered, this, &SomeRuler::_about);
    trayIconMenu->addAction(aboutAction);

    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
    trayIconMenu->addAction(quitAction);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &SomeRuler::_iconActivated);

    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->show();
}

void SomeRuler::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Escape:
        hide();
        break;
    default:
        break;
    }
}

QPoint SomeRuler::_handlePos() {
    QSize sz = m_geoCalc.getRulerSize();
    return {sz.width() - HANDLE_MARGIN, sz.height() / 2};
}

void SomeRuler::paintEvent(QPaintEvent *) {
    QSize rulerSize = m_geoCalc.getRulerSize();
    int w = rulerSize.width();
    int h = rulerSize.height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = painter.font();
    font.setPixelSize(11);
    painter.setFont(font);

    // Rect
    QRectF rulerRect = QRectF{.5, .5, w + .0, h + .0};
    painter.setTransform(m_geoCalc.getTransform()); // After setting clipper
    painter.setBrush(QColor(0xff, 0xff, 0xff, 0xc0));
    painter.setPen(Qt::black);
    painter.drawRect(rulerRect);

    // Ticks
    painter.setPen(Qt::black);
    for (int tick = 0; tick < w; tick += 2) {
        double len = tick % 100 == 0 ? 15 : tick % 10 == 0 ? 10 : 5;
        painter.drawLine(QPointF{tick + .5, .5}, QPointF{tick + .5, .5 + len});
        painter.drawLine(QPointF{tick + .5, h + .5}, QPointF{tick + .5, h + .5 - len});
    }

    // Selected tick
    if (m_selectedTick >= 0) {
        painter.setPen(Qt::red);
        painter.drawLine(QPointF{m_selectedTick + .5, .5}, QPointF{m_selectedTick + .5, h + .5});
    }

    // Labels
    painter.setPen(Qt::black);
    for (int tick = 0; tick < w; tick++) {
        if (tick % 100 == 0) {
            QString label = QString::number(tick * devicePixelRatio());

            QRectF upperRect(tick + 1.5, 15, 100, 100);
            upperRect &= rulerRect;
            painter.drawText(upperRect, Qt::AlignLeft | Qt::AlignTop, label);

            QRectF lowerRect(tick + 1.5, h + .5 - 15 - 100, 100, 100);
            lowerRect &= rulerRect;
            painter.drawText(lowerRect, Qt::AlignLeft | Qt::AlignBottom, label);
        }
    }

    // Info
    painter.setPen(m_selectedTick >= 0 ? Qt::red : Qt::black);
    QRect infoRect(15, 0, 100, h);
    QString infoText = _makeInfoText();
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

bool SomeRuler::_isPosInHandleArea(QPoint pos) {
    QPoint handlePos = m_geoCalc.transformPos(_handlePos());
    QRect handleArea(handlePos.x() - HANDLE_DETECT_RADIUS, handlePos.y() - HANDLE_DETECT_RADIUS,
                     HANDLE_DETECT_RADIUS * 2, HANDLE_DETECT_RADIUS * 2);
    return handleArea.contains(pos);
}

void SomeRuler::_highlightHandle(bool in) {
    if (in != m_handleHighlighted) {
        m_handleHighlighted = in;
        update();
    }
}

void SomeRuler::_syncGeometryWithCalculator() {
    QSize newSize = m_geoCalc.getWindowSize();

    QPoint newTopLeft;
    if (m_geoCalc.getRotationState() == RotationState::up) {
        QPoint oldTopLeft = frameGeometry().topLeft();
        QSize oldSize = frameGeometry().size();
        newTopLeft = QPoint(oldTopLeft.x(), oldTopLeft.y() + oldSize.height() - newSize.height());
    } else {
        newTopLeft = geometry().topLeft();
    }

    QRect newGeometry(newTopLeft, newSize);
    setGeometry(newGeometry);
}

QString SomeRuler::_makeInfoText() {
    QSize rulerSize = m_geoCalc.getRulerSize();
    int w = rulerSize.width();
    int num2Show = (m_selectedTick >= 0 ? m_selectedTick : w) * devicePixelRatio();
    QString text = QString::number(num2Show) + " px";
    qreal rotation = abs(m_geoCalc.getRotation());
    if (rotation != 90 && rotation != 0) {
        text += ", " + QString::number(rotation, 'f', 1) + " deg";
    }
    return text;
}

void SomeRuler::_reset() {
    m_geoCalc.setRulerLength(600);
    m_geoCalc.setRotationState(RotationState::flat);
    m_geoCalc.setRotation(0);
    _syncGeometryWithCalculator();

    QScreen *screen = window()->windowHandle()->screen();
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), screen->geometry()));

    m_selectedTick = -1;
    m_handleHighlighted = false;
    update();
}

void SomeRuler::_about() {
    QDialog *dialog = new QDialog(this);
    Ui::About aboutUi;
    aboutUi.setupUi(dialog);
    dialog->show();

    dialog->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QScreen *screen = dialog->window()->windowHandle()->screen();
    dialog->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, dialog->size(), screen->geometry()));
}

void SomeRuler::_iconActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        _appear();
        break;
    default:;
    }
}

void SomeRuler::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        _highlightHandle(_isPosInHandleArea(event->localPos().toPoint()));
        m_dragState = DragState_recognizing;
        event->accept();
    }
}

void SomeRuler::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        if (event->localPos() != m_dragPosition) {
            if (m_dragState == DragState_recognizing) {
                if (!m_handleHighlighted)
                    m_dragState = DragState_moving;
                else {
                    QPoint diff =
                        m_geoCalc.inversePos(event->localPos().toPoint()) - m_geoCalc.inversePos(m_dragPosition);
                    if (qAbs(diff.x()) > qAbs(diff.y()) + HANDLE_MOVE_THRESHOLD)
                        m_dragState = DragState_resizing;
                    else if (qAbs(diff.x()) < qAbs(diff.y()) - HANDLE_MOVE_THRESHOLD) {
                        m_dragState = DragState_rotating;
                        if (m_geoCalc.getRotationState() == RotationState::flat) {
                            m_geoCalc.setRotationState(diff.y() > 0 ? RotationState::down : RotationState::up);
                        }
                    }
                }
            }
        }

        QSize rulerSize = m_geoCalc.getRulerSize();
        QPoint origin = m_geoCalc.transformPos(QPoint{0, rulerSize.height() / 2}) + geometry().topLeft();
        QPoint delta = event->globalPos() - origin;

        switch (m_dragState) {
        case DragState_moving:
            move(event->globalPos() - m_dragPosition);
            _syncGeometryWithCalculator(); // Needs this in case there are errors when setting geometry
            break;
        case DragState_resizing: {
            if (QPoint::dotProduct(delta, m_geoCalc.getMainDirection()) > 0) {
                int len = static_cast<int>(roundf(sqrtf(static_cast<float>(QPoint::dotProduct(delta, delta)))));
                len += HANDLE_MARGIN;
                m_geoCalc.setRulerLength(len);
                _syncGeometryWithCalculator();
            }
            break;
        }
        case DragState_rotating: {
            qreal angle = qRadiansToDegrees(atan2(delta.y(), delta.x()));
            m_geoCalc.setRotation(angle);
            _syncGeometryWithCalculator();
            break;
        }
        default:
            break;
        }
    } else {
        _highlightHandle(_isPosInHandleArea(event->localPos().toPoint()));
    }

    event->accept();
}

void SomeRuler::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint rawPos = m_geoCalc.inversePos(event->localPos().toPoint());
        bool inTickArea = rawPos.y() < 15 || rawPos.y() > m_geoCalc.getRulerSize().height() - 15;
        bool hasDragged = m_dragState > DragState_recognizing;

        if (m_dragState == DragState_rotating) {
            if (m_geoCalc.getRotation() == 0)
                m_geoCalc.setRotationState(RotationState::flat);
            event->accept();
        } else if (hasDragged && !inTickArea) {
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
        _highlightHandle(_isPosInHandleArea(event->localPos().toPoint()));
    }
}

void SomeRuler::mouseDoubleClickEvent(QMouseEvent *event) {
    if (_isPosInHandleArea(event->localPos().toPoint())) {
        _reset();

        m_dragState = DragState_idle;

        event->accept();
    }
}
