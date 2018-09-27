import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page

    allowedOrientations: Orientation.Portrait

    Column {
        width: page.width
        spacing: Theme.paddingLarge

        PageHeader {
            title: qsTrId("about")
        }


        Image {
            id: icon
            source: "/usr/share/icons/hicolor/128x128/apps/VoltFish.png"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: appName + " v" + appVersion
            color: Theme.primaryColor
            font.pixelSize: Theme.fontSizeLarge
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: qsTrId("about-text")
            width: parent.width
            color: Theme.secondaryColor
            font.pixelSize: Theme.fontSizeMedium
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            horizontalAlignment: Text.AlignHCenter
            onLinkActivated : Qt.openUrlExternally(link)
        }

        Button {
            text: qsTrId("button-report-issue")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: Qt.openUrlExternally("https://github.com/sigurg/VoltFish/issues")
        }

        Button {
            text: qsTrId("button-github-page")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: Qt.openUrlExternally("https://github.com/sigurg/VoltFish")
        }

    }
}
