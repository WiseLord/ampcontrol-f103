#include "amp.h"

Amp::Amp(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
}

EmulDisp *Amp::getEmulDisp()
{
    return static_cast<EmulDisp*>(this->centralWidget());
}
