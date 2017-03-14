/**************************************************************************
 *	ItemSprite.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEDATA_H
#define SPRITEDATA_H

#include "IData.h"
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

class SpriteData : public IData
{
    Q_OBJECT

    HyPrimitive2d               m_primOriginHorz;
    HyPrimitive2d               m_primOriginVert;
    
public:
    SpriteData(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~SpriteData();
    
protected:
    virtual void OnGiveMenuActions(QMenu *pMenu);

    virtual void OnGuiLoad(IHyApplication &hyApp);
    virtual void OnGuiUnload(IHyApplication &hyApp);
    virtual void OnGuiShow(IHyApplication &hyApp);
    virtual void OnGuiHide(IHyApplication &hyApp);
    virtual void OnGuiUpdate(IHyApplication &hyApp);

    virtual void OnLink(AtlasFrame *pFrame);
    virtual void OnReLink(AtlasFrame *pFrame);
    virtual void OnUnlink(AtlasFrame *pFrame);
    
    virtual QJsonValue OnSave();
};

#endif // SPRITEDATA_H
