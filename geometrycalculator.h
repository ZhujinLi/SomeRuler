#ifndef GEOMETRYCALCULATOR_H
#define GEOMETRYCALCULATOR_H

#include <QSize>
#include <QTransform>

enum class RotationState {
    flat, ///< The ruler is unrotated
    up,   ///< The ruler handle is above original position
    down, ///< The ruler handle is below original position
};

class GeometryCalculator {
public:
    GeometryCalculator();

    /// @param len
    /// Ruler's horizontal length in pixels.
    void setRulerLength(int len);

    /// Ruler's intrinsic dimension.
    QSize getRulerSize() { return m_rulerSize; }

    RotationState getRotationState() { return m_rotationState; }
    void setRotationState(RotationState state) { m_rotationState = state; }

    /// The ruler's rotation angle in degrees, between the range [-90, 90], where 0 is its original state,
    /// and a positive angle indicates that it's rotated downwards.
    /// @note
    /// It will be clamped according to current rotation mode.
    qreal getRotation() { return m_rotation; }
    void setRotation(qreal rotation);

    /// @note Initial value: 0
    void setPaddings(int paddings) { m_paddings = paddings; }

    /// The size of the bounding box of the rotated ruler.
    QSize getWindowSize() const { return m_windowSize; }

    /// Transform the position from standard unrotated space to actual screen space.
    QPoint transformPos(const QPoint &pos) const;

    /// The reverse of @ref transformPos.
    QPoint inversePos(const QPoint &pos) const;

    /// @sa transformPos
    QTransform getTransform() const { return m_transform; }

private:
    void _update();

    QSize m_rulerSize;
    qreal m_rotation;
    QSize m_windowSize;
    QTransform m_transform;
    QTransform m_invTransform;
    int m_paddings;
    RotationState m_rotationState;
};

#endif // GEOMETRYCALCULATOR_H
