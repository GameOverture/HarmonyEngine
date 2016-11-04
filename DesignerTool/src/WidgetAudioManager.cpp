#include "WidgetAudioManager.h"
#include "ui_WidgetAudioManager.h"

WidgetAudioManager::WidgetAudioManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetAudioManager)
{
    ui->setupUi(this);
}

WidgetAudioManager::~WidgetAudioManager()
{
    delete ui;
}
