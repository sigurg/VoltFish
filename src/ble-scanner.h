#pragma once

#include <QObject>
#include <QVariant>
#include <QBluetoothDeviceDiscoveryAgent>


class BLEDevice: public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER name NOTIFY deviceChanged);
    Q_PROPERTY(QString address MEMBER address NOTIFY deviceChanged);

Q_SIGNALS:
    void deviceChanged();

public:
    BLEDevice(const QString &name, const QString &address):
        name(name),
        address(address) {
        emit deviceChanged();
    };

    BLEDevice(const QBluetoothDeviceInfo &d):
        name(d.name()),
        address(d.address().toString()) {
        emit deviceChanged();
    };

private:
    QString name;
    QString address;
};



class BLEScanner: public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariant devicesList READ getDevices NOTIFY devicesUpdated)
    Q_PROPERTY(bool scanning READ getScanning NOTIFY stateChanged)
    //Q_PROPERTY(bool controllerError READ hasControllerError)
public:
    BLEScanner();
    ~BLEScanner();
    QVariant getDevices();
    QString getUpdate();
    bool getScanning() {
        return scanning;
    };

public slots:
    void startDeviceDiscovery();

private slots:
    void addDevice(const QBluetoothDeviceInfo &);
    void scanFinished();
    void scanError(QBluetoothDeviceDiscoveryAgent::Error);

Q_SIGNALS:
    void devicesUpdated();
    void updateChanged();
    void stateChanged();

private:
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QList<QObject *> mooshimeters;
    bool scanning;

};
