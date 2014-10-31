#include "WidgetSprite.h"
#include "ui_WidgetSprite.h"

WidgetSprite::WidgetSprite(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetSprite)
{
    ui->setupUi(this);
}

WidgetSprite::~WidgetSprite()
{
    delete ui;
}
