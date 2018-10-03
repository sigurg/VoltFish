import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    id: page

    allowedOrientations: Orientation.Portrait

    Timer {
        running: true
        repeat: true
        interval: 1000
        triggeredOnStart: true
        onTriggered: {
            if (!blescanner.scanning)
                blescanner.startDeviceDiscovery()
        }
    }

    Column {
        anchors.fill: parent
        spacing: Theme.paddingMedium

        PageHeader {
            id: header
            //% "Connect your Mooshimeter"
            //: page title of device discovery page
            title: qsTrId("discovery-title")
        }

        SilicaListView {
            anchors.top: header.bottom
            width: parent.width - 2*Theme.paddingMedium
            id: view
            currentIndex: -1
            model: blescanner.devicesList
            anchors.horizontalCenter: parent.horizontalCenter

            ViewPlaceholder {
                enabled: view.count == 0
                //% "No Mooshimeters found"
                //: placeholder text on discovery page, if devices list is empty
                text: qsTrId("discovery-no-devices")
            }

            delegate: BackgroundItem {
                id: backgroundItem

                ListView.onAdd: AddAnimation {
                    target: backgroundItem
                }
                ListView.onRemove: RemoveAnimation {
                    target: backgroundItem
                }


                Column {
                    Label {
                        text: modelData.name
                        color: Theme.primaryColor
                        font.pixelSize: Theme.fontSizeLarge
                    }
                    Label {
                        text: modelData.address
                        color: Theme.secondaryColor
                        font.pixelSize: Theme.fontSizeMedium
                    }
                }

                onClicked: {
                    mooshimeter.btaddr = modelData.address
                    pageStack.navigateBack(PageStackAction.Animated)
                }
            }
        }
    }
}
