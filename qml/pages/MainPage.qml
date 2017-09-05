import QtQuick 2.0
import QtQuick.Window 2.2
import Sailfish.Silica 1.0
import "../lib"

Page {
    id: page

    allowedOrientations: Orientation.Portrait

    ScreenBlank {
        enabled: Window.active && page.status == PageStatus.Active
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(Qt.resolvedUrl("Settings.qml"))
            }
        }

        contentHeight: column.height

        Component.onCompleted: {
            if (mooshimeter.btaddr != "")
                mooshimeter.connect()
        }

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: qsTr("Mooshimeter")
                Label {
                    anchors.left: parent.left
                    anchors.margins: 20
                    y: Theme.horizontalPageMargin
                    visible: (mooshimeter.bat_percent > 0)
                    text: qsTr("Bat: ") + mooshimeter.bat_percent + "%"
                    color: Theme.secondaryColor
                }
            }

            Column {
                spacing: Theme.paddingSmall
                Label {
                    id: ch1_label
                    anchors.rightMargin: 20
                    x: Theme.horizontalPageMargin
                    text: mooshimeter.ch1
                    color: Theme.highlightColor
                    font.pixelSize: 100
                    anchors.right: parent.right
                }

                Row {
                    ComboBox {
                        id: ch1_mode
                        width: page.width/1.8

                        menu: ContextMenu {
                            MenuItem {
                                text: qsTr("Current D/C")
                                onClicked: {
                                    mooshimeter.ch1_mapping = 1
                                    mooshimeter.ch1_analysis = 0
                                }
                            }
                            MenuItem {
                                text: qsTr("Current A/C")
                                onClicked: {
                                    mooshimeter.ch1_mapping = 1
                                    mooshimeter.ch1_analysis = 1
                                }
                            }
                            MenuItem {
                                text: qsTr("Aux. Voltage D/C")
                                onClicked: {
                                    mooshimeter.ch1_mapping = 3
                                    mooshimeter.ch1_analysis = 0
                                }
                            }
                            MenuItem {
                                text: qsTr("Aux. Voltage A/C")
                                onClicked: {
                                    mooshimeter.ch1_mapping = 3
                                    mooshimeter.ch1_analysis = 1
                                }
                            }
                            MenuItem {
                                text: qsTr("Resistance")
                                onClicked: {
                                    mooshimeter.ch1_mapping = 4
                                    mooshimeter.ch1_analysis = 0
                                }
                            }
                            MenuItem {
                                text: qsTr("Diode")
                                onClicked: {
                                    mooshimeter.ch1_mapping = 5
                                    mooshimeter.ch1_analysis = 0
                                }
                            }
                            MenuItem {
                                text: qsTr("Temperature")
                                onClicked: {
                                    mooshimeter.ch1_mapping = 2
                                    mooshimeter.ch1_analysis = 0
                                }
                            }
                        }
                    }

                    ComboBox {
                        id: ch1_range
                        width: page.width/2.2
                        menu: ContextMenu {
                            Repeater {
                                model: mooshimeter.model_ch1_range
                                MenuItem {
                                    text: modelData
                                    onClicked: mooshimeter.ch1_range = modelData
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                visible: false
                height: 100
            }

            Column {
                spacing: Theme.paddingSmall
                Label {
                    id: ch2_label
                    anchors.rightMargin: 20
                    x: Theme.horizontalPageMargin
                    text: mooshimeter.ch2
                    color: Theme.highlightColor
                    font.pixelSize: 100
                    anchors.right: parent.right
                }

                Row {
                    ComboBox {
                        width: page.width/1.8
                        id: ch2_mode
                        menu: ContextMenu {
                            MenuItem {
                                text: qsTr("Voltage D/C")
                                onClicked: {
                                    mooshimeter.ch2_mapping = 0
                                    mooshimeter.ch2_analysis = 0
                                }
                            }
                            MenuItem {
                                text: qsTr("Voltage A/C")
                                onClicked: {
                                    mooshimeter.ch2_mapping = 0
                                    mooshimeter.ch2_analysis = 1
                                }
                            }
                            MenuItem {
                                text: qsTr("Aux. Voltage D/C")
                                onClicked: {
                                    mooshimeter.ch2_mapping = 3
                                    mooshimeter.ch2_analysis = 0
                                }
                            }
                            MenuItem {
                                text: qsTr("Aux. Voltage A/C")
                                onClicked: {
                                    mooshimeter.ch2_mapping = 3
                                    mooshimeter.ch2_analysis = 1
                                }
                            }
                            MenuItem {
                                text: qsTr("Resistance")
                                onClicked: {
                                    mooshimeter.ch2_mapping = 4
                                    mooshimeter.ch2_analysis = 0
                                }
                            }
                            MenuItem {
                                text: qsTr("Diode")
                                onClicked: {
                                    mooshimeter.ch2_mapping = 5
                                    mooshimeter.ch2_analysis = 0
                                }
                            }
                            MenuItem {
                                text: qsTr("Temperature")
                                onClicked: {
                                    mooshimeter.ch2_mapping = 2
                                    mooshimeter.ch2_analysis = 0
                                }
                            }
                        }
                    }

                    ComboBox {
                        id: ch2_range
                        width: page.width/2.2
                        menu: ContextMenu {
                            Repeater {
                                model: mooshimeter.model_ch2_range
                                MenuItem {
                                    text: modelData
                                    onClicked: mooshimeter.ch2_range = modelData
                                }
                            }
                        }
                    }
                }
            }


            Row {
                ComboBox {
                    id: smpl_rate
                    width: page.width/1.8
                    label: qsTr("Sampling Rate")
                    currentIndex: 3

                    menu: ContextMenu {
                        Repeater {
                            model: ["125", "250", "500", "1000", "2000", "4000", "8000"]
                            MenuItem {
                                text: modelData
                                onClicked: mooshimeter.rate = modelData
                            }
                        }
                    }
                }

                ComboBox {
                    id: buf_depth
                    width: page.width/2
                    label: qsTr("Buffer Depth")
                    currentIndex: 2

                    menu: ContextMenu {
                        Repeater {
                            model: ["32", "64", "128", "256"]
                            MenuItem {
                                text: modelData
                                onClicked: mooshimeter.depth = modelData
                            }
                        }
                    }
                }
            }
        }
    }
}

