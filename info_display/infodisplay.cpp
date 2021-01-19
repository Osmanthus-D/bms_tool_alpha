#include "infodisplay.h"
#include "ui_infodisplay.h"

InfoDispaly::InfoDispaly(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoDisplay)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    thread = new QThread;

    modbusHelper = new ModbusHelper(0);
    modbusHelper->moveToThread(thread);
    pref = new Preference(this);

    initDeviceDesc();
    
    pixmapYesOrig = QPixmap::fromImage(imageYes);
    pixmapWarnOrig = QPixmap::fromImage(imageWarn);

    connect(this, SIGNAL(rtuReadSig()), modbusHelper, SLOT(rtuRead()));
    connect(modbusHelper,SIGNAL(refreshCorePortectSig(uint8_t *)),this,SLOT(refreshCoreProtect(uint8_t *)));

    thread->start();
}

InfoDispaly::~InfoDispaly()
{
    stop();

    delete modbusHelper;
    delete desc;
    delete ui;
}

void InfoDispaly::initDeviceDesc()
{
    desc = new device_desc_t;
    memset(desc, 0x00, sizeof(device_desc_t));
}

void InfoDispaly::setCoreNumDisp(int n)
{
    setGroupBoxCoreNum(ui->groupBoxOcProtect, n, OC_PROTECT_PREFIX_LEN);
    setGroupBoxCoreNum(ui->groupBoxOdProtect, n, OD_PROTECT_PREFIX_LEN);

    if(n > VOLTAGE_STANDARD_LINE_NUM * 2)
    {
        ui->groupBoxGrp_2->setVisible(true);
        ui->coreVolGrp1->setLineNumber(VOLTAGE_STANDARD_LINE_NUM);
        ui->coreVolGrp2->setLineNumber(n / 2 - VOLTAGE_STANDARD_LINE_NUM);
    }
    else
    {
        ui->groupBoxGrp_2->setVisible(false);
        ui->coreVolGrp1->setLineNumber(n / 2);
        ui->coreVolGrp2->setLineNumber(0);
    }
}

void InfoDispaly::setGroupBoxCoreNum(QGroupBox *gb, int n, int prefixLen)
{
    bool ok = false;
    int m = 0;
    QList<QLabel *> list = gb->findChildren<QLabel *>();

    foreach(QLabel *l, list)
    {
        m = l->text().remove(0, 5).toInt(&ok);

        if(!ok)
        {
            m = l->objectName().remove(0, prefixLen).toInt(&ok);

            if(ok && m > n)
            {
                l->setVisible(false);
            }
            else
            {
                l->setVisible(true);
            }
            continue;
        }
        if(m > n)
        {
            l->setVisible(false);
        }
        else
        {
            l->setVisible(true);
        }
    }
}

void InfoDispaly::setGroupBoxStatus(QGroupBox *box, QString prefix, quint8 *buf)
{
    bool ok = false;
    QLabel *label;
    QString labelName;
    QObjectList children = box->children();

    setPixmapScaled();

    foreach(QObject *child, children)
    {
        QString className = QString(child->metaObject()->className());
        if(!className.compare("QLabel"))
        {
            label = (QLabel *)child;
            labelName = label->objectName();

            if (labelName.contains(prefix))
            {
                int n = labelName.remove(prefix).toInt(&ok);

                if(ok)
                {
                    if (0 == buf[n])
                    {
                        label->setPixmap(pixmapYes);
                    }
                    else
                    {
                        label->setPixmap(pixmapWarn);
                    }
                }
            }
            else
            {
                continue;
            }
        }
    }
}

void InfoDispaly::setVoltageRange(double min, double max)
{
    ui->coreVolGrp1->setRange((int)(min * 1000), (int)(max * 1000));
    ui->coreVolGrp2->setRange((int)(min * 1000), (int)(max * 1000));
}

void InfoDispaly::setTempRange(double minValue, double maxValue)
{
    ui->ringCoreTemp_1->setRange(minValue, maxValue);
    ui->ringCoreTemp_2->setRange(minValue, maxValue);
    ui->ringPCBTemp->setRange(minValue, maxValue);
    ui->ringAmbientTemp->setRange(minValue, maxValue);
}

