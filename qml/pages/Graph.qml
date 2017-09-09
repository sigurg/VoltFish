import QtQuick 2.0
import Sailfish.Silica 1.0
import "../lib"


Page {
    id: page

    allowedOrientations: defaultAllowedOrientations

    ScreenBlank {
        enabled: Window.active && page.status == PageStatus.Active
    }
}
