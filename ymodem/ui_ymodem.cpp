#include "ui_ymodem.h"
#include "ui_ui_ymodem.h"
#include <QFileDialog>
#include <QtCore>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>

ui_ymodem::ui_ymodem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ui_ymodem),
    serialPort(new QSerialPort),
    ymodemFileTransmit(new YmodemFileTransmit),
    ymodemFileReceive(new YmodemFileReceive)
{
    transmitButtonStatus = false;
    receiveButtonStatus  = false;

    ui->setupUi(this);
    ui->groupBox->setVisible(false);
//    Find_SerialPort();
    timer = new QTimer();
    connect(ymodemFileTransmit, SIGNAL(transmitProgress(int)), this, SLOT(transmitProgress(int)));
    connect(ymodemFileReceive, SIGNAL(receiveProgress(int)), this, SLOT(receiveProgress(int)));
    connect(ymodemFileTransmit, SIGNAL(transmitStatus(YmodemFileTransmit::Status)), this, SLOT(transmitStatus(YmodemFileTransmit::Status)));
    connect(ymodemFileReceive, SIGNAL(receiveStatus(YmodemFileReceive::Status)), this, SLOT(receiveStatus(YmodemFileReceive::Status)));

    connect(ymodemFileReceive, SIGNAL(sendReceived(QString)), this, SLOT(show_received(QString)));
    connect(ymodemFileReceive, SIGNAL(sendSent(QString)), this, SLOT(show_sent(QString)));

    connect(timer,SIGNAL(timeout()),this,SLOT(ss_timer_irq()));
}

ui_ymodem::~ui_ymodem()
{
    delete ui;
    delete serialPort;
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
    qDebug() << __func__ << transmitButtonStatus;
    if(transmitButtonStatus == false)
    {
        serialPort->close();

        ymodemFileTransmit->setFileName(ui->transmitPath->text());
        ymodemFileTransmit->setPort(this->serialPort);
        //ymodemFileTransmit->setPortName(this->serialPort->);
       // ymodemFileTransmit->setPortBaudRate(ui->comBaudRate->currentText().toInt());

        if(ymodemFileTransmit->startTransmit() == true)
        {
            transmitButtonStatus = true;

            //ui->comButton->setDisabled(true);

            ui->receiveBrowse->setDisabled(true);
            ui->receiveButton->setDisabled(true);

            ui->transmitBrowse->setDisabled(true);
            ui->transmitButton->setText(QStringLiteral("取消"));
            ui->transmitProgress->setValue(0);
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
    if(receiveButtonStatus == false)
    {
        serialPort->close();

        ymodemFileReceive->setFilePath(ui->receivePath->text());
        ymodemFileReceive->setPortName(ui->cbx_com_name->currentText());
        ymodemFileReceive->setPortBaudRate(ui->cbx_bandrate->currentText().toInt());

        if(ymodemFileReceive->startReceive() == true)
        {
            receiveButtonStatus = true;

           // ui->comButton->setDisabled(true);

            ui->transmitBrowse->setDisabled(true);
            ui->transmitButton->setDisabled(true);

            ui->receiveBrowse->setDisabled(true);
            ui->receiveButton->setText(QStringLiteral("取消"));
            ui->receiveProgress->setValue(0);
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
            ui->transmitButton->setText(QStringLiteral("发送"));

            QMessageBox::warning(this, u8"成功", u8"文件发送成功！", u8"关闭");

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
            ui->transmitButton->setText(QStringLiteral("发送"));

            QMessageBox::warning(this, u8"失败", u8"文件发送失败！", u8"关闭");

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
            ui->transmitButton->setText(QStringLiteral("发送"));

            QMessageBox::warning(this, u8"失败", u8"文件发送失败！", u8"关闭");

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
            ui->transmitButton->setText(QStringLiteral("发送"));

            QMessageBox::warning(this, u8"失败", u8"文件发送失败！", u8"关闭");
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
            ui->receiveButton->setText(QStringLiteral("接收"));

            QMessageBox::warning(this, QStringLiteral("成功"), QStringLiteral("文件接收成功！"), QStringLiteral("关闭"));

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
            ui->receiveButton->setText(QStringLiteral("接收"));

            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("文件接收失败！"), QStringLiteral("关闭"));

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
            ui->receiveButton->setText(QStringLiteral("接收"));

            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("文件接收失败！"), QStringLiteral("关闭"));

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
            ui->receiveButton->setText(QStringLiteral("接收"));

            QMessageBox::warning(this, QStringLiteral("失败"), QStringLiteral("文件接收失败！"), QStringLiteral("关闭"));
        }
    }
}

void ui_ymodem::on_btn_find_seriaport_clicked()
{
    Find_SerialPort();
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

void ui_ymodem::on_btn_open_port_clicked()
{
    static bool button_status = false;

   if(button_status == false)
   {
        serialPort->setPortName(ui->cbx_com_name->currentText());
       if(serialPort->open(QSerialPort::ReadWrite) == true)
       {
           button_status = true;
           serialPort->setBaudRate(ui->cbx_bandrate->currentText().toInt());
           serialPort->setStopBits(to_convert_stopbit(ui->cbx_stopbit->currentText()));
           serialPort->setDataBits(to_convert_databit(ui->cbx_databit->currentText()));
           serialPort->setParity(to_convert_paritybit(ui->cbx_paritybit->currentText()));
           ui->btn_open_port->setText(QStringLiteral("关闭串口"));
           ui->cbx_bandrate->setEnabled(false);
           ui->cbx_stopbit->setEnabled(false);
           ui->cbx_databit->setEnabled(false);
           ui->cbx_paritybit->setEnabled(false);
           ui->cbx_com_name->setEnabled(false);
           ui->btn_find_seriaport->setEnabled(false);
           //this->ss_ui->connected_serial_port();
#if QT_MAJOR_VERSION > 4
           connect(serialPort, &QSerialPort::readyRead, this, &ui_ymodem::ReadData);
#else
//           connect(serialPort, SIGNAL(readyRead()), this, SLOT(ReadData()));
#endif

          // ui->transmitBrowse->setEnabled(true);
          // ui->receiveBrowse->setEnabled(true);

           if(ui->transmitPath->text().isEmpty() != true)
           {
               ui->transmitButton->setEnabled(true);
           }

           if(ui->receivePath->text().isEmpty() != true)
           {
               ui->receiveButton->setEnabled(true);
           }
       }
       else
       {
           QMessageBox::warning(this, QStringLiteral("串口打开失败"), QStringLiteral("请检查串口是否已被占用！"), QStringLiteral("关闭"));
       }
   }
   else
   {
       button_status = false;

       serialPort->close();

//       serialPort->clear();
//       serialPort->close();

       ui->btn_open_port->setText(QStringLiteral("打开串口"));
       ui->cbx_bandrate->setEnabled(true);
       ui->cbx_stopbit->setEnabled(true);
       ui->cbx_databit->setEnabled(true);
       ui->cbx_paritybit->setEnabled(true);
       ui->cbx_com_name->setEnabled(true);
       ui->btn_find_seriaport->setEnabled(true);

       ui->transmitBrowse->setDisabled(true);
      // ui->transmitButton->setDisabled(true);

       ui->receiveBrowse->setDisabled(true);
       //ui->receiveButton->setDisabled(true);
   }
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
