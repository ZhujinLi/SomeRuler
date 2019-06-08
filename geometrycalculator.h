#ifndef GEOMETRYCALCULATOR_H
#define GEOMETRYCALCULATOR_H

#include <QSize>



class GeometryCalculator
{
public:
    GeometryCalculator();

    void setRulerSize(const QSize& size) { m_rulerSize = size; }

    // Unit: degrees
    // It will be clamped to [0, 90].
    // Initial value: 0
    void setRotation(float rotation);

    QSize getWindowSize() const;

private:
    QSize m_rulerSize;
    float m_rotation;
};

#endif // GEOMETRYCALCULATOR_H
