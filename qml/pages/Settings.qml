import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page

    allowedOrientations: Orientation.Portrait

    Column {
        anchors.fill: parent

        PageHeader {
            title: qsTr("Settings")
        }

        Label {
            text: qsTr("Note:\nrequires Mooshimeter firmware 147xxx")
            width: parent.width
            wrapMode: TextInput.WordWrap
            padding: Theme.paddingMedium
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
            EnterKey.iconSource: "image://theme/icon-m-enter-accept"
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

            RemorsePopup { id: remorse }

            Button {
                text: qsTr("Reboot Mooshimeter")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: remorse.execute(qsTr("Rebooting Mooshimeter"), mooshimeter.reboot)
            }

            Button {
                text: qsTr("Shipping Mode")
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: remorse.execute(qsTr("Enter shipping mode"), mooshimeter.shipping_mode)
            }
        }
    }
}
