# VoltFish
[SailfishOS](https://sailfishos.org/) app for the [Mooshimeter](https://moosh.im/mooshimeter/) BluetoothLE Multimeter based on [pk8's mooshimeter lib](https://github.com/pk8/mooshimeter-lib).

This app is in early alpha stage, and has only been tested in my phone (Aquafish) with my meter, so expect plenty of bugs :)

**To use the app, you must enter the Bluetooth ID of your Mooshimeter in the app's settings dialog. Currently only 147xxx series firmware is supported, 152xxx does not work.**


## What works:
* display basic readings
* select input channels and ranges
* "math" channel (power calculations and K-Thermocouple)
* battery level indicator
* temperatur units (Celcius / Kelvin / Fahrenheit)

## What doesn't work:
* there is no scanning/connection dialog to connect to a meter
* logging is not yet supported
* there are no pretty graphs (not even ugly ones)
* probably a lot of other features I can't think of right now :)

## Known bugs:
* app sometimes fails to connect to Mooshimeter at startup and becomes unresponsive
