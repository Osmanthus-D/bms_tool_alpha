#ifndef STRUCT_H
#define STRUCT_H

#define MODBUS_DEVICE_LEN               (128)
#define MODBUS_COILS_READ_LEN           (52)
#define MODBUS_REGISTERS_READ_LEN       (30)
#define MODBUS_DEVICE_ID_ADDR           (1000)
#define MODBUS_DEVICE_ID_LEN            (13)
#define BUFFER_LEN                      (512)

#define OC_PROTECT_PREFIX_LEN           (12)
#define OD_PROTECT_PREFIX_LEN           (15)
#define VOLTAGE_STANDARD_LINE_NUM       (5)
#define WARNING_NUM                     (11)
#define CORE_VOLTAGE_NUM                (20)

#define MODBUS_DEBUG_LOG_FILENAME       "modbus_debug_log.txt"
#define MODBUS_ERROR_LOG_FILENAME       "modbus_error_log.txt"

typedef struct _modbus_device_desc_t_
{
    char device[MODBUS_DEVICE_LEN];
    int baud;
    char parity;
    int data_bit;
    int stop_bit;
}device_desc_t;

typedef struct _battery_desc_t_
{
    quint16 outputVoltage;  //电池包实际总电压
    quint16 numOfCores;     //电芯数量
    quint16 soc;            //电量SOC
    quint16 uncertain;      //uncertain;
    quint16 soh;            //SOH
    qint16 chargingCurrent; //充电电流
    qint16 ambientTemp;     //环境温度
    qint16 coreTemp1;       //电芯温度1
    qint16 coreTemp2;       //电芯温度2
    qint16 boardTemp;       //板卡温度
    quint16 coreVolt1;      //电芯电压1
    quint16 coreVolt2;      //电芯电压2
    quint16 coreVolt3;      //电芯电压3
    quint16 coreVolt4;      //电芯电压4
    quint16 coreVolt5;      //电芯电压5
    quint16 coreVolt6;      //电芯电压6
    quint16 coreVolt7;      //电芯电压7
    quint16 coreVolt8;      //电芯电压8
    quint16 coreVolt9;      //电芯电压9
    quint16 coreVolt10;     //电芯电压10
    quint16 coreVolt11;     //电芯电压11
    quint16 coreVolt12;     //电芯电压12
    quint16 coreVolt13;     //电芯电压13
    quint16 coreVolt14;     //电芯电压14
    quint16 coreVolt15;     //电芯电压15
    quint16 coreVolt16;     //电芯电压16
    quint16 coreVolt17;     //电芯电压17
    quint16 coreVolt18;     //电芯电压18
    quint16 coreVolt19;     //电芯电压19
    quint16 coreVolt20;     //电芯电压20
    quint16 id[13];
} battery_desc_t;

#endif // STRUCT_H
