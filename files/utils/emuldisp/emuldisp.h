#ifndef EMULDISP_H
#define EMULDISP_H

#include <QWidget>
#include <QPainter>

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

protected:
    virtual void paintEvent(QPaintEvent *pe);

};

#endif // EMULDISP_H
