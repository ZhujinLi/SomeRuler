#ifndef GEOMETRYCALCULATOR_H
#define GEOMETRYCALCULATOR_H

#include <QSize>
#include <QTransform>

class GeometryCalculator
{
public:
    GeometryCalculator();

    void setRulerSize(const QSize& size) { m_rulerSize = size; }
    const QSize& getRulerSize() { return m_rulerSize; }

    // Unit: degrees
    // It will be clamped to [0, 90].
    // Initial value: 0
    void setRotation(qreal rotation);

    QSize getWindowSize() const;

    QTransform getTransform() const;

private:
    QSize m_rulerSize;
    qreal m_rotation;
};

#endif // GEOMETRYCALCULATOR_H
