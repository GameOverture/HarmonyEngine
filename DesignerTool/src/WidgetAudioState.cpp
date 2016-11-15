#include "WidgetAudioState.h"
#include "ui_WidgetAudioState.h"

WidgetAudioState::WidgetAudioState(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAudioState)
{
    ui->setupUi(this);
}

WidgetAudioState::~WidgetAudioState()
{
    delete ui;
}
