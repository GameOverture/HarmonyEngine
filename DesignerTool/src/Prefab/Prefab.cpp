#include "Prefab.h"
#include "ui_Prefab.h"

Prefab::Prefab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Prefab)
{
    ui->setupUi(this);
}

Prefab::~Prefab()
{
    delete ui;
}
