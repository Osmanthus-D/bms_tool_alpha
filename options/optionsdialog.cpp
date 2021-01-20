#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);

    setModal(true);
    initTabPort();
    serialPort = new QSerialPort;
    pref = new Preference(this);

    connect(this, SIGNAL(finished(int)), this, SLOT(updateOptions(int)));
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
    delete serialPort;
}

void OptionsDialog::load()
{
    QString parity;
    QComboBox *box = ui->comboBoxPort;

    box->setCurrentIndex(box->findData(pref->portName()));

    box = ui->comboBoxBaudRate;
    box->setCurrentIndex(box->findData(pref->baudRate()));

    box = ui->comboBoxDataBits;
    box->setCurrentIndex(box->findData(pref->dataBits()));

    box = ui->comboBoxParity;
    parity = pref->parity();
    if(!parity.compare("None", Qt::CaseInsensitive)) {
        box->setCurrentIndex(0);
    } else if(!parity.compare("Even", Qt::CaseInsensitive)) {
        box->setCurrentIndex(1);
    } else if(!parity.compare("odd", Qt::CaseInsensitive)) {
        box->setCurrentIndex(2);
    } else {
        box->setCurrentIndex(-1);
    }

    box = ui->comboBoxStopBits;
    box->setCurrentIndex(box->findData(pref->stopBits()));

    box = ui->comboBoxCoreNum;
    box->setCurrentIndex(box->findText(QString::number(pref->coreNum())));

    ui->lineEditTempLowerLimit->setText(QString::number(pref->tempLowerLimit()));
    ui->lineEditTempUpperLimit->setText(QString::number(pref->tempUpperLimit()));
    ui->lineEditVoltLowerLimit->setText(QString::number(pref->voltLowerLimit()));
    ui->lineEditVoltUpperLimit->setText(QString::number(pref->voltUpperLimit()));
}

void OptionsDialog::initTabPort()
{
    ui->comboBoxBaudRate->clear();
    ui->comboBoxBaudRate->addItem("9600", QSerialPort::Baud9600);
    ui->comboBoxBaudRate->addItem("19200", QSerialPort::Baud19200);
    ui->comboBoxBaudRate->addItem("38400", QSerialPort::Baud38400);
    ui->comboBoxBaudRate->addItem("115200", QSerialPort::Baud115200);
    ui->comboBoxBaudRate->setCurrentIndex(0);

    ui->comboBoxDataBits->clear();
    ui->comboBoxDataBits->addItem("5", QSerialPort::Data5);
    ui->comboBoxDataBits->addItem("6", QSerialPort::Data6);
    ui->comboBoxDataBits->addItem("7", QSerialPort::Data7);
    ui->comboBoxDataBits->addItem("8", QSerialPort::Data8);
    ui->comboBoxDataBits->setCurrentIndex(3);

    ui->comboBoxParity->clear();
    ui->comboBoxParity->addItem("None", QSerialPort::NoParity);
    ui->comboBoxParity->addItem("Even", QSerialPort::EvenParity);
    ui->comboBoxParity->addItem("Odd", QSerialPort::OddParity);
    ui->comboBoxParity->setCurrentIndex(0);

    ui->comboBoxStopBits->clear();
    ui->comboBoxStopBits->addItem("1", QSerialPort::OneStop);
    ui->comboBoxStopBits->addItem("1.5", QSerialPort::OneAndHalfStop);
    ui->comboBoxStopBits->addItem("2", QSerialPort::TwoStop);
    ui->comboBoxStopBits->setCurrentIndex(0);
}

void OptionsDialog::listAvailiablePorts()
{
    ui->comboBoxPort->clear();

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        ui->comboBoxPort->addItem(QString("%1 (%2)").arg(info.description()).arg(info.portName()), info.portName());
    }
}

void OptionsDialog::updateOptions(int result)
{
    bool ok = false;
    int index = 0;
    int value = 0;
    double convert;

    if(pref && QDialog::Accepted == result)
    {
        convert = ui->lineEditTempLowerLimit->text().toDouble(&ok);
        if(!ok)
        {
            QMessageBox::information(this, tr("Info"), tr("Temp Lower Limit unresolved."));
            return;
        }

        convert = ui->lineEditTempUpperLimit->text().toDouble(&ok);
        if(!ok)
        {
            QMessageBox::information(this, tr("Info"), tr("Temp Upper Limit unresolved."));
            return;
        }

        convert = ui->lineEditVoltLowerLimit->text().toDouble(&ok);
        if(!ok)
        {
            QMessageBox::information(this, tr("Info"), tr("Volt Lower Limit unresolved."));
            return;
        }

        convert = ui->lineEditVoltUpperLimit->text().toDouble(&ok);
        if(!ok)
        {
            QMessageBox::information(this, tr("Info"), tr("Volt Upper Limit unresolved."));
            return;
        }

        // tab port
        index = ui->comboBoxPort->currentIndex();
        serialPort->setPortName(ui->comboBoxPort->itemData(index).toString());
        pref->setPortName(ui->comboBoxPort->itemData(index).toString());

        index = ui->comboBoxBaudRate->currentIndex();
        value = ui->comboBoxBaudRate->itemData(index).toInt();
        serialPort->setBaudRate((QSerialPort::BaudRate)value);
        pref->setBaudRate(ui->comboBoxBaudRate->currentText().toInt());

        index = ui->comboBoxDataBits->currentIndex();
        value = ui->comboBoxDataBits->itemData(index).toInt();
        serialPort->setDataBits((QSerialPort::DataBits)value);
        pref->setDataBits(ui->comboBoxDataBits->currentText().toInt());

        index = ui->comboBoxParity->currentIndex();
        value = ui->comboBoxParity->itemData(index).toInt();
        serialPort->setParity((QSerialPort::Parity)value);
        pref->setParity(ui->comboBoxParity->currentText());

        index = ui->comboBoxStopBits->currentIndex();
        value = ui->comboBoxStopBits->itemData(index).toInt();
        serialPort->setStopBits((QSerialPort::StopBits)value);
        pref->setStopBits(ui->comboBoxStopBits->itemData(index).toInt());

        emit serialPortUpdated(serialPort);

        // tab display
        pref->setCoreNum(ui->comboBoxCoreNum->currentText().toInt());

        convert = ui->lineEditTempLowerLimit->text().toDouble();
        pref->setTempLowerLimit(convert);

        convert = ui->lineEditTempUpperLimit->text().toDouble();
        pref->setTempUpperLimit(convert);

        convert = ui->lineEditVoltLowerLimit->text().toDouble();
        pref->setVoltLowerLimit(convert);

        convert = ui->lineEditVoltUpperLimit->text().toDouble();
        pref->setVoltUpperLimit(convert);

        pref->sync();
    }
}
