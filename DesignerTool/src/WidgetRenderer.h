/**************************************************************************
 *	WidgetRenderer.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETRENDERER_H
#define WIDGETRENDERER_H

#include "HyGlobal.h"

#include <QWidget>
#include <QQueue>

#include "Item.h"
#include "ItemProject.h"

namespace Ui {
class WidgetRenderer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetRenderer : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetRenderer(QWidget *parent = 0);
    ~WidgetRenderer();
    
    HyGuiRenderer *GetRenderer();

    void OpenItem(Item *pItem);
    void CloseItem(Item *pItem);
    
    QStringList GetOpenItemPaths();


private:
    Ui::WidgetRenderer *ui;
};

#endif // WIDGETRENDERER_H
