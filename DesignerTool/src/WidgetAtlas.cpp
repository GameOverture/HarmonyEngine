#include "WidgetAtlas.h"
#include "ui_WidgetAtlas.h"

WidgetAtlas::WidgetAtlas(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAtlas)
{
    ui->setupUi(this);
}

WidgetAtlas::~WidgetAtlas()
{
    delete ui;
}
