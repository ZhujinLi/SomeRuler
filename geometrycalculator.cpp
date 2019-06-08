#include "geometrycalculator.h"
#include <QtMath>

GeometryCalculator::GeometryCalculator()
{
    m_rotation = 0;
}

void GeometryCalculator::setRotation(qreal rotation)
{
    rotation = qMax(rotation, 0.0);
    rotation = qMin(rotation, 90.0);
    m_rotation = rotation;
}

QSize GeometryCalculator::getWindowSize() const
{
    int w = m_rulerSize.width();
    int h = m_rulerSize.height();
    qreal rotationInRadius = qDegreesToRadians(m_rotation);
    qreal winW = h + w * cos(rotationInRadius);
    qreal winH = w * sin(rotationInRadius) + h * cos(rotationInRadius);
    return {static_cast<int>(ceil(winW)), static_cast<int>(ceil(winH))};
}

QTransform GeometryCalculator::getTransform() const
{
    return QTransform()
            .translate(m_rulerSize.height(), 0)
            .rotate(m_rotation);
}
