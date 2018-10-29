#ifndef EMULDISP_H
#define EMULDISP_H

#include <QWidget>
#include <QPainter>

#include "../../../display/glcd.h"
#include "../../../canvas/canvas.h"

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

    void setSize(uint16_t w, uint16_t h);

    void drawScreen(void);

private:
    Ui::EmulDisp *ui;

    QPainter *painter;

    Glcd *glcd;
    Canvas *canvas;

protected:
    virtual void paintEvent(QPaintEvent *pe);

};

#endif // EMULDISP_H
