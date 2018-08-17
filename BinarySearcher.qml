import QtQuick 2.0

Item {
    id: boundary_finder
    property real low: 0.0
    property real high: 1.0
    property real mid: 0.5*(low + high)

    function ok() {
        high = mid
    }

    function not_ok() {
        low = mid
    }
}
