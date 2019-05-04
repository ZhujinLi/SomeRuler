import QtQuick 2.2
import QtQuick.Controls 1.4
import Qt.labs.platform 1.0

ApplicationWindow {
    id: window
    flags: Qt.FramelessWindowHint
    visible: true
    title: qsTr("QkRuler")
    width: 400
    height: 75
    color: "transparent"

    function appear() {
        window.show()
        window.raise()
        window.requestActivate()
    }

    SystemTrayIcon {
        visible: true
        iconSource: "qrc:/images/qkruler-icon.png"

        menu: Menu {
            MenuItem {
                text: qsTr("Show")
                onTriggered: appear()
            }
            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }
    }

    Canvas {
        id: canvas
        width: parent.width
        height: parent.height
        focus: true
        Keys.onEscapePressed: window.hide()

        onPaint: {
            var ctx = canvas.getContext('2d')

            // Background
            ctx.rect(0, 0, width, height)
            ctx.fillStyle = "#80ffffff"
            ctx.fill()

            // Ticks
            ctx.strokeStyle = "black"
            ctx.beginPath()
            for (var tick = 0; tick < width; tick++) {
                if (tick % 2 == 0) {
                    var len = tick % 100 == 0 ? 15 : tick % 10 == 0 ? 10 : 5
                    ctx.moveTo(tick, 0)
                    ctx.lineTo(tick, len)
                }
            }
            ctx.stroke()

            // Labels
            ctx.fillStyle = "black"
            ctx.beginPath()
            for (tick = 0; tick < width; tick++) {
                if (tick % 100 == 0) {
                    ctx.text(tick, tick, 25)
                }
            }
            ctx.fill()
        }
    }
}

