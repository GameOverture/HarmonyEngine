/**************************************************************************
*	HyRenderSurface.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyRenderSurface_h__
#define __HyRenderSurface_h__

#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"

enum HyRenderSurfaceType
{
	HYRENDERSURFACE_Texture = 0,
	HYRENDERSURFACE_Window
};

class HyRenderSurface
{
	const HyRenderSurfaceType			m_eTYPE;
	const int32							m_iID;
	int32								m_iRenderSurfaceWidth;
	int32								m_iRenderSurfaceHeight;

	HyRenderSurfaceHandleInterop		m_hData;

public:
	HyRenderSurface(HyRenderSurfaceType eType, uint32 iID, int32 iRenderSurfaceWidth, int32 iRenderSurfaceHeight);
	~HyRenderSurface();

	HyRenderSurfaceType GetType();
	int32 GetId();

	HyRenderSurfaceHandleInterop GetHandle();
	void SetHandle(HyRenderSurfaceHandleInterop hHandle);

	int32 GetWidth();
	int32 GetHeight();
	void Resize(int32 iWidth, int32 iHeight);
};

#endif /* __HyRenderSurface_h__ */
