#include "WidgetGradientEditor.h"
#include "ui_WidgetGradientEditor.h"

WidgetGradientEditor::WidgetGradientEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetGradientEditor)
{
    ui->setupUi(this);
}

WidgetGradientEditor::~WidgetGradientEditor()
{
    delete ui;
}
