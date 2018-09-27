import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page

    allowedOrientations: Orientation.Portrait

    Column {
        anchors.fill: parent

        PageHeader {
            title: qsTrId("settings")
        }

        Label {
            text: qsTrId("firmware-note")
            width: parent.width
            wrapMode: TextInput.WordWrap
            padding: Theme.paddingMedium
        }

        TextField {
            id: bt
            focus: true
            label: qsTrId("bluetooth-address")
            placeholderText: label
            text: mooshimeter.btaddr
            width: parent.width
            EnterKey.enabled: text.length == 17
            EnterKey.onClicked: mooshimeter.btaddr = text
            EnterKey.iconSource: "image://theme/icon-m-enter-accept"
        }

        ComboBox {
            label: qsTrId("temperature-unit") + ":"
            currentIndex: mooshimeter.temp_unit
            menu: ContextMenu {
                Repeater {
                    model: [qsTrId("temp-unit-kelvin"), qsTrId("temp-unit-celcius"), qsTrId("temp-unit-fahrenheit")]
                    MenuItem {
                        text: modelData
                        onClicked: mooshimeter.temp_unit = index
                    }
                }
            }
        }

        Column {
            spacing: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter

            RemorsePopup { id: remorse }

            Button {
                text: qsTrId("mooshimeter-reboot")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: remorse.execute(qsTrId("remorse-reboot"), mooshimeter.reboot)
            }

            Button {
                text: qsTrId("mooshimeter-shipping-mode")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: remorse.execute(qsTrId("remorse-shipping-mode"), mooshimeter.shipping_mode)
            }
        }
    }
}
