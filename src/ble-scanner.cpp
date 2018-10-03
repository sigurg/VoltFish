#include <QBluetoothUuid>
#include <QDebug>

#include "ble-scanner.h"



BLEScanner::BLEScanner(): scanning(false) {
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    connect(discoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
            this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
    connect(discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this, SLOT(scanError(QBluetoothDeviceDiscoveryAgent::Error)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));
}



BLEScanner::~BLEScanner() {
    delete discoveryAgent;
    qDeleteAll(mooshimeters);
    mooshimeters.clear();
}



void BLEScanner::startDeviceDiscovery() {
    qDeleteAll(mooshimeters);
    mooshimeters.clear();
    emit devicesUpdated();

    discoveryAgent->start();
    scanning = discoveryAgent->isActive();
}



void BLEScanner::addDevice(const QBluetoothDeviceInfo &info) {
    // Meter Service UUID, used to identify Mooshimeters
    static const QBluetoothUuid METER_SERVICE(QString("1bc5ffa0-0200-62ab-e411-f254e005dbd4"));

    if (info.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) 	// is BLE device
        if (info.majorDeviceClass() == QBluetoothDeviceInfo::MiscellaneousDevice)		// misc device
            if (info.serviceUuids().contains(METER_SERVICE)) {							// has meter service UUID
                mooshimeters.append(new BLEDevice(info));
                emit devicesUpdated();
            }
}



void BLEScanner::scanFinished() {
    emit devicesUpdated();
    scanning = false;
    emit stateChanged();
}



QVariant BLEScanner::getDevices() {
    return QVariant::fromValue(mooshimeters);
}



void BLEScanner::scanError(QBluetoothDeviceDiscoveryAgent::Error error) {
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        qDebug() << "The Bluetooth adaptor is powered off, power it on before doing discovery.";
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        qDebug() << "Writing or reading from the device resulted in an error.";
    else
        qDebug() << "An unknown error has occurred.";

    scanning = false;
    emit devicesUpdated();
    emit stateChanged();
}
