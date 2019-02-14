#include "Global.h"
#include "Entity3d.h"
#include "ui_Entity3d.h"

Entity3d::Entity3d(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Entity3d)
{
	ui->setupUi(this);
}

Entity3d::~Entity3d()
{
	delete ui;
}
