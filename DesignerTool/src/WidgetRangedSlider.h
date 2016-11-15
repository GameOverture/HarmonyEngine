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
    
public:
    explicit WidgetRangedSlider(QWidget *parent = 0);
    ~WidgetRangedSlider();
    
    QString GetTitle();
    void SetTitle(QString sTitle);
    
    void SetRange(int iMin, int iMax);
    void SetRange(double dMin, double dMax, int iNumDecimalPrecision);
    
    void SetValue(int iMin, int iMax);
    void SetValue(double dMin, double dMax);
    
    QVariant GetMin();
    QVariant GetMax();
    
private:
    Ui::WidgetRangedSlider *ui;
};

#endif // WIDGETRANGEDSLIDER_H
