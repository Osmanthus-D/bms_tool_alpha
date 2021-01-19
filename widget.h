#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMenuBar>
#include <QDebug>
#include <QSettings>
#include "ui_costum_tran.h"
#include "ui_ymodem.h"
#include "infodisplay.h"
#include "optionsdialog.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void showOptionsDialog();
    void dispOptionsChanged();
    void statusChanged(QString atip, bool ok, const QString & module = "");

private:
    Ui::Widget *ui;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QAction *actionOptions;
    QAction *actionPlay;
    QAction *actionStop;
    QSettings *iniFile;
    Preference *pref;
    ui_costum_tran *costumTran;
    ui_ymodem *ymodem;
    InfoDispaly *infoDisplay;
    OptionsDialog *optionsDlg;
};

#endif // WIDGET_H
