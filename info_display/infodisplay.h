#ifndef INFODISPLAY_H
#define INFODISPLAY_H

#include <QWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QTimer>
#include <QThread>
#include <QSerialPort>
#include <QtEndian>
#include "modbushelper.h"
#include "preference.h"
#include "defs.h"

const QImage imageYes(":/img/yes_96x96.png");
const QImage imageWarn(":/img/warning_96x96.png");

namespace Ui {
class InfoDisplay;
}

class InfoDispaly : public QWidget
{
    Q_OBJECT

public:
    explicit InfoDispaly(QWidget *parent = 0);
    ~InfoDispaly();
    void displayTest();

signals:
    void rtuReadSig();
    void reportStatus(QString tip, bool ok, const QString & module = "DISPLAY");

public slots:
    void setCoreNumDisp(int n);
    void setVoltageRange(double minValue, double maxValue);
    void setTempRange(double minValue, double maxValue);
    void refreshCoreProtect(uint8_t * a);
    void play();
    void stop();

private:
    Ui::InfoDisplay *ui;
    QTimer *timer;
    QThread *thread;
    QSerialPort *serialPort;
    ModbusHelper *modbusHelper;
    Preference *pref;
    device_desc_t *desc;

    QPixmap pixmapYesOrig;
    QPixmap pixmapWarnOrig;
    QPixmap pixmapYes;
    QPixmap pixmapWarn;

    int createModbus(const char *device, int baud, char parity,
                      int data_bit, int stop_bit);
    void destroyModbus();
    void setPixmapScaled();
    void setGroupBoxCoreNum(QGroupBox *gb, int n, int prefixLen);
    void setGroupBoxStatus(QGroupBox *box, QString prefix, quint8 *buf);
    void initDeviceDesc();
};

#endif // INFODISPLAY_H
