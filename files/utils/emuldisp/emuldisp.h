#ifndef EMULDISP_H
#define EMULDISP_H

#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTimer>

#include "../../../actions.h"

namespace Ui {
class EmulDisp;
}

class EmulDisp : public QWidget
{
    Q_OBJECT

public:
    explicit EmulDisp(QWidget *parent = nullptr);
    ~EmulDisp() {}

    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void setSize(uint16_t w, uint16_t h);

    void setBrightness(const int8_t &value);

public slots:
    void menuSelected(QAction *action);

private slots:
    void systick();

private:
    Ui::EmulDisp *ui;
    QPainter *painter;

    QMenu *cMenu;
    QAction *actScreenTime;
    QAction *actScreenSpectrum;
    QAction *actScreenBrightness;
    QAction *actScreenInput;
    QAction *actScreenAudio;
    QAction *actScreenTuner;
    QAction *actScreenMenu;

    QTimer *updateTimer;
    QTimer *mSecTimer;

    Action action;
    int8_t brightness;

protected:
    virtual void paintEvent(QPaintEvent *pe);

    virtual void contextMenuEvent(QContextMenuEvent *pe);
};

#endif // EMULDISP_H
