import QtQuick 2.0
import Sailfish.Silica 1.0
import "util.js" as Util

CoverBackground {
    Column {
        spacing: Theme.paddingMedium
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
                font.pixelSize: Theme.fontSizeLarge * 1.2
                font.bold: true
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
                font.pixelSize: Theme.fontSizeLarge * 1.2
                font.bold: true
                anchors.right: parent.right
            }
        }

        Column {
            width: parent.width
            spacing: Theme.paddingSmall

            Label {
                text: qsTrId(mooshimeter.model_math()[mooshimeter.math_mode])
                color: Theme.secondaryColor
                font.pixelSize: Theme.fontSizeSmall
            }

            Label {
                id: cover_math
                text: mooshimeter.math
                color: Theme.highlightColor
                font.pixelSize: Theme.fontSizeLarge
                font.bold: true
                anchors.right: parent.right
            }
        }

    }
}

