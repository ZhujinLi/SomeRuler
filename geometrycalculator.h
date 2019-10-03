#ifndef GEOMETRYCALCULATOR_H
#define GEOMETRYCALCULATOR_H

#include <QSize>
#include <QTransform>

enum RotationMode
{
    RotationMode_up,
    RotationMode_down,
    RotationMode_both
};

class GeometryCalculator
{
public:
    GeometryCalculator();

    void setRulerLength(int len);

    void setRotationMode(RotationMode mode) { m_rotationMode = mode; }
    RotationMode getRotationMode() { return m_rotationMode; }

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
    RotationMode m_rotationMode;
};

#endif // GEOMETRYCALCULATOR_H
