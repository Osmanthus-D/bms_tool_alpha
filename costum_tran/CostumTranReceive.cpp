#include "CostumTranReceive.h"

#define READ_TIME_OUT   (10)
#define WRITE_TIME_OUT  (100)

CostumTranReceive::CostumTranReceive(QObject *parent) :
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

CostumTranReceive::~CostumTranReceive()
{
    delete file;
    delete readTimer;
    delete writeTimer;
    if(serialPort->isOpen()) {
        serialPort->close();
        qDebug("%s closed", qPrintable(portName()));
    }
}

void CostumTranReceive::setFilePath(const QString &path)
{
    filePath = path + "/";
}

void CostumTranReceive::updateSerialPort(QSerialPort *port)
{
    if(!setPort(port))
    {
        qDebug("%s set serial port failed.", __func__);
    }
}

bool CostumTranReceive::startReceive()
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

void CostumTranReceive::stopReceive()
{
    file->close();
    abort();
    status = StatusAbort;
    writeTimer->start(WRITE_TIME_OUT);
}

int CostumTranReceive::getReceiveProgress()
{
    return progress;
}

CostumTran::Status CostumTranReceive::getReceiveStatus()
{
    return status;
}

void CostumTranReceive::readTimeOut()
{
    readTimer->stop();

    receive();

    if((status == StatusEstablish) || (status == StatusTransmit))
    {
        readTimer->start(READ_TIME_OUT);
    }
}

void CostumTranReceive::writeTimeOut()
{
    writeTimer->stop();
    serialPort->close();
    qDebug("%s closed", qPrintable(portName()));
    receiveStatus(status);
}

CostumTran::Code CostumTranReceive::callback(Status status, uint8_t *buff, uint32_t *len)
{
    Q_UNUSED(len)

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
                    CostumTranReceive::status = StatusEstablish;

                    receiveStatus(StatusEstablish);

                    return CodeAck;
                }
                else
                {
                    CostumTranReceive::status = StatusError;

                    writeTimer->start(WRITE_TIME_OUT);

                    return CodeCan;
                }
            }
            else
            {
                CostumTranReceive::status = StatusError;

                writeTimer->start(WRITE_TIME_OUT);

                return CodeCan;
            }
        }

        case StatusTransmit:
        {
//            if((fileSize - fileCount) > *len)
//            {
//                file->write((char *)buff, *len);

//                fileCount += *len;
//            }
//            else
//            {
//                file->write((char *)buff, fileSize - fileCount);

//                fileCount += fileSize - fileCount;
//            }

//            progress = (int)(fileCount * 100 / fileSize);

            CostumTranReceive::status = StatusTransmit;

//            receiveProgress(progress);
            receiveStatus(StatusTransmit);

            return CodeAck;
        }

        case StatusFinish:
        {
            file->close();

            CostumTranReceive::status = StatusFinish;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeAck;
        }

        case StatusAbort:
        {
            file->close();

            CostumTranReceive::status = StatusAbort;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeCan;
        }

        case StatusTimeout:
        {
            CostumTranReceive::status = StatusTimeout;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeCan;
        }

        default:
        {
            file->close();

            CostumTranReceive::status = StatusError;

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

uint32_t CostumTranReceive::read(uint8_t *buff, uint32_t len)
{
    QByteArray arr;
    int real_len = 0;
    real_len = serialPort->read((char *)buff, len);
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
        sendReceived(QString(arr));
    }
    return real_len;
}

uint32_t CostumTranReceive::write(uint8_t *buff, uint32_t len)
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
