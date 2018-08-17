import QtQuick 2.0

Rectangle {
    property Item reference
    property real bl_x: 0.0
    property real bl_y: 0.0
    property real tr_x: 1.0
    property real tr_y: 1.0

    x: reference.x + bl_x * reference.width
    y: reference.y + (1 - tr_y) * reference.height
    width: reference.width * (tr_x - bl_x)
    height: reference.height * (tr_y - bl_y)
}
