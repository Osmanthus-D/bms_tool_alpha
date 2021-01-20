#ifndef UI_COSTUM_TRAN_H
#define UI_COSTUM_TRAN_H

#include <QWidget>
#include <QSerialPort>
#include <QDateTime>
#include "serialporthelper.h"
#include "CostumTranTransmit.h"
#include "CostumTranReceive.h"

namespace Ui {
class ui_costum_tran;
}

class ui_costum_tran : public QWidget, public SerialPortHelper
{
    Q_OBJECT

public:
    explicit ui_costum_tran(QWidget *parent = NULL) ;
    ~ui_costum_tran();
    enum Status
    {
      StatusEstablish,
      StatusTransmit,
      StatusFinish,
      StatusAbort,
      StatusTimeout,
      StatusError,
      Statusupdatebtnclick
    };

signals:
    void ymodem_signel(ui_costum_tran::Status status);
    void reportStatus(QString tip, bool ok, const QString & module = "EXPORT");
    void serialPortUpdated(QSerialPort *port);

public slots:
    void updateSerialPort(QSerialPort *port);

private slots:
    //void on_btn_start_update_clicked();
    void transmitStatus(CostumTranTransmit::Status status);
    void receiveStatus(CostumTranReceive::Status status);
    void transmitProgress(int progress);
    void receiveProgress(int progress);
    void on_transmitBrowse_clicked();
    void ReadData();
    void ss_timer_irq();
    void on_receiveBrowse_clicked();
    void on_pushButtonExportHistoricalData_clicked();
    void on_pushButtonExportAlarmData_clicked();
    void on_btn_find_seriaport_clicked();
    void on_btn_open_port_clicked();
    void show_received(QString msg);
    void show_sent(QString msg);
    void startRecv();

private:
    Ui::ui_costum_tran *ui;
    QTimer *timer;
    CostumTranTransmit *costumTranTransmit;
    CostumTranReceive *costumTranReceive;
    bool transmitButtonStatus;
    bool receiveButtonStatus;
    CostumTran::Mode mode;
   // void ymodem_start_transmit();
    void Find_SerialPort();
    QSerialPort::StopBits to_convert_stopbit(QString bit);
    QSerialPort::DataBits to_convert_databit(QString  bit);
    QSerialPort::Parity to_convert_paritybit(QString  bit);
};

#endif // UI_COSTUM_TRAN_H
