/**************************************************************************
 *	HyOpenGL_Desktop.h
 *
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyOpenGL_Desktop_h__
#define HyOpenGL_Desktop_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/OpenGL/HyOpenGL.h"

class HyOpenGL_Desktop : public HyOpenGL
{
public:
	HyOpenGL_Desktop(HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef);
	~HyOpenGL_Desktop();

	virtual bool Initialize() override;
	virtual void StartRender() override;
	virtual void FinishRender() override;
};

#endif /* HyOpenGL_Desktop_h__ */
