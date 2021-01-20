#include "YmodemFileReceive.h"

#define READ_TIME_OUT   (10)
#define WRITE_TIME_OUT  (100)

YmodemFileReceive::YmodemFileReceive(QObject *parent) :
    QObject(parent),
    file(new QFile),
    readTimer(new QTimer),
    writeTimer(new QTimer)
{
    setTimeDivide(499);
    setTimeMax(5);
    setErrorMax(999);

    // restore serial port properties set last time
    if(!initPort())
    {
        qDebug("%s serial port init failed.", __func__);
    }

    connect(readTimer, SIGNAL(timeout()), this, SLOT(readTimeOut()));
    connect(writeTimer, SIGNAL(timeout()), this, SLOT(writeTimeOut()));
}

YmodemFileReceive::~YmodemFileReceive()
{
    delete file;
    delete readTimer;
    delete writeTimer;
    if(serialPort->isOpen()) {
        serialPort->close();
        qDebug("%s closed", qPrintable(portName()));
    }
}

void YmodemFileReceive::setFilePath(const QString &path)
{
    filePath = path + "/";
}

void YmodemFileReceive::updateSerialPort(QSerialPort *port)
{
    if(!setPort(port))
    {
        qDebug("%s set serial port failed.", __func__);
    }
}

bool YmodemFileReceive::startReceive()
{
    progress = 0;
    status   = StatusEstablish;

    if(serialPort->open(QSerialPort::ReadWrite) == true)
    {
        readTimer->start(READ_TIME_OUT);

        return true;
    }
    else
    {
        return false;
    }
}

void YmodemFileReceive::stopReceive()
{
    file->close();
    abort();
    status = StatusAbort;
    writeTimer->start(WRITE_TIME_OUT);
}

int YmodemFileReceive::getReceiveProgress()
{
    return progress;
}

Ymodem::Status YmodemFileReceive::getReceiveStatus()
{
    return status;
}

void YmodemFileReceive::readTimeOut()
{
    readTimer->stop();

    receive();

    if((status == StatusEstablish) || (status == StatusTransmit))
    {
        readTimer->start(READ_TIME_OUT);
    }
}

void YmodemFileReceive::writeTimeOut()
{
    writeTimer->stop();
    serialPort->close();
    qDebug("%s closed", qPrintable(portName()));
    receiveStatus(status);
}

Ymodem::Code YmodemFileReceive::callback(Status status, uint8_t *buff, uint32_t *len)
{
    switch(status)
    {
        case StatusEstablish:
        {
            if(buff[0] != 0)
            {
                int  i         =  0;
                char name[128] = {0};
                char size[128] = {0};

                for(int j = 0; buff[i] != 0; i++, j++)
                {
                    name[j] = buff[i];
                }

                i++;

                for(int j = 0; buff[i] != 0; i++, j++)
                {
                    size[j] = buff[i];
                }

                fileName  = QString::fromLocal8Bit(name);
                QString file_desc(size);
                QString sizeStr = file_desc.left(file_desc.indexOf(' '));
                fileSize  = sizeStr.toULongLong();
                fileCount = 0;

                file->setFileName(filePath + fileName);

                if(file->open(QFile::WriteOnly) == true)
                {
                    YmodemFileReceive::status = StatusEstablish;

                    receiveStatus(StatusEstablish);

                    return CodeAck;
                }
                else
                {
                    YmodemFileReceive::status = StatusError;

                    writeTimer->start(WRITE_TIME_OUT);

                    return CodeCan;
                }
            }
            else
            {
                YmodemFileReceive::status = StatusError;

                writeTimer->start(WRITE_TIME_OUT);

                return CodeCan;
            }
        }

        case StatusTransmit:
        {
            if((fileSize - fileCount) > *len)
            {
                file->write((char *)buff, *len);

                fileCount += *len;
            }
            else
            {
                file->write((char *)buff, fileSize - fileCount);

                fileCount += fileSize - fileCount;
            }

            progress = (int)(fileCount * 100 / fileSize);

            YmodemFileReceive::status = StatusTransmit;

            receiveProgress(progress);
            receiveStatus(StatusTransmit);

            return CodeAck;
        }

        case StatusFinish:
        {
            file->close();

            YmodemFileReceive::status = StatusFinish;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeAck;
        }

        case StatusAbort:
        {
            file->close();

            YmodemFileReceive::status = StatusAbort;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeCan;
        }

        case StatusTimeout:
        {
            YmodemFileReceive::status = StatusTimeout;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeCan;
        }

        default:
        {
            file->close();

            YmodemFileReceive::status = StatusError;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeCan;
        }
    }
}

//uint32_t YmodemFileReceive::read(uint8_t *buff, uint32_t len)
//{
//    return serialPort->read((char *)buff, len);
//}

//uint32_t YmodemFileReceive::write(uint8_t *buff, uint32_t len)
//{
//    return serialPort->write((char *)buff, len);
//}

uint32_t YmodemFileReceive::read(uint8_t *buff, uint32_t len)
{
    QByteArray arr;
    int real_len = serialPort->read((char *)buff, len);
    if(real_len != 0)
    {
//        qDebug() << __FUNCTION__ << ":";
    }
    for(int i = 0; i < real_len; i++)
    {
        arr.append(buff[i]);
//        qDebug() << QString() .sprintf("%02X", buff[i]);
    }
    if(real_len != 0)
    {
        sendReceived(arr.toHex());
    }
    return real_len;
}

uint32_t YmodemFileReceive::write(uint8_t *buff, uint32_t len)
{
    QByteArray arr;
    int real_len = serialPort->write((char *)buff, len);
    for(int i = 0; i < real_len; i++)
    {
        arr.append(buff[i]);
    }
    if(real_len != 0)
    {
        sendSent(arr.toHex());
    }
    return real_len;
}
