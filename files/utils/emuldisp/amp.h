#ifndef AMP_H
#define AMP_H

#include "ui_amp.h"
#include <QMainWindow>

class Amp : public QMainWindow, private Ui::Amp
{
    Q_OBJECT

public:
    explicit Amp(QWidget *parent = nullptr);

    EmulDisp *getEmulDisp();
};

#endif // AMP_H
