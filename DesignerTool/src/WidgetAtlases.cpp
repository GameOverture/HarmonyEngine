#include "WidgetAtlases.h"
#include "ui_WidgetAtlases.h"

WidgetAtlases::WidgetAtlases(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAtlases)
{
    ui->setupUi(this);
}

WidgetAtlases::~WidgetAtlases()
{
    delete ui;
}
