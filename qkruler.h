#ifndef QKRULER_H
#define QKRULER_H

#include "geometrycalculator.h"

#include <QWidget>

class QkRuler : public QWidget
{
    Q_OBJECT

public:
    QkRuler(QWidget *parent = nullptr);
    ~QkRuler() override;

protected:
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void _appear();
    void _initTray();

    GeometryCalculator m_geoCalc;


    // QWidget interface
public:
    QSize sizeHint() const override;
};

#endif // QKRULER_H
