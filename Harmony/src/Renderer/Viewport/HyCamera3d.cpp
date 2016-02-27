/**************************************************************************
 *	HyCamera3d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Viewport/HyCamera3d.h"
#include "Renderer/Viewport/HyWindow.h"

HyCamera3d::HyCamera3d(HyWindow *pViewport) :	ITransform<HyAnimVec3>(),
												m_kpViewportPtr(pViewport)
{
	m_ViewportRect.left = m_ViewportRect.top = 0.0f;
	m_ViewportRect.right = m_ViewportRect.bottom = 1.0f;
}

HyCamera3d::~HyCamera3d(void)
{
}

// All values are [0.0 - 1.0] representing percentages of the entire game window
void HyCamera3d::SetViewport(float fPosX, float fPosY, float fWidth, float fHeight)
{
	m_ViewportRect.left = fPosX;
	m_ViewportRect.top = fPosY;
	m_ViewportRect.right = m_ViewportRect.left + fWidth;
	m_ViewportRect.bottom = m_ViewportRect.bottom + fHeight;
}
