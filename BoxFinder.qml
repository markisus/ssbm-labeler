import QtQuick 2.0

Item {
    property Item parent_item

    property real bl_x: 0.0
    property real tr_x: 1.0
    property real tr_y: 1.0
    property real bl_y: 0.0

    property alias box_color: box.color
    property alias box_border: box.border

    NDCRectangle {
        reference: parent_item
        id: box

        bl_x: parent.bl_x
        bl_y: parent.bl_y
        tr_x: parent.tr_x
        tr_y: parent.tr_y
    }

    property string mode: "left"

    BinarySearcher {
        id: top_finder
        high: 1.0
        low: 0.0
    }

    BinarySearcher {
        id: left_finder
        high: 0.0
        low: 1.0
    }

    BinarySearcher {
        id: right_finder
        high: 1.0
        low: 0.0
    }

    BinarySearcher {
        id: bottom_finder
        high: 0.0
        low: 1.0
    }

    readonly property var mode_to_finder: {
        "left": left_finder,
        "right": right_finder,
        "top": top_finder,
        "bottom": bottom_finder,
        "sex": false,
    }

    readonly property var mode_to_setter: {
        "left": function(s){bl_x = s},
        "right": function(s){tr_x = s},
        "bottom": function(s){bl_y = s},
        "top": function(s){tr_y = s},
    }

   readonly property var mode_to_next_mode: {
        "left": "bottom",
        "bottom": "right",
        "right": "top",
        "top": "left"
    }

    function ok() {
        mode_to_finder[mode].ok()
        advance_mode()
    }

    function not_ok() {
        mode_to_finder[mode].not_ok()
        mode_to_setter[mode](mode_to_finder[mode].mid)
    }

    function advance_mode() {
        console.log("Advancing mode")
        var min_viable_finder_width = 0.001
        for (var i = 0; i < 4; ++i) {
            mode = mode_to_next_mode[mode]
            mode_to_setter[mode](mode_to_finder[mode].mid)
            var finder_width = Math.abs(
                        mode_to_finder[mode].high - mode_to_finder[mode].low)
            if (finder_width > min_viable_finder_width) {
                break
            }
        }

        console.log("coords ", bl_x, bl_y, tr_x, tr_y)
        // After mode advanced, the rectangle might have to be increased
        // Also prevent infinite loop
        var max_not_oks = 10
        var not_oks = 0
        while (bl_x >= tr_x && not_oks < max_not_oks) {
            console.log("box not ok, adjusting")
            not_ok()
        }
        while (tr_y <= bl_y && not_oks < max_not_oks) {
            console.log("box not ok, adjusting")
            not_ok()
        }
    }

    Component.onCompleted: {
        mode_to_setter[mode](mode_to_finder[mode].mid)
    }
}
