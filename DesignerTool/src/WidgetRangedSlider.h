/**************************************************************************
 *	WidgetRangedSlider.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETRANGEDSLIDER_H
#define WIDGETRANGEDSLIDER_H

#include <QWidget>

namespace Ui {
class WidgetRangedSlider;
}

class WidgetRangedSlider : public QWidget
{
    Q_OBJECT
    
    enum eType
    {
        TYPE_INT = 0,
        TYPE_DOUBLE
    };
    
    int                         m_iPrevMin;
    int                         m_iPrevMax;
    double                      m_dPrevMin;
    double                      m_dPrevMax;
    
public:
    explicit WidgetRangedSlider(QWidget *parent = 0);
    ~WidgetRangedSlider();
    
    bool IsIntType();
    
    QString GetTitle();
    void SetTitle(QString sTitle);
    
    void SetRange(int iMin, int iMax);
    void SetRange(double dMin, double dMax, int iNumDecimalPrecision);
    
    void SetValue(int iMin, int iMax);
    void SetValue(double dMin, double dMax);
    
    QVariant GetMin();
    QVariant GetMax();
    
signals:
    void userChangedValue(QVariant &oldMin, QVariant &oldMax, QVariant &newMin, QVariant &newMax);
    
private slots:
    void on_sbMin_Double_editingFinished();
    
    void on_sbMax_Double_editingFinished();
    
    void on_sbMin_Int_editingFinished();
    
    void on_sbMax_Int_editingFinished();
    
    void on_maxSlider_sliderMoved(int position);
    
    void on_minSlider_sliderMoved(int position);
    
private:
    Ui::WidgetRangedSlider *ui;
    
    void OnDataCorrect();
};

#endif // WIDGETRANGEDSLIDER_H
