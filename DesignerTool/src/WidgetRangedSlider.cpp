/**************************************************************************
 *	WidgetRangedSlider.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetRangedSlider.h"
#include "ui_WidgetRangedSlider.h"

#include "HyGlobal.h"

WidgetRangedSlider::WidgetRangedSlider(QWidget *parent) :   QWidget(parent),
                                                            ui(new Ui::WidgetRangedSlider)
{
    ui->setupUi(this);
}

WidgetRangedSlider::~WidgetRangedSlider()
{
    delete ui;
}

QString WidgetRangedSlider::GetTitle()
{
    return ui->lblTitle->text();
}

void WidgetRangedSlider::SetTitle(QString sTitle)
{
    ui->lblTitle->setText(sTitle);
}

void WidgetRangedSlider::SetRange(int iMin, int iMax)
{
    ui->stackedSpinBoxes->setCurrentIndex(TYPE_INT);
    
    ui->minSlider->setRange(iMin, iMax);
    ui->maxSlider->setRange(iMin, iMax);
    
    ui->sbMin_Int->setRange(iMin, iMax);
    ui->sbMax_Int->setRange(iMin, iMax);
}

void WidgetRangedSlider::SetRange(double dMin, double dMax, int iNumDecimalPrecision)
{
    if(iNumDecimalPrecision > 6)
    {
        HyGuiLog("WidgetRangedSlider::SetRange cannot have a 'iNumDecimalPrecision' greater than 6", LOGTYPE_Error);
        return;
    }
    
    ui->stackedSpinBoxes->setCurrentIndex(TYPE_DOUBLE);
    
    ui->sbMin_Double->setRange(dMin, dMax);
    ui->sbMin_Double->setDecimals(iNumDecimalPrecision);
    ui->sbMax_Double->setRange(dMin, dMax);
    ui->sbMax_Double->setDecimals(iNumDecimalPrecision);
    
    // Convert double to int for sliders
    for(int i = 0; i < iNumDecimalPrecision; ++i)
    {
        dMin *= 10.0f;
        dMax *= 10.0f;
    }
    
    int iSliderMin = static_cast<int>(dMin);
    int iSliderMax = static_cast<int>(dMax);
    
    ui->minSlider->setRange(iSliderMin, iSliderMax);
    ui->maxSlider->setRange(iSliderMin, iSliderMax);
}

void WidgetRangedSlider::SetValue(int iMin, int iMax)
{
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_DOUBLE)
    {
        SetValue(static_cast<double>(iMin), static_cast<double>(iMax));
        return;
    }

    ui->minSlider->blockSignals(true);

}

void WidgetRangedSlider::SetValue(double dMin, double dMax)
{
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_INT)
    {
        SetValue(static_cast<int>(dMin), static_cast<int>(dMax));
        return;
    }
}

QVariant WidgetRangedSlider::GetMin()
{
}

QVariant WidgetRangedSlider::GetMax()
{
}

