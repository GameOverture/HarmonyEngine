/**************************************************************************
 *	HyRenderSurface.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyRenderSurface.h"

HyRenderSurface::HyRenderSurface(HyRenderSurfaceType eType, uint32 iID, int32 iRenderSurfaceWidth, int32 iRenderSurfaceHeight) :	m_eTYPE(eType),
																																	m_iID(iID),
																																	m_iRenderSurfaceWidth(iRenderSurfaceWidth),
																																	m_iRenderSurfaceHeight(iRenderSurfaceHeight),
																																	m_hData(nullptr)
{
}

HyRenderSurface::~HyRenderSurface()
{
}

HyRenderSurfaceType HyRenderSurface::GetType()
{
	return m_eTYPE;
}

int32 HyRenderSurface::GetId()
{
	return m_iID;
}

HyRenderSurfaceHandleInterop HyRenderSurface::GetHandle()
{
	return m_hData;
}

void HyRenderSurface::SetHandle(HyRenderSurfaceHandleInterop hHandle)
{
	m_hData = hHandle;
}

int32 HyRenderSurface::GetWidth()
{
	return m_iRenderSurfaceWidth;
}

int32 HyRenderSurface::GetHeight()
{
	return m_iRenderSurfaceHeight;
}

void HyRenderSurface::Resize(int32 iWidth, int32 iHeight)
{
	// Prevent A Divide By Zero
	if(iHeight == 0)
		iHeight = 1;

	m_iRenderSurfaceWidth = iWidth;
	m_iRenderSurfaceHeight = iHeight;
}
