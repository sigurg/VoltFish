function mapping_to_str(m, a) {
    switch(m){
        case 0:
            if (a == 1)
                return "Voltage A/C"
            else
                return "Voltage D/C"
            break
        case 1:
            if (a == 1)
                return "Current A/C"
            else
                return "Current D/C"
            break
        case 2:
            return "Temperature"
            break
        case 3:
            if (a == 1)
                return "Aux. Voltage A/C"
            else
                return "Aux. Voltage D/C"
            break
        case 4:
            return "Resistance"
            break
        case 5:
            return "Diode"
            break
        default:
            return ""
    }
}
