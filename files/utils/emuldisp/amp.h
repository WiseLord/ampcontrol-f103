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

    uint8_t bus;

private:
    int dialValue;
    int dialMax;

    void dialChanged(int value);

    QTimer *dialTimer;

private slots:
    void on_btn0_pressed();
    void on_btn0_released();
    void on_btn1_pressed();
    void on_btn1_released();
    void on_btn2_pressed();
    void on_btn2_released();
    void on_btn3_pressed();
    void on_btn3_released();
    void on_btn4_pressed();
    void on_btn4_released();
    void on_btn5_pressed();
    void on_btn5_released();
    void on_dial_valueChanged(int value);
    void on_dial_sliderMoved(int position);

    void dialTimerElapsed();
};

#endif // AMP_H
