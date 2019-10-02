#include <QtTest>
#include "../geometrycalculator.h"

static const int TOLERANCE = 2;

static bool _QSize_fuzzyCompare(const QSize& a, const QSize& b)
{
    int diffX = qAbs(a.width() - b.width());
    int diffY = qAbs(a.height() - b.height());
    return diffX <= TOLERANCE && diffY <= TOLERANCE;
}

class TstGeometryCalculator : public QObject
{
    Q_OBJECT

private slots:
    void test_case_window_size()
    {
        GeometryCalculator o;
        o.setRulerLength(1000);
        QVERIFY(_QSize_fuzzyCompare(o.getWindowSize(), QSize{1100, 100}));

        o.setRotation(45);
        QVERIFY(_QSize_fuzzyCompare(o.getWindowSize(), QSize{808, 778}));

        o.setRotation(90);
        QVERIFY(_QSize_fuzzyCompare(o.getWindowSize(), QSize{100, 1000}));

        o.setRotation(-45);
        QVERIFY(_QSize_fuzzyCompare(o.getWindowSize(), QSize{int(100+1000/1.414+100/1.414), int(1000/1.414+100)}));
    }

    void test_case_transform()
    {
        GeometryCalculator o;
        o.setRulerLength(1000);
        QCOMPARE(o.inversePos(o.transformPos(QPoint{0, 0})), (QPoint{0, 0}));

        o.setRotation(90);
        QCOMPARE(o.inversePos(o.transformPos(QPoint{500, 50})), (QPoint{500, 50}));

        o.setRotation(-90);
        QCOMPARE(o.inversePos(o.transformPos(QPoint{500, 50})), (QPoint{500, 50}));
    }
};

QTEST_MAIN(TstGeometryCalculator)

#include "tst_geometrycalculator.moc"
