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

#include "IData.h"

class ItemAudio : public ItemWidget
{
    Q_OBJECT
    
public:
    ItemAudio(ItemProject *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~ItemAudio();
    
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

#endif // ITEMAUDIO_H
