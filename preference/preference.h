#ifndef PREFERENCE_H
#define PREFERENCE_H

#include <QSettings>

class Preference : public QSettings
{
    Q_OBJECT

public:
    explicit Preference(QObject *parent = 0);

    QString portName();
    void setPortName(QString value);

    int baudRate();
    void setBaudRate(int value);

    int dataBits();
    void setDataBits(int value);

    QString parity();
    void setParity(QString value);

    int stopBits();
    void setStopBits(int value);

    int coreNum();
    void setCoreNum(int value);

    double tempLowerLimit();
    void setTempLowerLimit(double value);

    double tempUpperLimit();
    void setTempUpperLimit(double value);

    double voltLowerLimit();
    void setVoltLowerLimit(double value);

    double voltUpperLimit();
    void setVoltUpperLimit(double value);
};

#endif // PREFERENCE_H
