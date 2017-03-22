/**************************************************************************
 *	AtlasDraw.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASDRAW_H
#define ATLASDRAW_H

#include "IDraw.h"
#include "AtlasModel.h"

class AtlasDraw : public IDraw
{
    AtlasModel &                m_ModelRef;
    
    HyPrimitive2d               m_ShadeBackground;
    
    // TODO: Test whether splitting HyTexturedQuad's into multiple maps has any lookup/insert benefit, rather than one massive QMap
    struct TextureEnt : public HyEntity2d
    {
        QMap<quint32, HyTexturedQuad2d *>   m_TexQuadMap;
        
        TextureEnt(IHyNode *pParent) : HyEntity2d(pParent)
        { }
    };
    QList<TextureEnt *>             m_MasterList;
    
    QMap<quint32, HyTexturedQuad2d *>     m_CurrentPreviewMap;

public:
    AtlasDraw(AtlasModel *pModelRef, IHyApplication *pHyApp);
    virtual ~AtlasDraw();
    
    void SetSelected(QList<QTreeWidgetItem *> selectedList);

    virtual void OnShow(IHyApplication &hyApp);
    virtual void OnHide(IHyApplication &hyApp);
};

#endif // ATLASDRAW_H
