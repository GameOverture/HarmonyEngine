/**************************************************************************
 *	IDraw.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IDraw.h"

IDraw::IDraw(ProjectItem &projItemRef, IHyApplication &hyApp) :   m_HyAppRef(hyApp),
                                        m_pCamera(nullptr)
{
    m_pCamera = m_HyAppRef.Window().CreateCamera2d();
    m_pCamera->SetEnabled(false);
}

/*virtual*/ IDraw::~IDraw()
{
    m_HyAppRef.Window().RemoveCamera(m_pCamera);
}

void IDraw::Show()
{
    m_pCamera->SetEnabled(true);
    OnShow(m_HyAppRef);
}

void IDraw::Hide()
{
    m_pCamera->SetEnabled(false);
    OnHide(m_HyAppRef);
}

