/**************************************************************************
 *	HyCamera2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Viewport/HyCamera2d.h"
#include "Creator/Viewport/HyViewport.h"

HyCamera2d::HyCamera2d(HyViewport *pViewport) :	ITransform<HyAnimVec2>(),
												m_pViewportPtr(pViewport),
												m_bEnabled(true)
{
	m_RenderRect.x = m_RenderRect.y = 0.0f;
	m_RenderRect.width = m_RenderRect.height = 1.0f;
}

HyCamera2d::~HyCamera2d(void)
{
}

// All values are [0.0 - 1.0] representing percentages of the entire game window
void HyCamera2d::SetRenderPercentageCoordinates(float fPosX, float fPosY, float fWidth, float fHeight)
{
	m_RenderRect.x = fPosX;
	m_RenderRect.y = fPosY;
	m_RenderRect.width = fWidth;
	m_RenderRect.height = fHeight;
}
