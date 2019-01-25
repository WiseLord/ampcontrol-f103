#ifndef EMULDISP_H
#define EMULDISP_H

#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTimer>

#include "../../../actions.h"
#include "../../../display/fonts/fonts.h"

#define EMUL_DISP_BORDER    5
#define EMUL_DISP_WIDTH     480
#define EMUL_DISP_HEIGHT    320
#define EMUL_DISP_SCALE     2

namespace Ui {
class EmulDisp;
}

class EmulDisp : public QWidget
{
    Q_OBJECT

public:
    explicit EmulDisp(QWidget *parent = nullptr);
    ~EmulDisp() override {}

    void init();
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawRectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawImage(tImage *img, int16_t x, int16_t y, uint16_t color, uint16_t bgColor);

    void setSize(uint16_t w, uint16_t h);

    void setBrightness(const int8_t &value);

private slots:
    void systick();
    void drawscreen();

private:
    Ui::EmulDisp *ui;
    QImage image;

    QTimer *updateTimer;
    QTimer *mSecTimer;

    Action action;
    int8_t brightness;

    void resizeImage(QImage *image, const QSize &newSize);

protected:
    virtual void paintEvent(QPaintEvent *pe) override;
    void resizeEvent(QResizeEvent *event) override;
};

#endif // EMULDISP_H
