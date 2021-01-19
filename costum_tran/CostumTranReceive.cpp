#include "CostumTranReceive.h"

#define READ_TIME_OUT   (10)
#define WRITE_TIME_OUT  (100)

CostumTranReceive::CostumTranReceive(QObject *parent) :
    QObject(parent),
    file(new QFile),
    readTimer(new QTimer),
    writeTimer(new QTimer),
    serialPort(new QSerialPort)
{
    setTimeDivide(499);
    setTimeMax(5);
    setErrorMax(999);

    pref = new Preference(this);
    if(!initSerialPort())
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
        qDebug("%s closed", qPrintable(serialPort->portName()));
    }
    delete serialPort;
}

void CostumTranReceive::setFilePath(const QString &path)
{
    filePath = path + "/";
}

QString CostumTranReceive::genPortSummary() const
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

void CostumTranReceive::setPort(QSerialPort *port)
{
    if(port)
    {
        serialPort->setPortName(port->portName());
        if(!serialPort->setBaudRate(port->baudRate()))
        {
            qDebug() << __func__ << "set baud rate failed";
            return;
        }
        if(!serialPort->setDataBits(port->dataBits()))
        {
            qDebug() << __func__ << "set data bits failed";
            return;
        }
        if(!serialPort->setParity(port->parity()))
        {
            qDebug() << __func__ << "set parity failed";
            return;
        }
        if(!serialPort->setStopBits(port->stopBits()))
        {
            qDebug() << __func__ << "set stop Bits failed";
            return;
        }
    }
}

void CostumTranReceive::setPortName(const QString &name)
{
    serialPort->setPortName(name);
}

void CostumTranReceive::setPortBaudRate(qint32 baudrate)
{
    serialPort->setBaudRate(baudrate);
}

bool CostumTranReceive::openPort()
{
    return serialPort->open(QSerialPort::ReadWrite);
}

void CostumTranReceive::closePort()
{
    if(serialPort->isOpen())
    {
        serialPort->close();
    }
}

bool CostumTranReceive::initSerialPort()
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

    if(!_parity.compare("None", Qt::CaseInsensitive)) {
        parity = QSerialPort::NoParity;
    } else if(!_parity.compare("Even", Qt::CaseInsensitive)) {
        parity = QSerialPort::EvenParity;
    } else if(!_parity.compare("Odd", Qt::CaseInsensitive)) {
        parity = QSerialPort::OddParity;
    } else {
        parity = QSerialPort::UnknownParity;
    }

    serialPort->setPortName(pref->portName());
    if(!serialPort->setBaudRate(pref->baudRate())) {
        return false;
    }
    if(!serialPort->setDataBits(dataBits)) {
        return false;
    }
    if(!serialPort->setStopBits(stopBits)) {
        return false;
    }
    if(!serialPort->setParity(parity)) {
        return false;
    }
    if(!serialPort->setFlowControl(QSerialPort::NoFlowControl)) {
        return false;
    }

    return true;
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
    qDebug("%s closed", qPrintable(serialPort->portName()));
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
