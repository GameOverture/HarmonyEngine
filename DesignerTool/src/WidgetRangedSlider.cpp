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
    
    SetRange(0, 100);
    SetValue(40, 60);
}

WidgetRangedSlider::~WidgetRangedSlider()
{
    delete ui;
}

bool WidgetRangedSlider::IsIntType()
{
    return ui->stackedSpinBoxes->currentIndex() == TYPE_INT;
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
    
    OnDataCorrect();
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
    
    OnDataCorrect();
}

void WidgetRangedSlider::SetValue(int iMin, int iMax)
{
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_DOUBLE)
    {
        SetValue(static_cast<double>(iMin), static_cast<double>(iMax));
        return;
    }

    ui->sbMin_Int->setValue(iMin);
    ui->sbMax_Int->setValue(iMax);
    
    OnDataCorrect();
}

void WidgetRangedSlider::SetValue(double dMin, double dMax)
{
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_INT)
    {
        SetValue(static_cast<int>(dMin), static_cast<int>(dMax));
        return;
    }
    
    ui->sbMin_Double->setValue(dMin);
    ui->sbMax_Double->setValue(dMax);
    
    OnDataCorrect();
}

QVariant WidgetRangedSlider::GetMin()
{
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_INT)
        return QVariant(ui->sbMin_Int->value());
    else
        return QVariant(ui->sbMin_Double->value());
}

QVariant WidgetRangedSlider::GetMax()
{
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_INT)
        return QVariant(ui->sbMax_Int->value());
    else
        return QVariant(ui->sbMax_Double->value());
}

void WidgetRangedSlider::OnDataCorrect()
{
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_INT)
    {
        if(ui->sbMin_Int->value() > ui->sbMax_Int->value())
            ui->sbMin_Int->setValue(ui->sbMax_Int->value());
        else if(ui->sbMax_Int->value() < ui->sbMin_Int->value())
            ui->sbMax_Int->setValue(ui->sbMin_Int->value());

        ui->minSlider->setValue(ui->sbMin_Int->value());
        ui->maxSlider->setValue(ui->sbMax_Int->value());
        
        m_iPrevMin = ui->sbMin_Int->value();
        m_iPrevMax = ui->sbMax_Int->value();
    }
    else
    {
        if(ui->sbMin_Double->value() > ui->sbMax_Double->value())
            ui->sbMin_Double->setValue(ui->sbMax_Double->value());
        else if(ui->sbMax_Double->value() < ui->sbMin_Double->value())
            ui->sbMax_Double->setValue(ui->sbMin_Double->value());
        
        double dMinValue = ui->sbMin_Double->value();
        for(int i = 0; i < ui->sbMin_Double->decimals(); ++i)
            dMinValue *= 10.0f;
        
        double dMaxValue = ui->sbMax_Double->value();
        for(int i = 0; i < ui->sbMax_Double->decimals(); ++i)
            dMaxValue *= 10.0f;

        ui->minSlider->setValue(static_cast<int>(dMinValue));
        ui->maxSlider->setValue(static_cast<int>(dMaxValue));
        
        m_dPrevMin = ui->sbMin_Double->value();
        m_dPrevMax = ui->sbMax_Double->value();
    }
}

void WidgetRangedSlider::on_sbMin_Double_editingFinished()
{
    QVariant oldMin(m_dPrevMin);
    QVariant oldMax(m_dPrevMax);
    OnDataCorrect();
    QVariant newMin(ui->sbMin_Double->value());
    QVariant newMax(ui->sbMax_Double->value());
    
    emit userChangedValue(oldMin, oldMax, newMin, newMax);
}

void WidgetRangedSlider::on_sbMax_Double_editingFinished()
{    
    QVariant oldMin(m_dPrevMin);
    QVariant oldMax(m_dPrevMax);
    OnDataCorrect();
    QVariant newMin(ui->sbMin_Double->value());
    QVariant newMax(ui->sbMax_Double->value());
    
    emit userChangedValue(oldMin, oldMax, newMin, newMax);
}

void WidgetRangedSlider::on_sbMin_Int_editingFinished()
{    
    QVariant oldMin(m_iPrevMin);
    QVariant oldMax(m_iPrevMax);
    OnDataCorrect();
    QVariant newMin(ui->sbMin_Int->value());
    QVariant newMax(ui->sbMax_Int->value());
    
    emit userChangedValue(oldMin, oldMax, newMin, newMax);
}

void WidgetRangedSlider::on_sbMax_Int_editingFinished()
{
    QVariant oldMin(m_iPrevMin);
    QVariant oldMax(m_iPrevMax);
    OnDataCorrect();
    QVariant newMin(ui->sbMin_Int->value());
    QVariant newMax(ui->sbMax_Int->value());
    
    emit userChangedValue(oldMin, oldMax, newMin, newMax);
}

void WidgetRangedSlider::on_maxSlider_sliderMoved(int position)
{
    QVariant oldMin, oldMax, newMin, newMax;
    
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_INT)
    {
        oldMin = QVariant(m_iPrevMin);
        oldMax = QVariant(m_iPrevMax);
        
        ui->sbMax_Int->setValue(ui->maxSlider->value());
        OnDataCorrect();
        
        newMin = QVariant(ui->sbMin_Int->value());
        newMax = QVariant(ui->sbMax_Int->value());
    }
    else
    {
        oldMin = QVariant(m_dPrevMin);
        oldMax = QVariant(m_dPrevMax);
        
        double dValue = ui->maxSlider->value();
        for(int i = 0; i < ui->sbMax_Double->decimals(); ++i)
            dValue /= 10.0f;
        
        ui->sbMax_Double->setValue(dValue);
        OnDataCorrect();
        
        newMin = QVariant(ui->sbMin_Double->value());
        newMax = QVariant(ui->sbMax_Double->value());
    }
    
    emit userChangedValue(oldMin, oldMax, newMin, newMax);
}

void WidgetRangedSlider::on_minSlider_sliderMoved(int position)
{
    QVariant oldMin, oldMax, newMin, newMax;
    
    if(ui->stackedSpinBoxes->currentIndex() == TYPE_INT)
    {
        oldMin = QVariant(m_iPrevMin);
        oldMax = QVariant(m_iPrevMax);
        
        ui->sbMin_Int->setValue(ui->minSlider->value());
        OnDataCorrect();
        
        newMin = QVariant(ui->sbMin_Int->value());
        newMax = QVariant(ui->sbMax_Int->value());
    }
    else
    {
        oldMin = QVariant(m_dPrevMin);
        oldMax = QVariant(m_dPrevMax);
        
        double dValue = ui->minSlider->value();
        for(int i = 0; i < ui->sbMin_Double->decimals(); ++i)
            dValue /= 10.0f;
        
        ui->sbMin_Double->setValue(dValue);
        OnDataCorrect();
        
        newMin = QVariant(ui->sbMin_Double->value());
        newMax = QVariant(ui->sbMax_Double->value());
    }
    
    emit userChangedValue(oldMin, oldMax, newMin, newMax);
}
