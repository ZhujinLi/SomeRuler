#include <QtTest>
#include "../geometrycalculator.h"

class TstGeometryCalculator : public QObject
{
    Q_OBJECT

private slots:
    void test_case_window_size()
    {
        GeometryCalculator o;
        o.setRulerSize(QSize{100, 10});
        QCOMPARE(o.getWindowSize(), (QSize{110, 10}));

        o.setRotation(45);
        QCOMPARE(o.getWindowSize(), (QSize{81, 78}));

        o.setRotation(90);
        QCOMPARE(o.getWindowSize(), (QSize{10, 100}));
    }

};

QTEST_MAIN(TstGeometryCalculator)

#include "tst_geometrycalculator.moc"
