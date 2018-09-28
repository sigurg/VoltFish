function mapping_to_str(m, a) {
    switch(m){
        case 0:
            if (a === 1)
                return qsTrId("mode-voltage-ac")
            else
                return qsTrId("mode-voltage-dc")
        case 1:
            if (a === 1)
                return qsTrId("mode-current-ac")
            else
                return qsTrId("mode-current-dc")
        case 2:
            return qsTrId("mode-temperature")
        case 3:
            if (a === 1)
                return qsTrId("mode-voltage-aux-ac")
            else
                return qsTrId("mode-voltage-aux-dc")
        case 4:
            return qsTrId("mode-resistance")
        case 5:
            return qsTrId("mode-diode")
        default:
            return ""
    }
}
