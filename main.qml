import QtQuick 2.2
import QtQuick.Controls 1.4
import Qt.labs.platform 1.0

ApplicationWindow {
    id: window
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    visible: true
    title: qsTr("QkRuler")
    width: 400
    height: 90
    color: "transparent"

    function appear() {
        window.show()
        window.raise()
        window.requestActivate()
    }

    SystemTrayIcon {
        visible: true
        iconSource: "qrc:/images/qkruler-icon.png"

        onActivated: {
            if (reason === SystemTrayIcon.DoubleClick)
                appear()
        }

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

    // Resize
    MouseArea {
        property int pressX: 0

        width: 5

        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        cursorShape: Qt.SizeHorCursor

        onPressed: pressX = mouseX

        onMouseXChanged: {
            var dx = mouseX - pressX
            var width = Math.max(150, parent.width + dx)
            window.setWidth(width)
        }
    }

    Canvas {
        id: canvas
        width: parent.width
        height: parent.height
        focus: true
        Keys.onEscapePressed: window.hide()

        property string clickArea: "none"
        property point clickPos: "0, 0"

        // Select
        MouseArea {
            property bool clickValid: false

            anchors.fill: parent
            anchors.rightMargin: 5

            onPressed: clickValid = true
            onPositionChanged: clickValid = false

            onClicked: {
                if (clickValid) {
                    parent.clickArea = "select"
                    parent.clickPos = Qt.point(mouseX, mouseY)
                    parent.requestPaint()
                }
            }
        }

        // Drag
        MouseArea {
            property point pressPos: "0, 0"
            property bool clickValid: false

            anchors.fill: parent
            anchors.rightMargin: 5
            anchors.topMargin: 15
            anchors.bottomMargin: 15

            onPressed: {
                pressPos = Qt.point(mouseX, mouseY)
                clickValid = true
            }

            onClicked: {
                if (clickValid) {
                    parent.clickArea = "drag"
                    parent.clickPos = Qt.point(mouseX, mouseY)
                    parent.requestPaint()
                }
            }

            onPositionChanged: {
                var delta = Qt.point(mouseX-pressPos.x, mouseY-pressPos.y)
                window.x += delta.x;
                window.y += delta.y;
                clickValid = false
            }
        }

        onPaint: {
            var ctx = canvas.getContext('2d')
            ctx.clearRect(0, 0, width, height)

            // Background
            ctx.rect(0, 0, width, height)
            ctx.fillStyle = "#c0ffffff"
            ctx.fill()

            // Ticks
            for (var tick = 0; tick < width; tick++) {
                if (tick % 2 == 0) {
                    ctx.beginPath()
                    ctx.strokeStyle = clickArea == "select" && (parseInt(clickPos.x) | 1) === (tick + 1) ? "red" : "black"
                    var len = tick % 100 == 0 ? 15 : tick % 10 == 0 ? 10 : 5
                    ctx.moveTo(tick, 0)
                    ctx.lineTo(tick, len)

                    ctx.moveTo(tick, height)
                    ctx.lineTo(tick, height - len)
                    ctx.stroke()
                }
            }

            // Labels
            ctx.fillStyle = "black"
            for (tick = 0; tick < width; tick++) {
                if (tick % 100 == 0) {
                    ctx.textBaseline = "top"
                    ctx.fillText(tick, tick, 15)

                    ctx.textBaseline = "bottom"
                    ctx.fillText(tick, tick, height - 15)
                }
            }

            // Info
            ctx.textBaseline = "middle"
            if (clickArea == "none" || clickArea == "drag") {
                ctx.fillStyle = "black"
                ctx.fillText(width + ' px', 15, height / 2)
            } else if (clickArea == "select") {
                ctx.fillStyle = "red"
                ctx.fillText(Math.round(clickPos.x) + ' px', 15, height / 2)
            }
        }
    }
}

