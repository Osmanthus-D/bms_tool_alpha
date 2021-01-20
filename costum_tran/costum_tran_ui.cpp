#include "costum_tran_ui.h"
#include "ui_ui_costum_tran.h"
#include <QFileDialog>
#include <QtCore>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

ui_costum_tran::ui_costum_tran(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ui_costum_tran),
    costumTranTransmit(new CostumTranTransmit),
    costumTranReceive(new CostumTranReceive)
{
    transmitButtonStatus = false;
    receiveButtonStatus  = false;

    ui->setupUi(this);
//    Find_SerialPort();
    timer = new QTimer();
    ui->groupBox->setVisible(false);

    connect(costumTranTransmit, SIGNAL(transmitProgress(int)), this, SLOT(transmitProgress(int)));
    connect(costumTranReceive, SIGNAL(receiveProgress(int)), this, SLOT(receiveProgress(int)));
    connect(costumTranTransmit, SIGNAL(transmitStatus(CostumTranTransmit::Status)), this, SLOT(transmitStatus(CostumTranTransmit::Status)));
    connect(costumTranReceive, SIGNAL(receiveStatus(CostumTranReceive::Status)), this, SLOT(receiveStatus(CostumTranReceive::Status)));

    connect(costumTranReceive, SIGNAL(sendReceived(QString)), this, SLOT(show_received(QString)));
    connect(costumTranReceive, SIGNAL(sendSent(QString)), this, SLOT(show_sent(QString)));
    connect(this, SIGNAL(serialPortUpdated(QSerialPort *)), costumTranReceive, SLOT(updateSerialPort(QSerialPort *)));

    connect(timer,SIGNAL(timeout()),this,SLOT(ss_timer_irq()));

    QDateTime dateTime = QDateTime::fromString(QString("2020- 1-14/ 6: 4:10").replace(' ', '0'), "yyyy-MM-dd/hh:mm:ss");
    qDebug() << dateTime.toString("yyyy-MM-dd/hh:mm:ss") << QString(10, 'x');
}

ui_costum_tran::~ui_costum_tran()
{
    delete ui;
    delete costumTranTransmit;
    delete costumTranReceive;
}

void ui_costum_tran::ReadData()
{
    QByteArray buf;
       buf =  serialPort->readAll();

       if (!buf.isEmpty())
       {
           if(buf[0] == 'C'||buf[0] == 'c')
           {
               //启动升级线程
               ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 重启成功，正在升级..."));
               timer->stop();

#if QT_MAJOR_VERSION > 4
               disconnect(serialPort, &QSerialPort::readyRead, this, &ui_ymodem::ReadData);// 连接固件升级的槽函数
#else
               disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(ui_costum_tran::ReadData()));// 连接固件升级的槽函数
#endif
               on_pushButtonExportHistoricalData_clicked();
           }
       }
       buf.clear();
}

void ui_costum_tran::updateSerialPort(QSerialPort *port)
{
    if(!setPort(port))
    {
        qDebug("%s set serial port failed.", __func__);
    }
    else
    {
        emit serialPortUpdated(port);
    }
}

