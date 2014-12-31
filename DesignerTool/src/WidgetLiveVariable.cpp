#include "WidgetLiveVariable.h"
#include "ui_WidgetLiveVariable.h"

WidgetLiveVariable::WidgetLiveVariable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetLiveVariable)
{
    ui->setupUi(this);
}

WidgetLiveVariable::~WidgetLiveVariable()
{
    delete ui;
}
