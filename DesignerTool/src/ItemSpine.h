/**************************************************************************
 *	ItemSpine.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMSPINE_H
#define ITEMSPINE_H

#include "ItemWidget.h"
#include "WidgetSpine.h"

class WidgetAtlasManager;

class ItemSpine : public ItemWidget
{
    Q_OBJECT

public:
    ItemSpine(ItemProject *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~ItemSpine();

protected:
    virtual void OnGiveMenuActions(QMenu *pMenu);

    virtual void OnGuiLoad(IHyApplication &hyApp);
    virtual void OnGuiUnload(IHyApplication &hyApp);

    virtual void OnGuiShow(IHyApplication &hyApp);
    virtual void OnGuiHide(IHyApplication &hyApp);
    virtual void OnGuiUpdate(IHyApplication &hyApp);

    virtual void OnLink(HyGuiFrame *pFrame);
    virtual void OnReLink(HyGuiFrame *pFrame);
    virtual void OnUnlink(HyGuiFrame *pFrame);

    virtual QJsonValue OnSave();
};

#endif // ITEMSPINE_H
