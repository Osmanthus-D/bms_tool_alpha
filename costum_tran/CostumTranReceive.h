#ifndef COSTUMTRANRECEIVE_H
#define COSTUMTRANRECEIVE_H

#include <QFile>
#include <QTimer>
#include <QObject>
#include "CostumTran.h"
#include "serialporthelper.h"

class CostumTranReceive : public QObject, public CostumTran, public SerialPortHelper
{
    Q_OBJECT

public:
    explicit CostumTranReceive(QObject *parent = 0);
    ~CostumTranReceive();

    void setFilePath(const QString &path);

    bool startReceive();
    void stopReceive();

    int getReceiveProgress();
    Status getReceiveStatus();

public slots:
    void updateSerialPort(QSerialPort *port);

signals:
    void receiveProgress(int progress);
    void receiveStatus(CostumTranReceive::Status status);
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

    int      progress;
    Status   status;
    QString  filePath;
    QString  fileName;
    uint64_t fileSize;
    uint64_t fileCount;
};

#endif // YMODEMFILERECEIVE_H
