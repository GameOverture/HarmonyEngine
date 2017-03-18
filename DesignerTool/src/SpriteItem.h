/**************************************************************************
 *	ItemSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEITEM_H
#define SPRITEITEM_H

#include "IProjItem.h"
#include "SpriteWidget.h"
#include "SpriteModels.h"

class AtlasesWidget;

class SpriteItem : public IProjItem
{
    Q_OBJECT

    SpriteStatesModel *      m_pStatesModel;
    
public:
    SpriteItem(Project *pItemProj, const QString sPrefix, const QString sName, QJsonArray stateArray);
    virtual ~SpriteItem();

    SpriteStatesModel *GetSpritesModel();
    
protected:
    virtual void OnGiveMenuActions(QMenu *pMenu) override;

    virtual void OnLink(AtlasFrame *pFrame) override;
    virtual void OnUnlink(AtlasFrame *pFrame) override;
    
    virtual QJsonValue OnSave() override;
};

#endif // SPRITEITEM_H
