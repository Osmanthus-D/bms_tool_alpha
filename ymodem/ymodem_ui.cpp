#include "ymodem_ui.h"
#include "ui_ui_ymodem.h"
#include <QFileDialog>
#include <QtCore>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

ui_ymodem::ui_ymodem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ui_ymodem),
    ymodemFileTransmit(new YmodemFileTransmit),
    ymodemFileReceive(new YmodemFileReceive)
{
    transmitButtonStatus = false;
    receiveButtonStatus  = false;

    ui->setupUi(this);
    ui->groupBox->setVisible(false);
//    Find_SerialPort();

    // restore serial port properties set last time
    if(!initPort())
    {
        qDebug("%s serial port init failed.", __func__);
    }

    timer = new QTimer();
    connect(ymodemFileTransmit, SIGNAL(transmitProgress(int)), this, SLOT(transmitProgress(int)));
    connect(ymodemFileReceive, SIGNAL(receiveProgress(int)), this, SLOT(receiveProgress(int)));
    connect(ymodemFileTransmit, SIGNAL(transmitStatus(YmodemFileTransmit::Status)), this, SLOT(transmitStatus(YmodemFileTransmit::Status)));
    connect(ymodemFileReceive, SIGNAL(receiveStatus(YmodemFileReceive::Status)), this, SLOT(receiveStatus(YmodemFileReceive::Status)));

    connect(ymodemFileReceive, SIGNAL(sendReceived(QString)), this, SLOT(show_received(QString)));
    connect(ymodemFileReceive, SIGNAL(sendSent(QString)), this, SLOT(show_sent(QString)));

    connect(this, SIGNAL(serialPortUpdated(QSerialPort *)), ymodemFileReceive, SLOT(updateSerialPort(QSerialPort *)));
    connect(this, SIGNAL(serialPortUpdated(QSerialPort *)), ymodemFileReceive, SLOT(updateSerialPort(QSerialPort *)));

    connect(timer,SIGNAL(timeout()),this,SLOT(ss_timer_irq()));
}

ui_ymodem::~ui_ymodem()
{
    delete ui;
    delete ymodemFileTransmit;
    delete ymodemFileReceive;
}

void ui_ymodem::ReadData()
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
               disconnect(serialPort, SIGNAL(readyRead()), this, SLOT(ui_ymodem::ReadData()));// 连接固件升级的槽函数
#endif
               on_transmitButton_clicked();
           }
       }
       buf.clear();
}

void ui_ymodem::updateSerialPort(QSerialPort *port)
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

void ui_ymodem::on_transmitBrowse_clicked()
{
    ui->transmitPath->setText(QFileDialog::getOpenFileName(this, QStringLiteral("打开文件"), ".", QStringLiteral("任意文件 (*.*)")));

    if(ui->transmitPath->text().isEmpty() != true)
    {
        ui->transmitButton->setEnabled(true);
    }
    else
    {
        ui->transmitButton->setDisabled(true);
    }
}

void ui_ymodem::on_receiveBrowse_clicked()
{
    ui->receivePath->setText(QFileDialog::getExistingDirectory(this, QStringLiteral("选择目录"), QStringLiteral("."), QFileDialog::ShowDirsOnly));

    if(ui->receivePath->text().isEmpty() != true)
    {
        ui->receiveButton->setEnabled(true);
    }
    else
    {
        ui->receiveButton->setDisabled(true);
    }
}

void ui_ymodem::on_transmitButton_clicked()
{
    QStringList list = genPortSum().remove(',').split('\40');

    if(transmitButtonStatus == false)
    {
        closePort();

        ymodemFileTransmit->setFileName(ui->transmitPath->text());

        if(ymodemFileTransmit->startTransmit() == true)
        {
            transmitButtonStatus = true;

            //ui->comButton->setDisabled(true);

            ui->receiveBrowse->setDisabled(true);
            ui->receiveButton->setDisabled(true);

            ui->transmitBrowse->setDisabled(true);
            ui->transmitButton->setText(QStringLiteral("取消"));
            ui->transmitProgress->setValue(0);

            qDebug("%s(%s-%s-%s-%s) opened", qPrintable(list.at(0)), qPrintable(list.at(2)), qPrintable(list.at(3)),
                   qPrintable(list.at(4).at(0)), qPrintable(list.at(5)));

            emit reportStatus(genPortSum(), true);
        }
        else
        {
            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("文件发送失败！"), QStringLiteral("关闭"));
        }
    }
    else
    {
        ymodemFileTransmit->stopTransmit();
    }
}

