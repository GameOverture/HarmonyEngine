#include "DlgColorPicker.h"
#include "ui_DlgColorPicker.h"

DlgColorPicker::DlgColorPicker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgColorPicker)
{
    ui->setupUi(this);
}

DlgColorPicker::~DlgColorPicker()
{
    delete ui;
}
