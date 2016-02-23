/**************************************************************************
 *	IHyGuiDrawItem.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IHyGuiDrawItem.h"

IHyGuiDrawItem::IHyGuiDrawItem() :  m_bLoaded(false),
                                    m_pCamera(NULL)
{
}

void IHyGuiDrawItem::DrawOpen(IHyApplication &hyApp)
{
    if(m_bLoaded)
        return;
            
    if(m_pCamera == NULL)
        m_pCamera = hyApp.Window().CreateCamera2d();

    m_pCamera->SetEnabled(false);

    OnDraw_Open(hyApp);

    m_bLoaded = true;
}

void IHyGuiDrawItem::DrawClose(IHyApplication &hyApp)
{
    if(m_pCamera)
        hyApp.Window().RemoveCamera(m_pCamera);

    OnDraw_Close(hyApp);

    m_bLoaded = false;
}

void IHyGuiDrawItem::DrawShow(IHyApplication &hyApp)
{
    HyAssert(m_bLoaded, "IHyGuiDrawItem::Show() invoked when this item hasn't loaded (aka Open)");
    m_pCamera->SetEnabled(true);

    OnDraw_Show(hyApp);
}

void IHyGuiDrawItem::DrawHide(IHyApplication &hyApp)
{
    m_pCamera->SetEnabled(false);

    OnDraw_Hide(hyApp);
}
