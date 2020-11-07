#include "geometrycalculator.h"
#include <QtMath>

GeometryCalculator::GeometryCalculator() {
    m_rotation = 0;
    m_rotationState = RotationState::flat;
    m_rulerSize = QSize(100, 100);
    m_paddings = 0;
    _update();
}

void GeometryCalculator::setRulerLength(int len) {
    len = qMax(len, 150);

    QSize size(len, 100);

    if (m_rulerSize != size) {
        m_rulerSize = size;
        _update();
    }
}

void GeometryCalculator::setRotation(qreal rotation) {
    switch (m_rotationState) {
    case RotationState::up:
        rotation = qMax(rotation, -90.0);
        rotation = qMin(rotation, 0.0);
        break;
    case RotationState::down:
        rotation = qMax(rotation, 0.0);
        rotation = qMin(rotation, 90.0);
        break;
    case RotationState::flat:
    default:
        rotation = qMax(rotation, -90.0);
        rotation = qMin(rotation, 90.0);
        break;
    }

    if (rotation != m_rotation) {
        m_rotation = rotation;
        _update();
    }
}

QPoint GeometryCalculator::transformPos(const QPoint &pos) const { return pos * m_transform; }

QPoint GeometryCalculator::inversePos(const QPoint &pos) const { return pos * m_invTransform; }

QPoint GeometryCalculator::getMainDirection() const {
    qreal rotationInRadius = qDegreesToRadians(m_rotation);
    int x = static_cast<int>(cos(rotationInRadius) * 100);
    int y = static_cast<int>(sin(rotationInRadius) * 100);
    return QPoint{x, y};
}

void GeometryCalculator::_update() {
    int w = m_rulerSize.width();
    int h = m_rulerSize.height();
    qreal rotationInRadius = qDegreesToRadians(m_rotation);

    qreal winW, winH;
    if (m_rotation > 0) {
        winW = h + w * cos(rotationInRadius);
        winH = w * abs(sin(rotationInRadius)) + h * cos(rotationInRadius);
    } else {
        winW = h + w * cos(rotationInRadius) + h * abs(sin(rotationInRadius));
        winH = h + w * abs(sin(rotationInRadius));
    }
    m_windowSize = {static_cast<int>(winW + 2 * m_paddings), static_cast<int>(winH + 2 * m_paddings)};

    if (m_rotation > 0) {
        m_transform =
            QTransform().translate(m_paddings, m_paddings).translate(m_rulerSize.height(), 0).rotate(m_rotation);
    } else {
        m_transform =
            QTransform().translate(m_paddings, m_paddings).translate(m_rulerSize.height(), winH - h).rotate(m_rotation);
    }

    assert(m_transform.isInvertible());
    m_invTransform = m_transform.inverted();
}
