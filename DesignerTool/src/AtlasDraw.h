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
    
    struct TextureEnt : public HyEntity2d
    {
        QList<HyTexturedQuad2d *>   m_PreviewQuadList;
        
        TextureEnt(IHyNode *pParent) : HyEntity2d(pParent)
        { }
    };
    QList<TextureEnt *>         m_TextureEntList;

public:
    AtlasDraw(AtlasModel *pModelRef, IHyApplication *pHyApp);
    virtual ~AtlasDraw();

    virtual void OnShow(IHyApplication &hyApp);
    virtual void OnHide(IHyApplication &hyApp);
};

#endif // ATLASDRAW_H
