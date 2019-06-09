#include "geometrycalculator.h"
#include <QtMath>

// For AA
static const int PADDINGS = 5;

GeometryCalculator::GeometryCalculator()
{
    m_rotation = 0;
    m_rulerSize = QSize(100, 100);
    _update();
}

void GeometryCalculator::setRulerLength(int len)
{
    len = qMax(len, 100);

    QSize size(len, 100);

    if (m_rulerSize != size) {
        m_rulerSize = size;
        _update();
    }
}

void GeometryCalculator::setRotation(qreal rotation)
{
    rotation = qMax(rotation, 0.0);
    rotation = qMin(rotation, 90.0);

    if (!qFuzzyCompare(rotation, m_rotation)) {
        m_rotation = rotation;
        _update();
    }
}

QPoint GeometryCalculator::transformPos(const QPoint &pos) const
{
    return pos * m_transform;
}

QPoint GeometryCalculator::inversePos(const QPoint &pos) const
{
    return pos * m_invTransform;
}

void GeometryCalculator::_update()
{
    int w = m_rulerSize.width();
    int h = m_rulerSize.height();
    qreal rotationInRadius = qDegreesToRadians(m_rotation);
    qreal winW = h + w * cos(rotationInRadius);
    qreal winH = w * sin(rotationInRadius) + h * cos(rotationInRadius);
    m_windowSize = {static_cast<int>(winW + 2 * PADDINGS), static_cast<int>(winH + 2 * PADDINGS)};

    m_transform = QTransform()
            .translate(PADDINGS, PADDINGS)
            .translate(m_rulerSize.height(), 0)
            .rotate(m_rotation);

    assert(m_transform.isInvertible());
    m_invTransform = m_transform.inverted();
}
