#ifndef CORES_H
#define CORES_H

#include <QWidget>

namespace Ui {
class cores;
}

class cores : public QWidget
{
    Q_OBJECT

public:
    explicit cores(QWidget *parent = 0);
    ~cores();

private:
    Ui::cores *ui;
};

#endif // CORES_H
