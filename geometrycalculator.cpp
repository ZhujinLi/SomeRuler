#include "geometrycalculator.h"
#include <QtMath>

GeometryCalculator::GeometryCalculator()
{
    m_rotation = 0;
}

void GeometryCalculator::setRotation(float rotation)
{
    rotation = qMax(rotation, 0.f);
    rotation = qMin(rotation, 90.f);
    m_rotation = rotation;
}

QSize GeometryCalculator::getWindowSize() const
{
    int w = m_rulerSize.width();
    int h = m_rulerSize.height();
    float rotationInRadius = qDegreesToRadians(m_rotation);
    float winW = h + w * cosf(rotationInRadius);
    float winH = w * sinf(rotationInRadius) + h * cosf(rotationInRadius);
    return {static_cast<int>(ceilf(winW)), static_cast<int>(ceilf(winH))};
}
