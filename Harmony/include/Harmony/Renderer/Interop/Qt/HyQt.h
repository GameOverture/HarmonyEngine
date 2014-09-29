/**************************************************************************
 *	HyQt.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyQt_h__
#define __HyQt_h__

#include "Harmony/Afx/HyStdAfx.h"

#ifdef HY_PLATFORM_GUI

#include "Harmony/Renderer/Interop/OpenGL/HyOpenGL.h"

class HyQt : public HyOpenGL, public Fl_Gl_Window
{
	bool					m_bInitialized;

public:
	HyQt();
	HyQt(int x,int y,int w,int h,const char *l=0);
	virtual ~HyQt(void);

	virtual bool Initialize();
	virtual bool PollApi();
	virtual bool CheckDevice();

	// Override DrawBuffers() to do nothing. We only want to do IGfxApi::DrawBuffers()
	// when Fl_Gl_Window does its draw() callback. 
	virtual void DrawBuffers();
	virtual void draw();
};

#endif /* HY_PLATFORM_GUI */

#endif /* __HyQt_h__ */
