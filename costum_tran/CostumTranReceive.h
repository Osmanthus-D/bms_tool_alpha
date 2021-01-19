#ifndef COSTUMTRANRECEIVE_H
#define COSTUMTRANRECEIVE_H

#include <QFile>
#include <QTimer>
#include <QObject>
#include <QSerialPort>
#include "CostumTran.h"
#include "preference.h"

class CostumTranReceive : public QObject, public CostumTran
{
    Q_OBJECT

public:
    explicit CostumTranReceive(QObject *parent = 0);
    ~CostumTranReceive();

    QString genPortSummary() const;
    void setFilePath(const QString &path);

    void setPortName(const QString &name);
    void setPortBaudRate(qint32 baudrate);
    bool openPort();
    void closePort();

    bool startReceive();
    void stopReceive();

    int getReceiveProgress();
    Status getReceiveStatus();
    void setPort(QSerialPort *port);

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
    bool initSerialPort();

    uint32_t read(uint8_t *buff, uint32_t len);
    uint32_t write(uint8_t *buff, uint32_t len);

    QFile       *file;
    QTimer      *readTimer;
    QTimer      *writeTimer;
    QSerialPort *serialPort;
    Preference  *pref;

    int      progress;
    Status   status;
    QString  filePath;
    QString  fileName;
    uint64_t fileSize;
    uint64_t fileCount;
};

#endif // YMODEMFILERECEIVE_H
