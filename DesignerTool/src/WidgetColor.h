/**************************************************************************
 *	WidgetColor.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETCOLOR_H
#define WIDGETCOLOR_H

#include <QWidget>

namespace Ui {
class WidgetColor;
}

class WidgetColor : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetColor(QWidget *parent = 0);
    ~WidgetColor();
    
    QColor GetColor();
    
private slots:
    void on_sbR_valueChanged(int arg1);
    
    void on_sbG_valueChanged(int arg1);
    
    void on_sbB_valueChanged(int arg1);
    
    void on_btnSample_clicked();
    
    void on_sliderR_valueChanged(int value);

    void on_sliderG_valueChanged(int value);

    void on_sliderB_valueChanged(int value);

private:
    Ui::WidgetColor *ui;
    
    void ApplyColorToSampleBtn();
};

#endif // WIDGETCOLOR_H
