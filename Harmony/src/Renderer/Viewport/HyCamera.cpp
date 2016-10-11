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

IHyCamera::IHyCamera(HyWindow *pWindow) : m_pWindowPtr(pWindow)
{
	m_ViewportRect.left = m_ViewportRect.bottom = 0.0f;
	m_ViewportRect.right = m_ViewportRect.top = 1.0f;
}


IHyCamera::~IHyCamera()
{
}

// All values are [0.0 - 1.0] representing percentages of the entire game window
void IHyCamera::SetViewport(float fPosX, float fPosY, float fWidth, float fHeight)
{
	m_ViewportRect.left = fPosX;
	m_ViewportRect.bottom = fPosY;
	m_ViewportRect.right = m_ViewportRect.left + fWidth;
	m_ViewportRect.top = m_ViewportRect.bottom + fHeight;
}

HyCamera2d::HyCamera2d(HyWindow *pWindow) : IHyCamera(pWindow)
{ }
HyCamera2d::~HyCamera2d()
{ }

/*virtual*/ void HyCamera2d::OnUpdate()
{
}

HyCamera3d::HyCamera3d(HyWindow *pWindow) : IHyCamera(pWindow)
{ }
HyCamera3d::~HyCamera3d()
{ }

/*virtual*/ void HyCamera3d::OnUpdate()
{
}
