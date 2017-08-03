#include "EntityWidget.h"
#include "ui_EntityWidget.h"

EntityWidget::EntityWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EntityWidget)
{
    ui->setupUi(this);
}

EntityWidget::~EntityWidget()
{
    delete ui;
}
