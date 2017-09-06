function mapping_to_str(m, a) {
    switch(m){
        case 0:
            if (a == 1)
                return qsTr("Voltage A/C")
            else
                return qsTr("Voltage D/C")
            break
        case 1:
            if (a == 1)
                return qsTr("Current A/C")
            else
                return qsTr("Current D/C")
            break
        case 2:
            return qsTr("Temperature")
            break
        case 3:
            if (a == 1)
                return qsTr("Aux. Voltage A/C")
            else
                return qsTr("Aux. Voltage D/C")
            break
        case 4:
            return qsTr("Resistance")
            break
        case 5:
            return qsTr("Diode")
            break
        default:
            return ""
    }
}
