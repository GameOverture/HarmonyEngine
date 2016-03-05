/**************************************************************************
 *	HyCamera.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Viewport/HyCamera.h"
#include "Renderer/Viewport/HyWindow.h"

#include "Utilities/Animation/ITransform.h"

IHyCamera::IHyCamera(HyWindow *pWindow) : m_pWindowPtr(pWindow)
{
	m_ViewportRect.left = m_ViewportRect.top = 0.0f;
	m_ViewportRect.right = m_ViewportRect.bottom = 1.0f;
}


IHyCamera::~IHyCamera()
{
}

// All values are [0.0 - 1.0] representing percentages of the entire game window
void IHyCamera::SetViewport(float fPosX, float fPosY, float fWidth, float fHeight)
{
	m_ViewportRect.left = fPosX;
	m_ViewportRect.top = fPosY;
	m_ViewportRect.right = m_ViewportRect.left + fWidth;
	m_ViewportRect.bottom = m_ViewportRect.top + fHeight;
}

HyCamera2d::HyCamera2d(HyWindow *pWindow) : IHyCamera(pWindow)
{ }
HyCamera2d::~HyCamera2d()
{ }

HyCamera3d::HyCamera3d(HyWindow *pWindow) : IHyCamera(pWindow)
{ }
HyCamera3d::~HyCamera3d()
{ }
