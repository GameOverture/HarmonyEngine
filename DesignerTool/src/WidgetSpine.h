/**************************************************************************
 *	WidgetSpine.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPINE_H
#define WIDGETSPINE_H

#include <QWidget>

namespace Ui {
class WidgetSpine;
}

class WidgetSpine : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetSpine(QWidget *parent = 0);
    ~WidgetSpine();

private:
    Ui::WidgetSpine *ui;
};

#endif // WIDGETSPINE_H
