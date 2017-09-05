import QtQuick 2.0
import Sailfish.Silica 1.0
import "util.js" as Util

CoverBackground {
    Column {
        spacing: Theme.paddingLarge
        anchors.centerIn: parent
        width: parent.width - 20

        Column {
            width: parent.width
            spacing: Theme.paddingSmall

            Label {
                text: Util.mapping_to_str(mooshimeter.ch1_mapping, mooshimeter.ch1_analysis)
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
            }
            Label {
                id: cover_ch1
                text: mooshimeter.ch1
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeExtraLarge
                anchors.right: parent.right
            }
        }

        Column {
            width: parent.width
            spacing: Theme.paddingSmall

            Label {
                text: Util.mapping_to_str(mooshimeter.ch2_mapping, mooshimeter.ch2_analysis)
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
            }

            Label {
                id: cover_ch2
                text: mooshimeter.ch2
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeExtraLarge
                anchors.right: parent.right
            }
        }
    }
}

