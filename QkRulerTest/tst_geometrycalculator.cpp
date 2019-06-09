#include <QtTest>
#include "../geometrycalculator.h"

static const int TOLERANCE = 20;

static bool _QPoint_fuzzyCompare(const QPoint& a, const QPoint& b)
{
    int diffX = qAbs(a.x() - b.x());
    int diffY = qAbs(a.y() - b.y());
    return diffX < TOLERANCE && diffY < TOLERANCE;
}

static bool _QSize_fuzzyCompare(const QSize& a, const QSize& b)
{
    int diffX = qAbs(a.width() - b.width());
    int diffY = qAbs(a.height() - b.height());
    return diffX < TOLERANCE && diffY < TOLERANCE;
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
    }

    void test_case_transform()
    {
        GeometryCalculator o;
        o.setRulerLength(1000);
        QVERIFY(_QPoint_fuzzyCompare(o.inversePos(o.transformPos(QPoint{0, 0})), (QPoint{0, 0})));

        o.setRotation(90);
        QVERIFY(_QPoint_fuzzyCompare(o.inversePos(o.transformPos(QPoint{500, 50})), (QPoint{500, 50})));
    }
};

QTEST_MAIN(TstGeometryCalculator)

#include "tst_geometrycalculator.moc"
