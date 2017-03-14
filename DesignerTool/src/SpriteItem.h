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

class AtlasesWidget;

class SpriteFrame
{
public:
    AtlasFrame *            m_pFrame;
    int                     m_iRowIndex;
    
    QPoint                  m_vOffset;
    float                   m_fDuration;
    
    SpriteFrame(AtlasFrame *pFrame, int iRowIndex) :    m_pFrame(pFrame),
                                                        m_iRowIndex(iRowIndex),
                                                        m_vOffset(0, 0),
                                                        m_fDuration(0.016f)
    { }

    QPoint GetRenderOffset()
    {
        QPoint ptRenderOffset;
        
        ptRenderOffset.setX(m_vOffset.x() + m_pFrame->GetCrop().left());
        ptRenderOffset.setY(m_vOffset.y() + (m_pFrame->GetSize().height() - m_pFrame->GetCrop().bottom()));

        return ptRenderOffset;
    }
};

class SpriteItem : public IProjItem
{
    Q_OBJECT
    
public:
    SpriteItem(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~SpriteItem();
    
protected:
    virtual void OnGiveMenuActions(QMenu *pMenu) override;

    virtual void OnLink(AtlasFrame *pFrame) override;
    virtual void OnReLink(AtlasFrame *pFrame) override;
    virtual void OnUnlink(AtlasFrame *pFrame) override;
    
    virtual QJsonValue OnSave() override;
};

#endif // SPRITEITEM_H
