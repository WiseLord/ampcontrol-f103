#ifndef EMULDISP_H
#define EMULDISP_H

#include <QWidget>
#include <QPainter>
#include <QMenu>
#include <QContextMenuEvent>

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

public slots:
    void menuSelected(QAction *action);

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

protected:
    virtual void paintEvent(QPaintEvent *pe);

    virtual void contextMenuEvent(QContextMenuEvent *pe);
};

#endif // EMULDISP_H
