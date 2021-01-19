#include "preference.h"

Preference::Preference(QObject *parent) :
    QSettings("tool.ini", QSettings::IniFormat, parent)
{

}

QString Preference::portName()
{
    return value("/Port/PortName", QString("")).toString();
}

void Preference::setPortName(QString value)
{
    setValue("/Port/PortName", value);
}

int Preference::baudRate()
{
    return value("/Port/BaudRate", 9600).toInt();
}

void Preference::setBaudRate(int value)
{
    setValue("/Port/BaudRate", value);
}

int Preference::dataBits()
{
    return value("/Port/DataBits", 8).toInt();
}

void Preference::setDataBits(int value)
{
    setValue("/Port/DataBits", value);
}

QString Preference::parity()
{
    return value("/Port/Parity", "None").toString();
}

void Preference::setParity(QString value)
{
    setValue("/Port/Parity", value);
}

int Preference::stopBits()
{
    return value("/Port/StopBits", 1).toInt();
}

void Preference::setStopBits(int value)
{
    setValue("/Port/StopBits", value);
}

int Preference::coreNum()
{
    return value("/Display/CoreNum", 20).toInt();
}

void Preference::setCoreNum(int value)
{
    setValue("/Display/CoreNum", value);
}

double Preference::tempLowerLimit()
{
    return value("/Display/TempLowerLimit", -40).toDouble();
}

void Preference::setTempLowerLimit(double value)
{
    setValue("/Display/TempLowerLimit", value);
}

double Preference::tempUpperLimit()
{
    return value("/Display/TempUpperLimit", 100).toDouble();
}

void Preference::setTempUpperLimit(double value)
{
    setValue("/Display/TempUpperLimit", value);
}

double Preference::voltLowerLimit()
{
    return value("/Display/VoltLowerLimit", 0).toDouble();
}

void Preference::setVoltLowerLimit(double value)
{
    setValue("/Display/VoltLowerLimit", value);
}

double Preference::voltUpperLimit()
{
    return value("/Display/VoltUpperLimit", 5).toDouble();
}

void Preference::setVoltUpperLimit(double value)
{
    setValue("/Display/VoltUpperLimit", value);
}
