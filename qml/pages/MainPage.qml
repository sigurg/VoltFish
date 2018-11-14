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

    Timer {
        id: connectTimer
        running: false
        repeat: false
        interval: 500
        onTriggered: {
            if (mooshimeter.btaddr != "")
                mooshimeter.connect()
            else
                pageStack.push(Qt.resolvedUrl("discovery.qml"))
        }
    }

    /*
    onStatusChanged: {
        if (status === PageStatus.Active) {
            pageStack.pushAttached(Qt.resolvedUrl("Graph.qml"))
        }
    }
    */

    Connections {
        target: mooshimeter
        onCh1Config: ch1_range.currentIndex = mooshimeter.ch1_range
        onCh2Config: ch2_range.currentIndex = mooshimeter.ch2_range
    }

    SilicaFlickable {
        anchors.fill: parent

        PullDownMenu {
            Repeater {
                model: ["about", "settings"]
                MenuItem {
                    text: qsTrId(modelData)
                    onClicked: pageStack.push(Qt.resolvedUrl(modelData + ".qml"))
                }
            }
        }

        contentHeight: column.height

        Component.onCompleted: {
            connectTimer.start();
        }

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: appName
                Label {
                    anchors.left: parent.left
                    anchors.margins: 20
                    y: Theme.horizontalPageMargin
                    visible: (mooshimeter.bat_percent > 0)
                    //% "Bat"
                    //: battery indicator label
                    text: qsTrId("battery-abbrev") + ": " + mooshimeter.bat_percent + "%"
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
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeHuge * 1.5
                    font.bold: true
                    anchors.right: parent.right
                }

                Row {
                    ComboBox {
                        id: ch1_mode
                        width: page.width/1.8

                        menu: ContextMenu {
                            Repeater {
                                model: [
                                    //% "Current D/C"
                                    //: measurement mode: electrical current (D/C)
                                    {"label":qsTrId("mode-current-dc"), "mapping":1, "analysis":0},
                                    //% "Current A/C"
                                    //: measurement mode: electrical current (A/C)
                                    {"label":qsTrId("mode-current-ac"), "mapping":1, "analysis":1},
                                    //% "Aux. Voltage D/C"
                                    //: measurement mode: high precision voltage (D/C)
                                    {"label":qsTrId("mode-voltage-aux-dc"), "mapping":3, "analysis":0},
                                    //% "Aux. Voltage A/C"
                                    //: measurement mode: high precision voltage (A/C)
                                    {"label":qsTrId("mode-voltage-aux-ac"), "mapping":3, "analysis":1},
                                    //% "Resistance"
                                    //: measurement mode: electrical resistance
                                    {"label":qsTrId("mode-resistance"), "mapping":4, "analysis":0},
                                    //% "Diode"
                                    //: measurement mode: diode forward voltage
                                    {"label":qsTrId("mode-diode"), "mapping":5,"analysis":0},
                                    //% "Temperature"
                                    //: measurement mode: temperature
                                    {"label":qsTrId("mode-temperature"), "mapping":2, "analysis":0}
                                ]

                                MenuItem {
                                    text: modelData.label
                                    onClicked: {
                                        mooshimeter.ch1_mapping = modelData.mapping
                                        mooshimeter.ch1_analysis = modelData.analysis
                                    }
                                }
                            }
                        }
                    }

                    ComboBox {
                        id: ch1_range
                        width: page.width/2.2
                        currentIndex: mooshimeter.ch1_range
                        menu: ContextMenu {
                            Repeater {
                                model: mooshimeter.model_ch1_range
                                MenuItem {
                                    text: modelData
                                    onClicked: mooshimeter.ch1_range = index
                                }
                            }
                        }
                    }
                }
            }

            Column {
                spacing: Theme.paddingSmall
                Label {
                    id: ch2_label
                    anchors.rightMargin: 20
                    x: Theme.horizontalPageMargin
                    text: mooshimeter.ch2
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeHuge * 1.5
                    font.bold: true
                    anchors.right: parent.right
                }

                Row {
                    ComboBox {
                        id: ch2_mode
                        width: page.width/1.8

                        menu: ContextMenu {
                            Repeater {
                                model: [
                                    //% "Voltage D/C"
                                    //: measurement mode: voltage (D/C)
                                    {"label":qsTrId("mode-voltage-dc"), "mapping":0, "analysis":0},
                                    //% "Voltage A/C"
                                    //: measurement mode: voltage (A/C)
                                    {"label":qsTrId("mode-voltage-ac"), "mapping":0, "analysis":1},
                                    {"label":qsTrId("mode-voltage-aux-dc"), "mapping":3, "analysis":0},
                                    {"label":qsTrId("mode-voltage-aux-ac"), "mapping":3, "analysis":1},
                                    {"label":qsTrId("mode-resistance"), "mapping":4, "analysis":0},
                                    {"label":qsTrId("mode-diode"), "mapping":5,"analysis":0},
                                    {"label":qsTrId("mode-temperature"), "mapping":2, "analysis":0}
                                ]

                                MenuItem {
                                    text: modelData.label
                                    onClicked: {
                                        mooshimeter.ch2_mapping = modelData.mapping
                                        mooshimeter.ch2_analysis = modelData.analysis
                                    }
                                }
                            }
                        }
                    }

                    ComboBox {
                        id: ch2_range
                        width: page.width/2.2
                        currentIndex: mooshimeter.ch2_range
                        menu: ContextMenu {
                            Repeater {
                                model: mooshimeter.model_ch2_range
                                MenuItem {
                                    text: modelData
                                    onClicked: mooshimeter.ch2_range = index
                                }
                            }
                        }
                    }
                }
            }

            Column {
                width: parent.width
                spacing: Theme.paddingSmall

                Label {
                    id: math_label
                    anchors.rightMargin: 20
                    x: Theme.horizontalPageMargin
                    text: qsTrId(mooshimeter.math)
                    color: Theme.primaryColor
                    font.pixelSize: Theme.fontSizeHuge
                    font.bold: true
                    anchors.right: parent.right
                }

                ComboBox {
                    id: math_mode
                    width: parent.width
                    currentIndex: mooshimeter.math_mode
                    menu: ContextMenu {
                        Repeater {
                            model: mooshimeter.model_math()
                            MenuItem {
                                text: qsTrId(modelData)
                                onClicked: mooshimeter.math_mode = index
                            }
                        }
                    }
                }
            }


            Row {
                ComboBox {
                    id: smpl_rate
                    width: page.width/1.8
                    //% "Sample Rate"
                    //: measurement sample rate selection
                    label: qsTrId("sampling-rate")
                    currentIndex: 3

                    menu: ContextMenu {
                        Repeater {
                            model: mooshimeter.model_rate()
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
                    //% "Buffer Depth"
                    //: measurement buffer depth selection
                    label: qsTrId("buffer-depth")
                    currentIndex: 2

                    menu: ContextMenu {
                        Repeater {
                            model: mooshimeter.model_depth()
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

