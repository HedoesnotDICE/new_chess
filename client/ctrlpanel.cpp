#include "CtrlPanel.h"
#include <QGridLayout>
#include <QVBoxLayout>
extern void newGame();
CtrlPanel::CtrlPanel(QWidget *parent) : QWidget(parent)
{
    QGridLayout* grid = new QGridLayout(this);
    QVBoxLayout* vLay = new QVBoxLayout;
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(2, 1);
    grid->setRowStretch(0, 1);
    grid->setRowStretch(2, 1);
    grid->addLayout(vLay, 1, 1);

    vLay->addWidget(this->_back = new QPushButton("Back"));
    connect(this->_back, SIGNAL(clicked()), this, SLOT(slotBack()));
}

CtrlPanel::~CtrlPanel()
{

}

void CtrlPanel::slotBack()
{
    newGame();
}