void ui_costum_tran::on_transmitBrowse_clicked()
{
    ui->transmitPath->setText(QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"), ".", QStringLiteral("任意文件 (*.*)")));

    if(ui->transmitPath->text().isEmpty() != true)
    {
        ui->pushButtonExportHistoricalData->setEnabled(true);
    }
    else
    {
        ui->pushButtonExportHistoricalData->setDisabled(true);
    }
}

void ui_costum_tran::on_receiveBrowse_clicked()
{
    ui->receivePath->setText(QFileDialog::getExistingDirectory(this, QStringLiteral("选择目录"), QStringLiteral("."), QFileDialog::ShowDirsOnly));

    if(ui->receivePath->text().isEmpty() != true)
    {
        ui->pushButtonExportAlarmData->setEnabled(true);
    }
    else
    {
        ui->pushButtonExportAlarmData->setDisabled(true);
    }
}

void ui_costum_tran::on_pushButtonExportHistoricalData_clicked()
{
    if(transmitButtonStatus == false)
    {
        if(openPort())
        {
            closePort();
            mode = CostumTran::RequestData;
            startRecv();
        }
        else
        {
            QMessageBox::warning(this, tr("Export"), tr("Serial Port Open Failed."), tr("Close"));
        }
    }
    else
    {
        costumTranReceive->stopReceive();
    }
}

void ui_costum_tran::on_pushButtonExportAlarmData_clicked()
{
    if(receiveButtonStatus == false)
    {
        if(openPort())
        {
            closePort();
            mode = CostumTran::RequestAlarm;
            startRecv();
        }
        else
        {
            QMessageBox::warning(this, tr("Export"), tr("Serial Port Open Failed."), tr("Close"));
        }
    }
    else
    {
        costumTranReceive->stopReceive();
    }
}

void ui_costum_tran::startRecv()
{
    QStringList list = genPortSum().remove(',').split('\40');

    costumTranReceive->setFilePath(ui->receivePath->text());
    costumTranReceive->setMode(mode);
    if(costumTranReceive->startReceive() == true)
    {
        if(CostumTran::RequestAlarm == mode)
        {
            receiveButtonStatus = true;

            ui->transmitBrowse->setDisabled(true);
            ui->pushButtonExportHistoricalData->setDisabled(true);

            ui->receiveBrowse->setDisabled(true);
            ui->pushButtonExportAlarmData->setText(tr("Cancel"));
        } else {
            transmitButtonStatus = true;

            ui->receiveBrowse->setDisabled(true);
            ui->pushButtonExportAlarmData->setDisabled(true);

            ui->transmitBrowse->setDisabled(true);
            ui->pushButtonExportHistoricalData->setText(tr("Cancel"));
        }

        qDebug("%s(%s-%s-%s-%s) opened", qPrintable(list.at(0)), qPrintable(list.at(2)), qPrintable(list.at(3)),
               qPrintable(list.at(4).at(0)), qPrintable(list.at(5)));

        emit reportStatus(genPortSum(), true);
    }
    else
    {
        QMessageBox::warning(this, tr("Export"), tr("Serial Port Open Failed."), tr("Close"));
    }
}

void ui_costum_tran::transmitProgress(int progress)
{
    Q_UNUSED(progress);
//    ui->transmitProgress->setValue(progress);
}

void ui_costum_tran::receiveProgress(int progress)
{
    Q_UNUSED(progress);
//    ui->receiveProgress->setValue(progress);
}

void ui_costum_tran::transmitStatus(CostumTran::Status status)
{
    switch(status)
    {
        case CostumTranTransmit::StatusEstablish:
        {
            ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 与设备握手成功"));
            break;
        }

        case CostumTranTransmit::StatusTransmit:
        {
            //ui->tx_firmware_msg->append("-->[info ] 正在发送。。。。");
            break;
        }

        case CostumTranTransmit::StatusFinish:
        {
            transmitButtonStatus = false;
            ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 固件升级成功"));
           // ui->comButton->setEnabled(true);

            ui->receiveBrowse->setEnabled(true);

            ui->receivePath->setText("place holder");
            if(ui->receivePath->text().isEmpty() != true)
            {
                ui->pushButtonExportAlarmData->setEnabled(true);
            }

            ui->transmitBrowse->setEnabled(true);
            ui->pushButtonExportHistoricalData->setText(tr("Start"));

            QMessageBox::warning(this, u8"成功", u8"文件发送成功！", u8"关闭");

            break;
        }

        case CostumTranTransmit::StatusAbort:
        {
            transmitButtonStatus = false;
            ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 升级被用户取消"));
           // ui->comButton->setEnabled(true);

            ui->receiveBrowse->setEnabled(true);

            ui->receivePath->setText("place holder");
            if(ui->receivePath->text().isEmpty() != true)
            {
                ui->pushButtonExportAlarmData->setEnabled(true);
            }

            ui->transmitBrowse->setEnabled(true);
            ui->pushButtonExportHistoricalData->setText(tr("Start"));

            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("文件发送失败！"), QStringLiteral("关闭"));

            break;
        }

        case CostumTranTransmit::StatusTimeout:
        {
            transmitButtonStatus = false;
             ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 和设备链接超时"));
           // ui->comButton->setEnabled(true);

            ui->receiveBrowse->setEnabled(true);

            ui->receivePath->setText("place holder");
            if(ui->receivePath->text().isEmpty() != true)
            {
                ui->pushButtonExportAlarmData->setEnabled(true);
            }

            ui->transmitBrowse->setEnabled(true);
            ui->pushButtonExportHistoricalData->setText(tr("Start"));

            QMessageBox::warning(this, u8"失败", u8"文件发送失败！", u8"关闭");

            break;
        }

        default:
        {
            transmitButtonStatus = false;
            ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 未知的异常出现"));
           // ui->comButton->setEnabled(true);

            ui->receiveBrowse->setEnabled(true);

            ui->receivePath->setText("place holder");
            if(ui->receivePath->text().isEmpty() != true)
            {
                ui->pushButtonExportAlarmData->setEnabled(true);
            }

            ui->transmitBrowse->setEnabled(true);
            ui->pushButtonExportHistoricalData->setText(tr("Start"));

            QMessageBox::warning(this, u8"失败", u8"文件发送失败！", u8"关闭");
        }
    }
}

void ui_costum_tran::showRecvStatus(CostumTran::Status status)
{
    switch (status)
    {
        case CostumTranReceive::StatusEstablish:
        {
            break;
        }
        case CostumTranReceive::StatusTransmit:
        {
            break;
        }
        case CostumTranReceive::StatusFinish:
        {
            QMessageBox::warning(this, tr("Export"), tr("Data Export Successful"), tr("Close"));
            break;
        }
        case CostumTranReceive::StatusAbort:
        {
            QMessageBox::information(this, tr("Export"), tr("Data Export Cancelled."), tr("Close"));
            break;
        }
        case CostumTranReceive::StatusTimeout:
        {
            QMessageBox::information(this, tr("Export"), tr("Communication Time Out, Export Finished."), tr("Close"));
            break;
        }
        default:
        {
            QMessageBox::warning(this, tr("Export"), tr("Data Export Failed"), tr("Close"));
            break;
        }
    }
}

