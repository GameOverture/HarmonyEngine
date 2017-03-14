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

IDraw::IDraw()
{

}

//void IDraw::Load(IHyApplication &hyApp)
//{
//    // A non NULL camera signifies that this has been loaded already
//    if(m_pCamera)
//        return;

//    m_pCamera = hyApp.Window().CreateCamera2d();
//    m_pCamera->SetEnabled(false);

//    OnGuiLoad(hyApp);
//    m_HyEntity.Load();
//}

//void IDraw::Unload(IHyApplication &hyApp)
//{
//    // A NULL camera signifies that this has hasn't been loaded
//    if(m_pCamera == NULL)
//        return;

//    hyApp.Window().RemoveCamera(m_pCamera);
//    m_pCamera = NULL;

//    OnGuiUnload(hyApp);
//    m_HyEntity.Load();
//}

//void IDraw::DrawShow(IHyApplication &hyApp)
//{
//    m_pCamera->SetEnabled(true);

//    OnGuiShow(hyApp);
//}

//void IDraw::DrawHide(IHyApplication &hyApp)
//{
//    m_pCamera->SetEnabled(false);

//    OnGuiHide(hyApp);
//}

//void IDraw::DrawUpdate(IHyApplication &hyApp)
//{
//    if(m_bReloadDraw || IsLoaded() == false)
//    {
//        m_bReloadDraw = false;

//        if(IsLoaded())
//            Unload(hyApp);

//        Load(hyApp);
//        DrawShow(hyApp);
//    }

//    OnGuiUpdate(hyApp);
//}

