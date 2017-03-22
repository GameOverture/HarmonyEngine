/**************************************************************************
 *	AtlasDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AtlasDraw.h"

AtlasDraw::AtlasDraw(AtlasModel *pModelRef, IHyApplication *pHyApp) :   IDraw(*pHyApp),
                                                                        m_ModelRef(*pModelRef)
{
    m_ShadeBackground.SetAsQuad(100.0f, 100.0f, false);
    m_ShadeBackground.SetTint(0.0f, 0.0f, 0.0f);
    m_ShadeBackground.alpha.Set(0.3f);
    
    AddChild(m_ShadeBackground);
    //m_ModelRef.GetFrames();
}

/*virtual*/ AtlasDraw::~AtlasDraw()
{

}

/*virtual*/ void AtlasDraw::OnShow(IHyApplication &hyApp)
{
}

/*virtual*/ void AtlasDraw::OnHide(IHyApplication &hyApp)
{
}
