#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    pref = new Preference(this);
    optionsDlg = new OptionsDialog(this);
    infoDisplay = new InfoDispaly(this);
    costumTran = new ui_costum_tran(this);
    ymodem = new ui_ymodem(this);

    dispOptionsChanged();
    statusChanged(pref->portName() + " CLOSED", false);

    menuBar = new QMenuBar(this);
    menuFile = new QMenu(tr("&File"), this);
    menuEdit = new QMenu(tr("&Edit"), this);

    actionOptions = new QAction(tr("&Options"), this);
    actionOptions->setIcon(QIcon(":/img/config.png"));
    actionOptions->setShortcut(QKeySequence("Ctrl+O"));
    connect(actionOptions, SIGNAL(triggered()), this, SLOT(showOptionsDialog()));

    actionPlay = new QAction(tr("&Play"), this);
    actionPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    actionPlay->setShortcut(QKeySequence("Ctrl+P"));
    connect(actionPlay, SIGNAL(triggered()), infoDisplay, SLOT(play()));

    actionStop = new QAction(tr("&Stop"), this);
    actionStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    actionStop->setShortcut(QKeySequence("Ctrl+S"));
    connect(actionStop, SIGNAL(triggered()), infoDisplay, SLOT(stop()));

    menuFile->addAction(actionOptions);

    menuEdit->addAction(actionPlay);
    menuEdit->addAction(actionStop);

    menuBar->addMenu(menuFile);
    menuBar->addMenu(menuEdit);

    ui->horizontalLayoutTop->addWidget(menuBar);
    ui->tabWidget->addTab(infoDisplay, QIcon(":/img/serialport.png"), tr("&Display"));
    ui->tabWidget->addTab(costumTran, QIcon(":/img/log.png"), tr("&Export"));
    ui->tabWidget->addTab(ymodem, QIcon(":/img/update.png"), tr("&Update"));

    connect(optionsDlg, SIGNAL(accepted()), this, SLOT(dispOptionsChanged()));
    connect(optionsDlg, SIGNAL(updateSerialPort(QSerialPort *)), costumTran, SLOT(setPort(QSerialPort *)));
    connect(infoDisplay, SIGNAL(reportStatus(QString, bool, const QString &)), this, SLOT(statusChanged(QString, bool, const QString &)));
    connect(costumTran, SIGNAL(reportStatus(QString, bool, const QString &)), this, SLOT(statusChanged(QString, bool, const QString &)));
    connect(ymodem, SIGNAL(reportStatus(QString, bool, const QString &)), this, SLOT(statusChanged(QString, bool, const QString &)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showOptionsDialog()
{
    optionsDlg->listAvailiablePorts();
    optionsDlg->load();
    optionsDlg->setVisible(true);
}

void Widget::dispOptionsChanged()
{
    infoDisplay->setCoreNumDisp(pref->coreNum());
    infoDisplay->setTempRange(pref->tempLowerLimit(), pref->tempUpperLimit());
    infoDisplay->setVoltageRange(pref->voltLowerLimit(), pref->voltUpperLimit());
}

void Widget::statusChanged(QString atip, bool ok, const QString & module)
{
    QPalette palette;
    const QString placeHolder = "  ";

    palette.setColor(QPalette::WindowText, ok ? Qt::darkGreen : Qt::red);
    ui->labelTip->setText(placeHolder + atip);
    ui->labelTip->setPalette(palette);
    if(ok) {
        palette.setColor(QPalette::WindowText, Qt::darkCyan);
        ui->lineSeparator->setVisible(true);
        ui->labelActive->setText(module);
        ui->labelActive->setPalette(palette);
    } else {
        ui->lineSeparator->setVisible(false);
        ui->labelActive->clear();
    }
}
