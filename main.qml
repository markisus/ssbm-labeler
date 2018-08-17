import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

Window {
    visible: true
    width: 1300
    height: 800
    title: qsTr("SSBM Labeler")

    // amount to increment the video when advancing
    property int dt_msecs: 100;

    Item {
        id: main
        anchors.fill: parent
        focus: true

        property int current_time_msecs: 0

        Image {
            anchors.horizontalCenter: main.horizontalCenter
            id: ssbm_image
            asynchronous: false
            source: "image://ssbm_video/" + parent.current_time_msecs
         }

        BoxFinder {
            id: box_finder
            parent_item: ssbm_image
            box_color: Qt.rgba(0, 1, 0, 0.2)
            box_border.color: Qt.rgba(0.5, 1, 0.5, 1)
        }

        Slider {
            id: seek_bar
            anchors.bottom: main.bottom
            value: 0
            from: 0
            to: max_msec
            anchors.left: main.left
            anchors.right: main.right
            stepSize: 25
            live: false
            onValueChanged: {
                console.log("Value is now ", value)
                main.current_time_msecs = seek_bar.value
            }
        }

        Keys.onPressed: {
            console.log("Key pressed")
            if (event.key == Qt.Key_Shift) {
                console.log("shrink");
                event.accepted = true;
                box_finder.ok()
            }
            if (event.key == Qt.Key_Space) {
                console.log("grow");
                event.accepted = true;
                box_finder.not_ok()
            }

            if (event.key == Qt.Key_QuoteLeft) {
                console.log("bq")
                boundary_finder.visible =
                        !boundary_finder.visible
            }
        }

    }
}
