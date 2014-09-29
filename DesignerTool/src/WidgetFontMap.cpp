#include "WidgetFontMap.h"
#include "ui_WidgetFontMap.h"

WidgetFontMap::WidgetFontMap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFontMap)
{
    ui->setupUi(this);
}

WidgetFontMap::~WidgetFontMap()
{
    delete ui;
}
