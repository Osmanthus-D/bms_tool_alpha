#include "CostumTranTransmit.h"
#include <QFileInfo>

#define READ_TIME_OUT   (10)
#define WRITE_TIME_OUT  (100)

CostumTranTransmit::CostumTranTransmit(QObject *parent) :
    QObject(parent),
    file(new QFile),
    readTimer(new QTimer),
    writeTimer(new QTimer),
    serialPort(new QSerialPort)
{
    setTimeDivide(499);
    setTimeMax(5);
    setErrorMax(999);

    serialPort->setPortName("COM1");
    serialPort->setBaudRate(115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    connect(readTimer, SIGNAL(timeout()), this, SLOT(readTimeOut()));
    connect(writeTimer, SIGNAL(timeout()), this, SLOT(writeTimeOut()));
}

CostumTranTransmit::~CostumTranTransmit()
{
    delete file;
    delete readTimer;
    delete writeTimer;
    delete serialPort;
}

void CostumTranTransmit::setFileName(const QString &name)
{
    file->setFileName(name);
}

void CostumTranTransmit::setPort(QSerialPort * ss)
{
    this->serialPort = ss;
}

//void YmodemFileTransmit::setPortName(const QString &name)
//{
//    serialPort->setPortName(name);
//}

//void YmodemFileTransmit::setPortBaudRate(qint32 baudrate)
//{
//    serialPort->setBaudRate(baudrate);
//}

bool CostumTranTransmit::startTransmit()
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

void CostumTranTransmit::stopTransmit()
{
    file->close();
    abort();
    status = StatusAbort;
    writeTimer->start(WRITE_TIME_OUT);
}

int CostumTranTransmit::getTransmitProgress()
{
    return progress;
}

CostumTran::Status CostumTranTransmit::getTransmitStatus()
{
    return status;
}

void CostumTranTransmit::readTimeOut()
{
    readTimer->stop();

    transmit();

    if((status == StatusEstablish) || (status == StatusTransmit))
    {
        readTimer->start(READ_TIME_OUT);
    }
}

void CostumTranTransmit::writeTimeOut()
{
    writeTimer->stop();
    serialPort->close();
    transmitStatus(status);
}

CostumTran::Code CostumTranTransmit::callback(Status status, uint8_t *buff, uint32_t *len)
{
    switch(status)
    {
        case StatusEstablish:
        {
            if(file->open(QFile::ReadOnly) == true)
            {
                QFileInfo fileInfo(*file);

                fileSize  = fileInfo.size();
                fileCount = 0;

                strcpy((char *)buff, fileInfo.fileName().toLocal8Bit().data());
                strcpy((char *)buff + fileInfo.fileName().toLocal8Bit().size() + 1, QByteArray::number(fileInfo.size()).data());

                *len = YMODEM_PACKET_SIZE;

                CostumTranTransmit::status = StatusEstablish;

                transmitStatus(StatusEstablish);

                return CodeAck;
            }
            else
            {
                CostumTranTransmit::status = StatusError;

                writeTimer->start(WRITE_TIME_OUT);

                return CodeCan;
            }
        }

        case StatusTransmit:
        {
            if(fileSize != fileCount)
            {
                if((fileSize - fileCount) > YMODEM_PACKET_SIZE)
                {
                    fileCount += file->read((char *)buff, YMODEM_PACKET_1K_SIZE);

                    *len = YMODEM_PACKET_1K_SIZE;
                }
                else
                {
                    fileCount += file->read((char *)buff, YMODEM_PACKET_SIZE);

                    *len = YMODEM_PACKET_SIZE;
                }

                progress = (int)(fileCount * 100 / fileSize);

                CostumTranTransmit::status = StatusTransmit;

                transmitProgress(progress);
                transmitStatus(StatusTransmit);

                return CodeAck;
            }
            else
            {
                CostumTranTransmit::status = StatusTransmit;

                transmitStatus(StatusTransmit);

                return CodeEot;
            }
        }

        case StatusFinish:
        {
            file->close();

            CostumTranTransmit::status = StatusFinish;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeAck;
        }

        case StatusAbort:
        {
            file->close();

            CostumTranTransmit::status = StatusAbort;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeCan;
        }

        case StatusTimeout:
        {
            CostumTranTransmit::status = StatusTimeout;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeCan;
        }

        default:
        {
            file->close();

            CostumTranTransmit::status = StatusError;

            writeTimer->start(WRITE_TIME_OUT);

            return CodeCan;
        }
    }
}

uint32_t CostumTranTransmit::read(uint8_t *buff, uint32_t len)
{
    return serialPort->read((char *)buff, len);
}

uint32_t CostumTranTransmit::write(uint8_t *buff, uint32_t len)
{
    return serialPort->write((char *)buff, len);
}
