#include "WidgetAtlas.h"
#include "ui_WidgetAtlas.h"

#include <QTreeWidget>

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
    
    m_bDirty = true;
}

void WidgetAtlas::on_btnTexSize512_clicked()
{
    ui->sbTextureWidth->setValue(512);
    ui->sbTextureHeight->setValue(512);
    
    m_bDirty = true;
}

void WidgetAtlas::on_btnTexSize1024_clicked()
{
    ui->sbTextureWidth->setValue(1024);
    ui->sbTextureHeight->setValue(1024);
    
    m_bDirty = true;
}

void WidgetAtlas::on_btnTexSize2048_clicked()
{
    ui->sbTextureWidth->setValue(2048);
    ui->sbTextureHeight->setValue(2048);
    
    m_bDirty = true;
}

void WidgetAtlas::on_btnAddFiles_clicked()
{
    
}

void WidgetAtlas::on_btnAddDir_clicked()
{
    
}

void WidgetAtlas::on_cmbHeuristic_currentIndexChanged(const QString &arg1)
{
    m_bDirty = true;
}


void WidgetAtlas::on_cmbSortOrder_currentIndexChanged(const QString &arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_cmbRotationStrategy_currentIndexChanged(const QString &arg1)
{
    m_bDirty = true;
}


void WidgetAtlas::on_sbFrameMarginTop_valueChanged(int arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_sbFrameMarginRight_valueChanged(int arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_sbFrameMarginBottom_valueChanged(int arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_sbFrameMarginLeft_valueChanged(int arg1)
{
    m_bDirty = true;
}

void WidgetAtlas::on_tabWidget_currentChanged(int index)
{
    if(m_bDirty)
    {
        ui->frameList->topLevelItemCount();
    }
}
