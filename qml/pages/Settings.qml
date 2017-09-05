import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page

    allowedOrientations: Orientation.All

    Column {
        anchors.fill: parent

        PageHeader {
            title: qsTr("Settings")
        }


        TextField {
            id: bt
            focus: true
            label: qsTr("Bluetooth Address")
            placeholderText: label
            text: mooshimeter.btaddr
            width: parent.width
            EnterKey.enabled: text.length == 17
            EnterKey.onClicked: mooshimeter.btaddr = text
        }

        ComboBox {
            label: qsTr("Temperature Unit")
            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Kelvin")
                    onClicked: mooshimeter.temp_unit = 0
                }
                MenuItem {
                    text: qsTr("Celcius")
                    onClicked: mooshimeter.temp_unit = 1
                }
                MenuItem {
                    text: qsTr("Fahrenheit")
                    onClicked: mooshimeter.temp_unit = 2
                }
            }
        }

        Column {
            spacing: Theme.paddingLarge
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: qsTr("Reboot Mooshimeter")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: mooshimeter.reboot()
            }

            Button {
                text: qsTr("Shipping Mode")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: mooshimeter.shipping_mode()
            }
        }
    }
}
