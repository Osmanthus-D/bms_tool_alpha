#ifndef COSTUMTRANTRANSMIT_H
#define COSTUMTRANTRANSMIT_H

#include <QFile>
#include <QTimer>
#include <QObject>
#include <QSerialPort>
#include "CostumTran.h"

class CostumTranTransmit : public QObject, public CostumTran
{
    Q_OBJECT

public:
    explicit CostumTranTransmit(QObject *parent = 0);
    ~CostumTranTransmit();

    void setFileName(const QString &name);
    void setPort(QSerialPort * ss);
   // void setPortName(const QString &name);
   // void setPortBaudRate(qint32 baudrate);

    bool startTransmit();
    void stopTransmit();

    int getTransmitProgress();
    Status getTransmitStatus();

signals:
    void transmitProgress(int progress);
    void transmitStatus(CostumTranTransmit::Status status);
    void sendReceived(QString msg);
    void sendSent(QString msg);

private slots:
    void readTimeOut();
    void writeTimeOut();

private:
    Code callback(Status status, uint8_t *buff, uint32_t *len);

    uint32_t read(uint8_t *buff, uint32_t len);
    uint32_t write(uint8_t *buff, uint32_t len);

    QFile       *file;
    QTimer      *readTimer;
    QTimer      *writeTimer;
    QSerialPort *serialPort;

    int      progress;
    Status   status;
    uint64_t fileSize;
    uint64_t fileCount;
};

#endif // YMODEMFILETRANSMIT_H
