#ifndef SOMERULER_H
#define SOMERULER_H

#include "geometrycalculator.h"

#include <QSystemTrayIcon>
#include <QWidget>

class SomeRuler : public QWidget
{
    Q_OBJECT

public:
    SomeRuler(QWidget *parent = nullptr);
    ~SomeRuler() override;

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
    void _updateMask();
    bool _inHandleArea(QPoint pos);
    void _highlightHandle(bool in);
    void _updateWindowGeometry();
    QString _makeInfoText();

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

#endif // SOMERULER_H
