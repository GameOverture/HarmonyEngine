/**************************************************************************
 *	HyEngine.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEngine_h__
#define __HyEngine_h__

#include "Afx/HyStdAfx.h"
#include "IApplication.h"

#include "Memory/HyMemoryHeap.h"

#include "Input/HyInput.h"
#include "Renderer/HyRenderer.h"
#include "Creator/HyCreator.h"
#include "Audio/HyAudio.h"
#include "GuiTool/HyGuiComms.h"

#include "Creator/Instances/HySprite2d.h"
#include "Creator/Instances/HySpine2d.h"
#include "Creator/Instances/HyPrimitive2d.h"
#include "Creator/Instances/HyTexturedQuad2d.h"
#include "Creator/HyPhysEntity2d.h"

//#if defined(HY_PLATFORM_WINDOWS) || defined(HY_PLATFORM_OSX) || defined(HY_PLATFORM_LINUX)
//	#include "Renderer/Interop/OpenGL/HyOpenGL.h"
//#endif

#if defined(HY_PLATFORM_WINDOWS)
	#include "Time/Interop/HyTimeApi_Win.h"
#endif

class HyEngine
{
	static HyEngine *	sm_pInstance;
	static HyMemoryHeap &	sm_Mem;

	IApplication &			m_AppRef;
	
	HY_TIME_API				m_Time;
	HyInput					m_Input;
	HyRenderer				m_Renderer;
	HyCreator				m_Creator;
	HyAudio					m_Audio;

#ifndef HY_PLATFORM_GUI
	HyGuiComms				m_GuiComms;
#else
public:
#endif
	// Private ctor invoked from RunGame(), once.
	// If HY_PLATFORM_GUI, make this ctor public as GuiTool requires special usage.
	HyEngine(IApplication &gameRef);

public:
	~HyEngine();

	void *operator new(tMEMSIZE size);
	void operator delete (void *ptr);

	static void RunGame(IApplication &gameRef);

#ifndef HY_PLATFORM_GUI
private:
#endif
	void Initialize();
	bool Update();
};

#endif /* __HyEngine_h__ */
