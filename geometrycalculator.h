#ifndef GEOMETRYCALCULATOR_H
#define GEOMETRYCALCULATOR_H

#include <QSize>
#include <QTransform>

class GeometryCalculator
{
public:
    GeometryCalculator();

    void setRulerLength(int len);

    // Unit: degrees
    // It will be clamped to [0, 90].
    // Initial value: 0
    void setRotation(qreal rotation);

    qreal getRotation() { return m_rotation; }

    // Initial value: 0
    void setPaddings(int paddings) { m_paddings = paddings; }

    const QSize& getRulerSize() { return m_rulerSize; }

    QSize getWindowSize() const { return m_windowSize; }

    QTransform getTransform() const { return m_transform; }

    QPoint transformPos(const QPoint& pos) const;
    QPoint inversePos(const QPoint& pos) const;

private:
    void _update();

    QSize m_rulerSize;
    qreal m_rotation;
    QSize m_windowSize;
    QTransform m_transform;
    QTransform m_invTransform;
    int m_paddings;
};

#endif // GEOMETRYCALCULATOR_H
