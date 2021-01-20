#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSettings>
#include <QAbstractButton>
#include "preference.h"

namespace Ui {
class OptionsDialog;
}

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
    void load();
    void listAvailiablePorts();

signals:
    void serialPortUpdated(QSerialPort *serialport);

private slots:
    void updateOptions(int result);

private:
    Ui::OptionsDialog *ui;
    QSerialPort *serialPort;
    Preference *pref;
    void initTabPort();
};

#endif // OPTIONSDIALOG_H
