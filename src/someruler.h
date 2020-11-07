#ifndef SOMERULER_H
#define SOMERULER_H

#include "geometrycalculator.h"
#include <QSystemTrayIcon>
#include <QWidget>

class SomeRuler : public QWidget {
    Q_OBJECT

public:
    SomeRuler(QWidget *parent = nullptr);
    virtual ~SomeRuler() override;

private slots:
    void _appear();
    void _about();
    void _iconActivated(QSystemTrayIcon::ActivationReason reason);

private:
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    void _reset();
    void _initTray();
    QPoint _handlePos();
    QBitmap _handleMask();
    void _updateMask();
    bool _isPosInHandleArea(QPoint pos);
    void _highlightHandle(bool in);
    void _updateWindowGeometry();
    QString _makeInfoText();

    enum DragState { DragState_idle, DragState_recognizing, DragState_moving, DragState_rotating, DragState_resizing };

    GeometryCalculator m_geoCalc;
    QPoint m_dragPosition;
    int m_selectedTick;
    bool m_handleHighlighted;
    DragState m_dragState;
};

#endif // SOMERULER_H
