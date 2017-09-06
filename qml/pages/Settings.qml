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
            currentIndex: mooshimeter.temp_unit
            menu: ContextMenu {
                Repeater {
                    model: [qsTr("Kelvin"), qsTr("Celcius"), qsTr("Fahrenheit")]
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
