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
#include "AtlasFrame.h"

IDraw::IDraw(IHyApplication &hyApp) :   m_HyAppRef(hyApp),
                                        m_pCamera(nullptr)
{
    m_pCamera = m_HyAppRef.Window().CreateCamera2d();
    m_pCamera->SetEnabled(false);
}

/*virtual*/ IDraw::~IDraw()
{
    m_HyAppRef.Window().RemoveCamera(m_pCamera);
}

void IDraw::ProjShow()
{
    m_pCamera->SetEnabled(true);
    OnProjShow(m_HyAppRef);
}

void IDraw::ProjHide()
{
    m_pCamera->SetEnabled(false);
    OnProjHide(m_HyAppRef);
}

void IDraw::ProjUpdate()
{
    OnProjUpdate(m_HyAppRef);
}

