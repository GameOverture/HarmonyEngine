#include "EntSpriteFrame.h"
#include "ui_EntSpriteFrame.h"

EntSpriteFrame::EntSpriteFrame(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::EntSpriteFrame)
{
    ui->setupUi(this);
}

EntSpriteFrame::~EntSpriteFrame()
{
    delete ui;
}
