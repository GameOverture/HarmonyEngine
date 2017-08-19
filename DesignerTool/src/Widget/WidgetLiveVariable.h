/**************************************************************************
 *	WidgetLiveVariable.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETLIVEVARIABLE_H
#define WIDGETLIVEVARIABLE_H

#include <QWidget>

namespace Ui {
class WidgetLiveVariable;
}

class WidgetLiveVariable : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetLiveVariable(QWidget *parent = 0);
    ~WidgetLiveVariable();
    
private:
    Ui::WidgetLiveVariable *ui;
};

#endif // WIDGETLIVEVARIABLE_H
