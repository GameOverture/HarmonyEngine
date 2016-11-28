/**************************************************************************
 *	ItemAudio.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMAUDIO_H
#define ITEMAUDIO_H

#include "ItemWidget.h"

class ItemAudio : public ItemWidget
{
    Q_OBJECT
    
public:
    ItemAudio(const QString sPath, WidgetAtlasManager &atlasManRef, WidgetAudioManager &audioManRef);
    virtual ~ItemAudio();
    
    virtual QList<QAction *> GetActionsForToolBar();
    
protected:
    virtual void OnLoad(IHyApplication &hyApp);
    virtual void OnUnload(IHyApplication &hyApp);

    virtual void OnDraw_Show(IHyApplication &hyApp);
    virtual void OnDraw_Hide(IHyApplication &hyApp);
    virtual void OnDraw_Update(IHyApplication &hyApp);

    virtual void OnLink(HyGuiFrame *pFrame);
    virtual void OnReLink(HyGuiFrame *pFrame);
    virtual void OnUnlink(HyGuiFrame *pFrame);
    
    virtual void OnSave();
};

#endif // ITEMAUDIO_H
