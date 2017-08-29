#include "EntityCommon.h"
#include "ui_EntityCommon.h"

EntityCommon::EntityCommon(QWidget *parent) :
    QToolBox(parent),
    ui(new Ui::EntityCommon)
{
    ui->setupUi(this);
}

EntityCommon::~EntityCommon()
{
    delete ui;
}
