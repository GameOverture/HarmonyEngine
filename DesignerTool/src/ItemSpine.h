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
    ItemSpine(const QString sPrefix, const QString sName, QJsonValue initVal, WidgetAtlasManager &atlasManRef, WidgetAudioManager &audioManRef);
    virtual ~ItemSpine();

protected:
    virtual void OnGiveMenuActions(QMenu *pMenu);

    virtual void OnLoad(IHyApplication &hyApp);
    virtual void OnUnload(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);

    virtual void OnLink(HyGuiFrame *pFrame);
    virtual void OnReLink(HyGuiFrame *pFrame);
    virtual void OnUnlink(HyGuiFrame *pFrame);

    virtual QJsonValue OnSave();
};

#endif // ITEMSPINE_H
