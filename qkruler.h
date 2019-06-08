#ifndef QKRULER_H
#define QKRULER_H

#include <QWidget>

class QkRuler : public QWidget
{
    Q_OBJECT

public:
    QkRuler(QWidget *parent = nullptr);
    ~QkRuler() override;

private:
    void _appear();
    void _initTray();

    // QWidget interface
protected:
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // QKRULER_H
