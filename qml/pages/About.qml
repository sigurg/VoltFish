import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page

    allowedOrientations: Orientation.Portrait

    Column {
        anchors.fill: parent

        PageHeader {
            title: qsTr("About")
        }
    }
}
