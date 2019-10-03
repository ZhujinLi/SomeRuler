#ifndef QKRULER_H
#define QKRULER_H

#include "geometrycalculator.h"

#include <QSystemTrayIcon>
#include <QWidget>

class QkRuler : public QWidget
{
    Q_OBJECT

public:
    QkRuler(QWidget *parent = nullptr);
    ~QkRuler() override;

protected:
    void keyReleaseEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    void _appear();
    void _initTray();
    QPoint _handlePos();
    QBitmap _handleMask();
    bool _inHandleArea(QPoint pos);
    void _highlightHandle(bool in);
    void _updateWindowGeometry();

    enum DragState
    {
        DragState_idle,
        DragState_recognizing,
        DragState_moving,
        DragState_rotating,
        DragState_resizing
    };

    GeometryCalculator m_geoCalc;
    QPoint m_dragPosition;
    int m_selectedTick;
    bool m_handleHighlighted;
    DragState m_dragState;

private slots:
    void _reset();
    void _about();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
};

#endif // QKRULER_H
