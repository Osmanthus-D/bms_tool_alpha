#ifndef COREVOLGRP_H
#define COREVOLGRP_H

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "progresscolor.h"
#include <QDebug>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

#define ROWS            2
#define DEFAULT_VALUE   4618

class QDESIGNER_WIDGET_EXPORT CoreVolGrp : public QWidget
#else
class CoreVolGrp : public QWidget
#endif
{
    Q_OBJECT
    Q_PROPERTY(double minValue READ getMinValue WRITE setMinValue)
    Q_PROPERTY(double maxValue READ getMaxValue WRITE setMaxValue)
    Q_PROPERTY(double value1 READ getValue1 WRITE setValue1)
    Q_PROPERTY(double value2 READ getValue2 WRITE setValue2)
    Q_PROPERTY(double value3 READ getValue3 WRITE setValue3)
    Q_PROPERTY(double value4 READ getValue4 WRITE setValue4)
    Q_PROPERTY(double value5 READ getValue5 WRITE setValue5)
    Q_PROPERTY(double value6 READ getValue6 WRITE setValue6)
    Q_PROPERTY(double value7 READ getValue7 WRITE setValue7)
    Q_PROPERTY(double value8 READ getValue8 WRITE setValue8)
    Q_PROPERTY(double value9 READ getValue9 WRITE setValue9)
    Q_PROPERTY(double value10 READ getValue10 WRITE setValue10)
    Q_PROPERTY(QString label1 READ getlabel1 WRITE setLabel1)
    Q_PROPERTY(QString label2 READ getlabel2 WRITE setLabel2)
    Q_PROPERTY(QString label3 READ getlabel3 WRITE setLabel3)
    Q_PROPERTY(QString label4 READ getlabel4 WRITE setLabel4)
    Q_PROPERTY(QString label5 READ getlabel5 WRITE setLabel5)
    Q_PROPERTY(QString label6 READ getlabel6 WRITE setLabel6)
    Q_PROPERTY(QString label7 READ getlabel7 WRITE setLabel7)
    Q_PROPERTY(QString label8 READ getlabel8 WRITE setLabel8)
    Q_PROPERTY(QString label9 READ getlabel9 WRITE setLabel9)
    Q_PROPERTY(QString label10 READ getlabel10 WRITE setLabel10)
    Q_PROPERTY(int lineNumber READ getLineNumber WRITE setLineNumber)

public:
    CoreVolGrp(QWidget *parent = 0);
    QLabel *labelCore1;
    QLabel *labelCore2;
    QLabel *labelCore3;
    QLabel *labelCore4;
    QLabel *labelCore5;
    QLabel *labelCore6;
    QLabel *labelCore7;
    QLabel *labelCore8;
    QLabel *labelCore9;
    QLabel *labelCore10;
//    QGroupBox *grpBox;
    QVBoxLayout *vBoxLayout;
    QHBoxLayout *hBoxlayout1;
    QHBoxLayout *hBoxlayout2;
    QHBoxLayout *hBoxlayout3;
    QHBoxLayout *hBoxlayout4;
    QHBoxLayout *hBoxlayout5;

    ProgressColor *coreVolt1;
    ProgressColor *coreVolt2;
    ProgressColor *coreVolt3;
    ProgressColor *coreVolt4;
    ProgressColor *coreVolt5;
    ProgressColor *coreVolt6;
    ProgressColor *coreVolt7;
    ProgressColor *coreVolt8;
    ProgressColor *coreVolt9;
    ProgressColor *coreVolt10;

private:
    double minValue;                //��Сֵ
    double maxValue;                //���ֵ
    double value1;                   //Ŀ��ֵ
    double value2;
    double value3;
    double value4;
    double value5;
    double value6;
    double value7;
    double value8;
    double value9;
    double value10;
    int precision;                  //��ȷ��,С�����λ
    int comFontSize;
    int lineNumber;
    QString label1;
    QString label2;
    QString label3;
    QString label4;
    QString label5;
    QString label6;
    QString label7;
    QString label8;
    QString label9;
    QString label10;
    void initHLayout(QHBoxLayout *& hLayout, QLabel *& first, ProgressColor *& second, QLabel *& third, ProgressColor *& fourth);

public:
    double getMinValue()             const;
    double getMaxValue()             const;
    double getValue1()               const;
    double getValue2()               const;
    double getValue3()               const;
    double getValue4()               const;
    double getValue5()               const;
    double getValue6()               const;
    double getValue7()               const;
    double getValue8()               const;
    double getValue9()               const;
    double getValue10()              const;
    QString getlabel1()              const;
    QString getlabel2()              const;
    QString getlabel3()              const;
    QString getlabel4()              const;
    QString getlabel5()              const;
    QString getlabel6()              const;
    QString getlabel7()              const;
    QString getlabel8()              const;
    QString getlabel9()              const;
    QString getlabel10()             const;
    int getLineNumber()              const;

public Q_SLOTS:
    //���÷�Χֵ
    void setRange(double minValue, double maxValue);
    void setRange(int minValue, int maxValue);

    //���������Сֵ
    void setMinValue(double minValue);
    void setMaxValue(double maxValue);

    //����Ŀ��ֵ
    void setValue1(double value);
    void setValue1(int value);
    void setValue2(double value);
    void setValue2(int value);
    void setValue3(double value);
    void setValue3(int value);
    void setValue4(double value);
    void setValue4(int value);
    void setValue5(double value);
    void setValue5(int value);
    void setValue6(double value);
    void setValue6(int value);
    void setValue7(double value);
    void setValue7(int value);
    void setValue8(double value);
    void setValue8(int value);
    void setValue9(double value);
    void setValue9(int value);
    void setValue10(double value);
    void setValue10(int value);
    void setLabel1(QString label);
    void setLabel2(QString label);
    void setLabel3(QString label);
    void setLabel4(QString label);
    void setLabel5(QString label);
    void setLabel6(QString label);
    void setLabel7(QString label);
    void setLabel8(QString label);
    void setLabel9(QString label);
    void setLabel10(QString label);
    void setLineNumber(int cnt);

public slots:
    void setFontSize(int fontSize);
};

#endif
