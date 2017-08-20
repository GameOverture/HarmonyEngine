#include "EntityToolbox.h"
#include "ui_EntityToolbox.h"

EntityToolbox::EntityToolbox(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::EntityToolbox)
{
    ui->setupUi(this);
}

EntityToolbox::~EntityToolbox()
{
    delete ui;
}
