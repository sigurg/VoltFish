import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page

    allowedOrientations: Orientation.Portrait

    Column {
        width: page.width
        spacing: Theme.paddingLarge

        PageHeader {
            title: qsTr("About")
        }


        Image {
            id: icon
            source: "/usr/share/icons/hicolor/128x128/apps/Mooshimeter.png"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: qsTr(appName + " v" + appVersion)
            color: Theme.primaryColor
            font.pixelSize: Theme.fontSizeLarge
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: qsTr("Inofficial Sailfish OS app for the <a href=\"\https://moosh.im/mooshimeter/\">Mooshimeter</a> Bluetooth low energy multimeter.")
            width: parent.width
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeMedium
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            onLinkActivated : Qt.openUrlExternally(link)
        }
    }
}
