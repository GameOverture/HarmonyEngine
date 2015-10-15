#include "DlgAtlasGroupSettings.h"
#include "ui_DlgAtlasGroupSettings.h"

DlgAtlasGroupSettings::DlgAtlasGroupSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAtlasGroupSettings)
{
    ui->setupUi(this);
}

DlgAtlasGroupSettings::~DlgAtlasGroupSettings()
{
    delete ui;
}
