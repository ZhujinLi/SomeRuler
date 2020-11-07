#include "../src/geometrycalculator.h"
#include <QtTest>

static const int TOLERANCE = 2;

static bool _QSize_fuzzyCompare(const QSize &a, const QSize &b) {
    int diffX = qAbs(a.width() - b.width());
    int diffY = qAbs(a.height() - b.height());
    return diffX <= TOLERANCE && diffY <= TOLERANCE;
}

class TstGeometryCalculator : public QObject {
    Q_OBJECT

private slots:
    void windowSizeIsMinimalBBox() {
        GeometryCalculator o;
        o.setRulerLength(1000);
        QVERIFY(_QSize_fuzzyCompare(o.getWindowSize(), QSize{1100, 100}));

        o.setRotation(45);
        QVERIFY(_QSize_fuzzyCompare(o.getWindowSize(), QSize{808, 778}));

        o.setRotation(90);
        QVERIFY(_QSize_fuzzyCompare(o.getWindowSize(), QSize{100, 1000}));

        o.setRotation(-45);
        QVERIFY(_QSize_fuzzyCompare(o.getWindowSize(),
                                    QSize{int(100 + 1000 / 1.414 + 100 / 1.414), int(1000 / 1.414 + 100)}));
    }

    void transformAndInversionAreUniform() {
        GeometryCalculator o;
        o.setRulerLength(1000);

        QCOMPARE(o.inversePos(o.transformPos(QPoint{0, 0})), (QPoint{0, 0}));

        o.setRotation(90);
        QCOMPARE(o.inversePos(o.transformPos(QPoint{500, 50})), (QPoint{500, 50}));

        o.setRotation(-90);
        QCOMPARE(o.inversePos(o.transformPos(QPoint{500, 50})), (QPoint{500, 50}));
    }

    void handlesRotationInFlatState() {
        GeometryCalculator o;
        o.setRotationState(RotationState::flat);

        o.setRotation(-91.0f);
        QCOMPARE(o.getRotation(), -90.0f);

        o.setRotation(-45.0f);
        QCOMPARE(o.getRotation(), -45.0f);

        o.setRotation(0.0f);
        QCOMPARE(o.getRotation(), 0.0f);

        o.setRotation(45.0f);
        QCOMPARE(o.getRotation(), 45.0f);

        o.setRotation(91.0f);
        QCOMPARE(o.getRotation(), 90.0f);
    }

    void handlesRotationInDownState() {
        GeometryCalculator o;
        o.setRotationState(RotationState::down);

        o.setRotation(-1.0f);
        QCOMPARE(o.getRotation(), 0.0f);

        o.setRotation(45.0f);
        QCOMPARE(o.getRotation(), 45.0f);

        o.setRotation(91.0f);
        QCOMPARE(o.getRotation(), 90.0f);
    }

    void handlesRotationInUpState() {
        GeometryCalculator o;
        o.setRotationState(RotationState::up);

        o.setRotation(1.0f);
        QCOMPARE(o.getRotation(), 0.0f);

        o.setRotation(-45.0f);
        QCOMPARE(o.getRotation(), -45.0f);

        o.setRotation(-91.0f);
        QCOMPARE(o.getRotation(), -90.0f);
    }

    void calculatesMainDirection() {
        GeometryCalculator o;
        QCOMPARE(o.getMainDirection(), (QPoint{100, 0}));

        o.setRotation(90);
        QCOMPARE(o.getMainDirection(), (QPoint{0, 100}));

        o.setRotationState(RotationState::up);
        o.setRotation(-90);
        QCOMPARE(o.getMainDirection(), (QPoint{0, -100}));
    }
};

QTEST_MAIN(TstGeometryCalculator)

#include "tst_geometrycalculator.moc"
