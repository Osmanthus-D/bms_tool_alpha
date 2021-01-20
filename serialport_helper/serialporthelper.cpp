#include "serialporthelper.h"

SerialPortHelper::SerialPortHelper()
{
    pref = new Preference();
    serialPort = new QSerialPort();
}

SerialPortHelper::~SerialPortHelper()
{
    delete pref;
    delete serialPort;
}

bool SerialPortHelper::initPort()
{
    QString _parity = pref->parity();
    QSerialPort::DataBits dataBits = QSerialPort::DataBits(pref->dataBits());
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity parity;

    switch (pref->stopBits()) {
    case 1:
        stopBits = QSerialPort::OneStop;
        break;
    case 2:
        stopBits = QSerialPort::OneAndHalfStop;
        break;
    case 3:
        stopBits = QSerialPort::TwoStop;
        break;
    default:
        stopBits = QSerialPort::UnknownStopBits;
        break;
    }

    if(!_parity.compare("None", Qt::CaseInsensitive))
    {
        parity = QSerialPort::NoParity;
    }
    else if(!_parity.compare("Even", Qt::CaseInsensitive))
    {
        parity = QSerialPort::EvenParity;
    }
    else if(!_parity.compare("Odd", Qt::CaseInsensitive))
    {
        parity = QSerialPort::OddParity;
    }
    else
    {
        parity = QSerialPort::UnknownParity;
    }

    setPortName(pref->portName());
    if(!setBaudRate(pref->baudRate())) {
        return false;
    }
    if(!setDataBits(dataBits)) {
        return false;
    }
    if(!setStopBits(stopBits)) {
        return false;
    }
    if(!setParity(parity)) {
        return false;
    }
    if(!setFlowControl(QSerialPort::NoFlowControl)) {
        return false;
    }

    return true;
}

bool SerialPortHelper::openPort()
{
    return serialPort->open(QSerialPort::ReadWrite);
}

void SerialPortHelper::closePort()
{
    if(serialPort->isOpen())
    {
        serialPort->close();
    }
}

bool SerialPortHelper::setPort(QSerialPort *port)
{
    if(port)
    {
        setPortName(port->portName());
        if(!setBaudRate(port->baudRate()))
        {
            qDebug() << __func__ << "set baud rate failed";
            return false;
        }
        if(!setDataBits(port->dataBits()))
        {
            qDebug() << __func__ << "set data bits failed";
            return false;
        }
        if(!setParity(port->parity()))
        {
            qDebug() << __func__ << "set parity failed";
            return false;
        }
        if(!setStopBits(port->stopBits()))
        {
            qDebug() << __func__ << "set stop Bits failed";
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

QString SerialPortHelper::portName()
{
    return serialPort->portName();
}

void SerialPortHelper::setPortName(const QString &portName)
{
    serialPort->setPortName(portName);
}

bool SerialPortHelper::setBaudRate(qint32 baudRate)
{
    return serialPort->setBaudRate(baudRate);
}

bool SerialPortHelper::setDataBits(QSerialPort::DataBits dataBits)
{
    return serialPort->setDataBits(dataBits);
}

bool SerialPortHelper::setParity(QSerialPort::Parity parity)
{
    return serialPort->setParity(parity);
}

bool SerialPortHelper::setStopBits(QSerialPort::StopBits stopBits)
{
    return serialPort->setStopBits(stopBits);
}

bool SerialPortHelper::setFlowControl(QSerialPort::FlowControl flowControl)
{
    return serialPort->setFlowControl(flowControl);
}

QString SerialPortHelper::genPortSum() const
{
    QString parity = 0;
    QString sum = 0;

    switch (serialPort->parity()) {
    case QSerialPort::NoParity:
        parity = "None";
        break;
    case QSerialPort::EvenParity:
        parity = "Even";
        break;
    case QSerialPort::OddParity:
        parity = "Odd";
        break;
    default:
        parity = "Unknown";
        break;
    }

    return sum.sprintf("%s OPEND, %d, %d, %s, %d", qPrintable(serialPort->portName()), serialPort->baudRate(),
                       serialPort->dataBits(), qPrintable(parity.toUpper()), serialPort->stopBits());
}
