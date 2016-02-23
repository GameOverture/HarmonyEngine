/**************************************************************************
 *	WidgetSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITE_H
#define WIDGETSPRITE_H

#include <QWidget>

namespace Ui {
class WidgetSprite;
}

class WidgetSprite : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetSprite(QWidget *parent = 0);
    ~WidgetSprite();
    
private:
    Ui::WidgetSprite *ui;
};

#endif // WIDGETSPRITE_H
