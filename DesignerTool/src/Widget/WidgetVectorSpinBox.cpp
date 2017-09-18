#include "WidgetVectorSpinBox.h"
#include "ui_WidgetVectorSpinBox.h"

WidgetVectorSpinBox::WidgetVectorSpinBox(QWidget *parent) : QWidget(parent),
                                                            ui(new Ui::WidgetVectorSpinBox)
{
    ui->setupUi(this);
}

/*virtual*/ WidgetVectorSpinBox::~WidgetVectorSpinBox()
{
    delete ui;
}

void WidgetVectorSpinBox::SetAsInt(bool bEnable)
{
    ui->stackedWidget->setCurrentIndex(bEnable ? 0 : 1);
}
