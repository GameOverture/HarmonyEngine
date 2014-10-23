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

void WidgetAtlas::on_btnTexSize256_clicked()
{
    ui->sbTextureWidth->setValue(256);
    ui->sbTextureHeight->setValue(256);
}

void WidgetAtlas::on_btnTexSize512_clicked()
{
    ui->sbTextureWidth->setValue(512);
    ui->sbTextureHeight->setValue(512);
}

void WidgetAtlas::on_btnTexSize1024_clicked()
{
    ui->sbTextureWidth->setValue(1024);
    ui->sbTextureHeight->setValue(1024);
}

void WidgetAtlas::on_btnTexSize2048_clicked()
{
    ui->sbTextureWidth->setValue(2048);
    ui->sbTextureHeight->setValue(2048);
}

void WidgetAtlas::on_btnAddFiles_clicked()
{
    
}

void WidgetAtlas::on_btnAddDir_clicked()
{
    
}

void WidgetAtlas::on_cmbHeuristic_currentIndexChanged(const QString &arg1)
{
    
}


void WidgetAtlas::on_cmbSortOrder_currentIndexChanged(const QString &arg1)
{
    
}

void WidgetAtlas::on_cmbRotationStrategy_currentIndexChanged(const QString &arg1)
{
    
}


void WidgetAtlas::on_sbFrameMarginTop_valueChanged(int arg1)
{
    
}

void WidgetAtlas::on_sbFrameMarginRight_valueChanged(int arg1)
{
    
}

void WidgetAtlas::on_sbFrameMarginBottom_valueChanged(int arg1)
{
    
}

void WidgetAtlas::on_sbFrameMarginLeft_valueChanged(int arg1)
{
    
}
