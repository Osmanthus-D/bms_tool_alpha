#ifndef SERIALPORTHELPER_H
#define SERIALPORTHELPER_H

#include <QDebug>
#include <QSerialPort>
#include "preference.h"

class SerialPortHelper
{
public:
    SerialPortHelper();
    ~SerialPortHelper();
    bool setPort(QSerialPort *port);
    QString portName();
    void setPortName(const QString &portName);
    bool setBaudRate(qint32 baudRate);
    bool setDataBits(QSerialPort::DataBits dataBits);
    bool setParity(QSerialPort::Parity parity);
    bool setStopBits(QSerialPort::StopBits stopBits);
    bool setFlowControl(QSerialPort::FlowControl flowControl);

protected:
    QSerialPort *serialPort;
    bool initPort();
    bool openPort();
    void closePort();
    QString genPortSum() const;

private:
    Preference *pref;
};

#endif // SERIALPORTHELPER_H