void ui_ymodem::on_receiveButton_clicked()
{
    QStringList list = genPortSum().remove(',').split('\40');

    if(receiveButtonStatus == false)
    {
        closePort();

        ymodemFileReceive->setFilePath(ui->receivePath->text());
        if(ymodemFileReceive->startReceive() == true)
        {
            receiveButtonStatus = true;

           // ui->comButton->setDisabled(true);

            ui->transmitBrowse->setDisabled(true);
            ui->transmitButton->setDisabled(true);

            ui->receiveBrowse->setDisabled(true);
            ui->receiveButton->setText(QStringLiteral("取消"));
            ui->receiveProgress->setValue(0);

            qDebug("%s(%s-%s-%s-%s) opened", qPrintable(list.at(0)), qPrintable(list.at(2)), qPrintable(list.at(3)),
                   qPrintable(list.at(4).at(0)), qPrintable(list.at(5)));

            emit reportStatus(genPortSum(), true);
        }
        else
        {
            QMessageBox::warning(this, u8"失败", u8"文件接收失败！", u8"关闭");
        }
    }
    else
    {
        ymodemFileReceive->stopReceive();
    }
}

void ui_ymodem::transmitProgress(int progress)
{
    ui->transmitProgress->setValue(progress);
}

void ui_ymodem::receiveProgress(int progress)
{
    ui->receiveProgress->setValue(progress);
}

void ui_ymodem::transmitStatus(Ymodem::Status status)
{
    switch(status)
    {
        case YmodemFileTransmit::StatusEstablish:
        {
            ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 与设备握手成功"));
            break;
        }

        case YmodemFileTransmit::StatusTransmit:
        {
            //ui->tx_firmware_msg->append("-->[info ] 正在发送。。。。");
            break;
        }

        case YmodemFileTransmit::StatusFinish:
        {
            transmitButtonStatus = false;
            ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 固件升级成功"));
           // ui->comButton->setEnabled(true);

            ui->receiveBrowse->setEnabled(true);

            if(ui->receivePath->text().isEmpty() != true)
            {
                ui->receiveButton->setEnabled(true);
            }

            ui->transmitBrowse->setEnabled(true);
            ui->transmitButton->setText(tr("Transmit"));

            QMessageBox::warning(this, tr("Update"), tr("Firmware Update Successful."), tr("Close"));

            break;
        }

        case YmodemFileTransmit::StatusAbort:
        {
            transmitButtonStatus = false;
            ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 升级被用户取消"));
           // ui->comButton->setEnabled(true);

            ui->receiveBrowse->setEnabled(true);

            if(ui->receivePath->text().isEmpty() != true)
            {
                ui->receiveButton->setEnabled(true);
            }

            ui->transmitBrowse->setEnabled(true);
            ui->transmitButton->setText(tr("Transmit"));

            QMessageBox::warning(this, tr("Update"), tr("Firmware Update Cancelled."), tr("Close"));

            break;
        }

        case YmodemFileTransmit::StatusTimeout:
        {
            transmitButtonStatus = false;
             ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 和设备链接超时"));
           // ui->comButton->setEnabled(true);

            ui->receiveBrowse->setEnabled(true);

            if(ui->receivePath->text().isEmpty() != true)
            {
                ui->receiveButton->setEnabled(true);
            }

            ui->transmitBrowse->setEnabled(true);
            ui->transmitButton->setText(tr("Transmit"));

            QMessageBox::warning(this, tr("Update"), tr("Firmware Update Time Out."), ("Close"));

            break;
        }

        default:
        {
            transmitButtonStatus = false;
            ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 未知的异常出现"));
           // ui->comButton->setEnabled(true);

            ui->receiveBrowse->setEnabled(true);

            if(ui->receivePath->text().isEmpty() != true)
            {
                ui->receiveButton->setEnabled(true);
            }

            ui->transmitBrowse->setEnabled(true);
            ui->transmitButton->setText(tr("Transmit"));

            QMessageBox::warning(this, tr("Update"), tr("Firmware Update Failed."), tr("Close"));
        }
    }
}