void ui_costum_tran::receiveStatus(CostumTranReceive::Status status)
{
    switch(status)
    {
        case CostumTranReceive::StatusEstablish:
        {
            break;
        }

        case CostumTranReceive::StatusTransmit:
        {
            break;
        }

        case CostumTranReceive::StatusFinish:
        {
            receiveButtonStatus = false;

           // ui->comButton->setEnabled(true);

            ui->transmitBrowse->setEnabled(true);

            ui->transmitPath->setText("place holder");
            if(ui->transmitPath->text().isEmpty() != true)
            {
                ui->pushButtonExportHistoricalData->setEnabled(true);
            }

            ui->receiveBrowse->setEnabled(true);
            ui->pushButtonExportAlarmData->setText(tr("Start"));

            emit reportStatus(QString("%1 CLOSED").arg(genPortSum().split('\40').at(0)), false);
            break;
        }

        case CostumTranReceive::StatusAbort:
        {
            receiveButtonStatus = false;
            transmitButtonStatus = false;

           // ui->comButton->setEnabled(true);

            ui->transmitBrowse->setEnabled(true);

            ui->transmitPath->setText("place holder");
            if(ui->transmitPath->text().isEmpty() != true)
            {
                ui->pushButtonExportHistoricalData->setEnabled(true);
            }

            ui->receiveBrowse->setEnabled(true);
            ui->pushButtonExportAlarmData->setText(tr("Start"));
            ui->pushButtonExportAlarmData->setEnabled(true);
            ui->pushButtonExportHistoricalData->setText(tr("Start"));

            emit reportStatus(QString("%1 CLOSED").arg(genPortSum().split('\40').at(0)), false);
            break;
        }

        case CostumTranReceive::StatusTimeout:
        {
            receiveButtonStatus = false;
            transmitButtonStatus = false;

            //ui->comButton->setEnabled(true);

            ui->transmitBrowse->setEnabled(true);

            ui->transmitPath->setText("place holder");
            if(ui->transmitPath->text().isEmpty() != true)
            {
                ui->pushButtonExportHistoricalData->setEnabled(true);
            }

            ui->receiveBrowse->setEnabled(true);
            ui->pushButtonExportAlarmData->setText(tr("Start"));
            ui->pushButtonExportAlarmData->setEnabled(true);
            ui->pushButtonExportHistoricalData->setText(tr("Start"));

            emit reportStatus(QString("%1 CLOSED").arg(genPortSum().split('\40').at(0)), false);
            break;
        }
        default:
        {
            receiveButtonStatus = false;

            //ui->comButton->setEnabled(true);

            ui->transmitBrowse->setEnabled(true);

            ui->transmitPath->setText("place holder");
            if(ui->transmitPath->text().isEmpty() != true)
            {
                ui->pushButtonExportHistoricalData->setEnabled(true);
            }

            ui->receiveBrowse->setEnabled(true);
            ui->pushButtonExportAlarmData->setText(tr("Start"));

            emit reportStatus(QString("%1 CLOSED").arg(genPortSum().split('\40').at(0)), false);
        }
    }

    showRecvStatus(status);
}

QSerialPort::StopBits ui_costum_tran::to_convert_stopbit(QString  bit)
{
   if(bit == tr("1"))
       return QSerialPort::OneStop;
   else if (bit == tr("1.5"))
       return QSerialPort::OneAndHalfStop;
   else if (bit == tr("2"))
       return QSerialPort::TwoStop;
   return QSerialPort::OneStop;
}

QSerialPort::DataBits ui_costum_tran::to_convert_databit(QString  bit)
{
   if(bit == tr("8"))
       return QSerialPort::Data8;

   if(bit == tr("7"))
       return QSerialPort::Data7;

   if(bit == tr("6"))
       return QSerialPort::Data6;

   if(bit == tr("5"))
       return QSerialPort::Data5;
   return QSerialPort::Data8;

}

QSerialPort::Parity ui_costum_tran::to_convert_paritybit(QString  bit)
{
   if(bit == tr("None"))
       return QSerialPort::NoParity;
   if(bit == tr("Odd"))
       return QSerialPort::OddParity;
   if(bit == tr("Even"))
       return QSerialPort::EvenParity;
    return QSerialPort::NoParity;

}

void ui_costum_tran::ss_timer_irq()
{
    if ((serialPort->isOpen() == false))
    {
        ui->tx_firmware_msg->append(QStringLiteral("-->[error]检查串口是否被打开"));
        if(timer->isActive()){
          timer->stop();
//          ui->btn_start_update->setEnabled(true);
        }
       return ;
    }
   // qDebug()<< "on_ss_timer_irq";
    serialPort->write("S");
}

void ui_costum_tran::show_received(QString msg)
{
    ui->tx_firmware_msg->append(QString("receive: ") + msg);
}

void ui_costum_tran::show_sent(QString msg)
{
    ui->tx_firmware_msg->append(QString("sent   : ") + msg);
}