void InfoDispaly::setPixmapScaled()
{
    QLabel *l = ui->label_charge01;

    pixmapYes = pixmapYesOrig.scaled(l->width(), l->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmapWarn = pixmapWarnOrig.scaled(l->width(), l->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void InfoDispaly::refreshCoreProtect(uint8_t * a)
{
    battery_desc_t desc;
    quint16 id[MODBUS_DEVICE_ID_LEN + 1] = {0};
    quint16 *q = (quint16 *)&desc + 10;

    memcpy(&desc, a + MODBUS_COILS_READ_LEN, sizeof(desc));
    memcpy(id, (quint16*)&desc + MODBUS_REGISTERS_READ_LEN, MODBUS_DEVICE_ID_LEN);
    setGroupBoxStatus(ui->groupBoxOcProtect, "label_charge", a + WARNING_NUM);
    setGroupBoxStatus(ui->groupBoxOdProtect, "label_discharge", a + WARNING_NUM + CORE_VOLTAGE_NUM);
    setGroupBoxStatus(ui->groupBoxWarn, "labelWarning", a);


    for(int i = 0; i < MODBUS_DEVICE_ID_LEN; i++)
    {
        uint16_t tmp = qFromBigEndian(id[i]);
        memcpy(id + i, &tmp, sizeof(tmp));
    }

    ui->frame->setValue((float)desc.soc / 10.0);
    ui->labelSoc->setText(QString("%1%").arg(desc.soc / 10.0));

    if(id[0] != 0x00)
    {
        ui->groupBoxDevice->setTitle(tr("Device") + QString("(%1)").arg((char *)id));
    }

    ui->labelVoltage->setText(QString("%1V").arg(QString::number((float)desc.outputVoltage / 10, 'f', 1)));

    ui->labelCurrent->setText(QString("%1A").arg(QString::number((float)desc.chargingCurrent/10, 'f', 1)));

    ui->ringCoreTemp_1->setValue(desc.coreTemp1);

    ui->ringCoreTemp_2->setValue(desc.coreTemp2);

    ui->ringPCBTemp->setValue(desc.boardTemp);

    ui->ringAmbientTemp->setValue(desc.ambientTemp);

    ui->coreVolGrp1->setValue1(*q++);
    ui->coreVolGrp1->setValue2(*q++);
    ui->coreVolGrp1->setValue3(*q++);
    ui->coreVolGrp1->setValue4(*q++);
    ui->coreVolGrp1->setValue5(*q++);
    ui->coreVolGrp1->setValue6(*q++);
    ui->coreVolGrp1->setValue7(*q++);
    ui->coreVolGrp1->setValue8(*q++);
    ui->coreVolGrp1->setValue9(*q++);
    ui->coreVolGrp1->setValue10(*q++);

    ui->coreVolGrp2->setValue1(*q++);
    ui->coreVolGrp2->setValue2(*q++);
    ui->coreVolGrp2->setValue3(*q++);
    ui->coreVolGrp2->setValue4(*q++);
    ui->coreVolGrp2->setValue5(*q++);
    ui->coreVolGrp2->setValue6(*q++);
    ui->coreVolGrp2->setValue7(*q++);
    ui->coreVolGrp2->setValue8(*q++);
    ui->coreVolGrp2->setValue9(*q++);
    ui->coreVolGrp2->setValue10(*q++);
}

int InfoDispaly::createModbus(const char *device, int baud, char parity, int data_bit, int stop_bit)
{
    ModbusHelper::mb = modbus_new_rtu(device, baud, parity, data_bit, stop_bit);   //same port can only open once
    //modbus_new_rtu("COM2", 9600, 'N', 8, 1);
    if(NULL == ModbusHelper::mb)
    {
        return -1;
    }

    modbus_set_slave(ModbusHelper::mb, 224);
    if(-1 == modbus_connect(ModbusHelper::mb))
    {
        qDebug() << QString("modbus_connect failed with errno(%1)").arg(modbus_strerror(errno));
        modbus_free(ModbusHelper::mb);
        ModbusHelper::mb = NULL;
        return -2;
    }

    struct timeval t;
    t.tv_sec=0;
    t.tv_usec=1000000;
    modbus_set_response_timeout(ModbusHelper::mb, t.tv_sec, t.tv_usec);
    modbus_set_debug(ModbusHelper::mb, 1);

    return 0;
}

void InfoDispaly::destroyModbus()
{
    if(ModbusHelper::mb)
    {
        modbus_close(ModbusHelper::mb);
        modbus_free(ModbusHelper::mb);
        ModbusHelper::mb = NULL;
    }
}

void InfoDispaly::displayTest()
{
    destroyModbus();
    if(0 != createModbus("COM2", 9600, 'N', 8, 1))
    {
        QMessageBox::information(this, QString("SerialPort(%1:%2-N-8-1").arg("COM2").arg(9600), "Open failed");
        return;
    }
    timer->start(1500);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(rtuReadSig()));
}

void InfoDispaly::play()
{
    const char *device;
    int baud        = 0;
    char parity     = 0;
    int data_bit    = 0;
    int stop_bit    = 0;
    QString _parity = pref->parity().toUpper();
    QByteArray byteArray = pref->portName().toAscii();

    device      = byteArray.data();
    baud        = pref->baudRate();
    parity      = _parity.at(0).toAscii();
    data_bit    = pref->dataBits();
    stop_bit    = pref->stopBits();

    if(strlen(device))
    {
        if(!ModbusHelper::mb)
        {
            if(0 != createModbus(device, baud, parity, data_bit, stop_bit))
            {
                qDebug("%s(%d-%c-%d-%d) open failed", device, baud, parity, data_bit, stop_bit);
                emit reportStatus(tr("%1 CLOSED").arg(device), false);
            }
            else
            {
                // save current config
                memcpy(&desc->device, device, strlen(device));
                memcpy(&desc->baud, &baud, sizeof(baud));
                memcpy(&desc->parity, &parity, sizeof(parity));
                memcpy(&desc->data_bit, &data_bit, sizeof(data_bit));
                memcpy(&desc->stop_bit, &stop_bit, sizeof(stop_bit));

                qDebug("%s(%d-%c-%d-%d) opened", device, baud, parity, data_bit, stop_bit);
                emit reportStatus(QString("%1 OPEND, %2, %3, %4, %5").arg(device).arg(baud).arg(data_bit).arg(_parity).arg(stop_bit), true);
                timer->start(1500);
                connect(timer, SIGNAL(timeout()), this, SIGNAL(rtuReadSig()));
            }
        }
    }
    else
    {
        QMessageBox::information(this, tr("Info"), tr("No Configuration Has Found."));
    }
}

void InfoDispaly::stop()
{    
    timer->stop();
    disconnect(timer, SIGNAL(timeout()), this, SIGNAL(rtuReadSig()));
    if(ModbusHelper::mb)
    {
        qDebug() << desc->device << "closed";
        emit reportStatus(QString("%1 CLOSED").arg(desc->device), false);
    }
    else
    {
        return;
    }

    destroyModbus();
}
