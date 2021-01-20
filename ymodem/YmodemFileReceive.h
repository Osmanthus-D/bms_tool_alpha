#ifndef YMODEMFILERECEIVE_H
#define YMODEMFILERECEIVE_H

#include <QObject>
#include <QFile>
#include <QTimer>
#include <QSerialPort>
#include "serialporthelper.h"
#include "Ymodem.h"

class YmodemFileReceive : public QObject, public Ymodem, public SerialPortHelper
{
    Q_OBJECT

public:
    explicit YmodemFileReceive(QObject *parent = 0);
    ~YmodemFileReceive();

    void setFilePath(const QString &path);

    bool startReceive();
    void stopReceive();

    int getReceiveProgress();
    Status getReceiveStatus();

public slots:
    void updateSerialPort(QSerialPort *port);

signals:
    void receiveProgress(int progress);
    void receiveStatus(YmodemFileReceive::Status status);
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
