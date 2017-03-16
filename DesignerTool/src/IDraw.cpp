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

IDraw::IDraw() :    m_pCamera(nullptr),
                    m_bReloadDraw(false)
{

}

void IDraw::ProjLoad(IHyApplication &hyApp)
{
    // A non NULL camera signifies that this has been loaded already
    if(m_pCamera)
        return;

    m_pCamera = hyApp.Window().CreateCamera2d();
    m_pCamera->SetEnabled(false);

    OnPreLoad(hyApp);
    SetEnabled(false);
    Load();
}

void IDraw::ProjUnload(IHyApplication &hyApp)
{
    // A NULL camera signifies that this has hasn't been loaded
    if(m_pCamera == NULL)
        return;

    hyApp.Window().RemoveCamera(m_pCamera);
    m_pCamera = NULL;

    Unload();
    OnPostUnload(hyApp);
}

void IDraw::ProjShow(IHyApplication &hyApp)
{
    m_pCamera->SetEnabled(true);
    OnProjShow(hyApp);
}

void IDraw::ProjHide(IHyApplication &hyApp)
{
    m_pCamera->SetEnabled(false);
    OnProjHide(hyApp);
}

void IDraw::ProjUpdate(IHyApplication &hyApp)
{
    if(m_bReloadDraw || IsLoaded() == false)
    {
        m_bReloadDraw = false;

        if(IsLoaded())
            ProjUnload(hyApp);

        ProjLoad(hyApp);
        ProjShow(hyApp);
    }

    OnProjUpdate(hyApp);
}