void ui_ymodem::receiveStatus(YmodemFileReceive::Status status)
{
    switch(status)
    {
        case YmodemFileReceive::StatusEstablish:
        {
            break;
        }

        case YmodemFileReceive::StatusTransmit:
        {
            break;
        }

        case YmodemFileReceive::StatusFinish:
        {
            receiveButtonStatus = false;

           // ui->comButton->setEnabled(true);

            ui->transmitBrowse->setEnabled(true);

            if(ui->transmitPath->text().isEmpty() != true)
            {
                ui->transmitButton->setEnabled(true);
            }

            ui->receiveBrowse->setEnabled(true);
            ui->receiveButton->setText(tr("Receive"));

            QMessageBox::warning(this, tr("Update"), tr("File Received Successfully."), tr("Close"));

            break;
        }

        case YmodemFileReceive::StatusAbort:
        {
            receiveButtonStatus = false;

           // ui->comButton->setEnabled(true);

            ui->transmitBrowse->setEnabled(true);

            if(ui->transmitPath->text().isEmpty() != true)
            {
                ui->transmitButton->setEnabled(true);
            }

            ui->receiveBrowse->setEnabled(true);
            ui->receiveButton->setText(tr("Receive"));

            QMessageBox::warning(this, tr("Update"), tr("File Receiving Cancelled."), tr("Close"));

            break;
        }

        case YmodemFileReceive::StatusTimeout:
        {
            receiveButtonStatus = false;

            //ui->comButton->setEnabled(true);

            ui->transmitBrowse->setEnabled(true);

            if(ui->transmitPath->text().isEmpty() != true)
            {
                ui->transmitButton->setEnabled(true);
            }

            ui->receiveBrowse->setEnabled(true);
            ui->receiveButton->setText(tr("Receive"));

            QMessageBox::warning(this, tr("Update"), tr("File Receiving Time Out."), tr("Close"));

            break;
        }

        default:
        {
            receiveButtonStatus = false;

            //ui->comButton->setEnabled(true);

            ui->transmitBrowse->setEnabled(true);

            if(ui->transmitPath->text().isEmpty() != true)
            {
                ui->transmitButton->setEnabled(true);
            }

            ui->receiveBrowse->setEnabled(true);
            ui->receiveButton->setText(tr("Receive"));

            QMessageBox::warning(this, tr("Update"), tr("File Received Failed."), tr("Closed"));
        }
    }
}

void ui_ymodem::Find_SerialPort()
{
    const auto infos = QSerialPortInfo::availablePorts();
    ui->cbx_com_name->clear();
    for(const QSerialPortInfo &info : infos)
    {
        QSerialPort serial;
        serial.setPort(info);
        if(serial.open(QIODevice::ReadWrite))
        {
           ui->cbx_com_name->addItem(info.portName());
           serial.close();
        }
    }
}

QSerialPort::StopBits ui_ymodem::to_convert_stopbit(QString  bit)
{
   if(bit == tr("1"))
       return QSerialPort::OneStop;
   else if (bit == tr("1.5"))
       return QSerialPort::OneAndHalfStop;
   else if (bit == tr("2"))
       return QSerialPort::TwoStop;
   return QSerialPort::OneStop;
}

QSerialPort::DataBits ui_ymodem::to_convert_databit(QString  bit)
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

QSerialPort::Parity ui_ymodem::to_convert_paritybit(QString  bit)
{
   if(bit == tr("None"))
       return QSerialPort::NoParity;
   if(bit == tr("Odd"))
       return QSerialPort::OddParity;
   if(bit == tr("Even"))
       return QSerialPort::EvenParity;
    return QSerialPort::NoParity;

}

void ui_ymodem::on_btn_start_update_clicked()
{
    ui->tx_firmware_msg->append(QStringLiteral("-->[info ] 等待重新启动设备升级程序"));
#if QT_MAJOR_VERSION > 4
    // disconnect(serialPort, &QSerialPort::readyRead, this, &MainWindow::ReadData);// 断开调试的槽函数
   connect(serialPort, &QSerialPort::readyRead, this, &ui_ymodem::ReadData);// 连接固件升级的槽函数
#else
   connect(serialPort, SIGNAL(readyRead()), this, SLOT(ReadData()));
#endif

   //ymodem_signel(ui_ymodem::Statusupdatebtnclick); // 传递信号给父容器处理
   timer->start(100);
   ui->btn_start_update->setEnabled(false);
}
void ui_ymodem::ss_timer_irq()
{
    if ((serialPort->isOpen() == false))
    {
        ui->tx_firmware_msg->append(QStringLiteral("-->[error]检查串口是否被打开"));
        if(timer->isActive()){
          timer->stop();
          ui->btn_start_update->setEnabled(true);
        }
       return ;
    }
   // qDebug()<< "on_ss_timer_irq";
    serialPort->write("S");
}

void ui_ymodem::show_received(QString msg)
{
    ui->tx_firmware_msg->append(QString("receive: ") + msg);
}

void ui_ymodem::show_sent(QString msg)
{
    ui->tx_firmware_msg->append(QString("sent   : ") + msg);
}
