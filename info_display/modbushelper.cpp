#include "modbushelper.h"

modbus_t *ModbusHelper::mb = NULL;

ModbusHelper::ModbusHelper(QObject *parent) : QObject(parent)
{
    modbusDebugLog = freopen(MODBUS_DEBUG_LOG_FILENAME, "w", stdout);
    modbusErrorLog = freopen(MODBUS_ERROR_LOG_FILENAME, "w", stderr);
}

ModbusHelper::~ModbusHelper()
{
    if(modbusDebugLog) {
        fclose(modbusDebugLog);
    }

    if(modbusErrorLog) {
        fclose(modbusErrorLog);
    }
}

void ModbusHelper::rtuRead()
{
    if(mb)
    {
        int ret = modbus_read_input_bits(mb, 0, MODBUS_COILS_READ_LEN, buffer);

        ret = modbus_read_registers(mb, 0, MODBUS_REGISTERS_READ_LEN,
                                    (quint16 *)(buffer + MODBUS_COILS_READ_LEN));

        ret = modbus_read_registers(mb, MODBUS_DEVICE_ID_ADDR, MODBUS_DEVICE_ID_LEN,
                                    (quint16 *)(buffer + MODBUS_COILS_READ_LEN + MODBUS_REGISTERS_READ_LEN * 2));
        Q_UNUSED(ret)

        emit refreshCorePortectSig(buffer);
    }    
}
