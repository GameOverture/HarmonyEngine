#include "WidgetAudioBank.h"
#include "ui_WidgetAudioBank.h"

WidgetAudioBank::WidgetAudioBank(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAudioBank)
{
    ui->setupUi(this);
}

WidgetAudioBank::~WidgetAudioBank()
{
    delete ui;
}
