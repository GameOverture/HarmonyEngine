/**************************************************************************
 *	HyCamera2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Viewport/HyCamera2d.h"
#include "Renderer/Viewport/HyWindow.h"

HyCamera2d::HyCamera2d(HyWindow *pViewport) :	ITransform<HyAnimVec2>(),
												m_pViewportPtr(pViewport)
{
	m_ViewportRect.left = m_ViewportRect.top = 0.0f;
	m_ViewportRect.right = m_ViewportRect.bottom = 1.0f;
}

HyCamera2d::~HyCamera2d(void)
{
}

// All values are [0.0 - 1.0] representing percentages of the entire game window
void HyCamera2d::SetViewport(float fPosX, float fPosY, float fWidth, float fHeight)
{
	m_ViewportRect.left = fPosX;
	m_ViewportRect.top = fPosY;
	m_ViewportRect.right = m_ViewportRect.left + fWidth;
	m_ViewportRect.bottom = m_ViewportRect.bottom + fHeight;
}
