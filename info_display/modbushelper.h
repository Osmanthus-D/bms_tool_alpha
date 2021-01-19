#ifndef MODBUSHELPER_H
#define MODBUSHELPER_H

#include <QObject>
#include <QDebug>
#include <modbus.h>
#include "defs.h"

class ModbusHelper : public QObject
{
    Q_OBJECT

public:
    explicit ModbusHelper(QObject *parent = 0);
    ~ModbusHelper();
    static modbus_t *mb;

signals:
    void refreshCorePortectSig(uint8_t * a);
    void refreshSOC(QVariant info);

public slots:
    void rtuRead();

private:
    uint8_t buffer[BUFFER_LEN] = {0};
    FILE *modbusDebugLog;
    FILE *modbusErrorLog;

    void rtuReadSwitch();
    void rtuReadRegister();
};

#endif // MODBUSHELPER_H
