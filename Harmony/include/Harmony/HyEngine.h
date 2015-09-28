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
#include "Afx/HyInteropAfx.h"

#include "IHyApplication.h"

#include "Memory/HyMemoryHeap.h"

#include "Audio/HyAudio.h"
#include "GuiTool/HyGuiComms.h"

#include "Creator/HyCreator.h"
#include "Creator/Instances/HySprite2d.h"
#include "Creator/Instances/HySpine2d.h"
#include "Creator/Instances/HyPrimitive2d.h"
#include "Creator/Instances/HyText2d.h"
#include "Creator/Instances/HyTexturedQuad2d.h"
#include "Creator/HyPhysEntity2d.h"

class HyEngine
{
	static HyEngine *		sm_pInstance;
	static HyMemoryHeap &	sm_Mem;

	IHyApplication &		m_AppRef;
	
	HyTimeInterop			m_Time;
	HyInputInterop			m_Input;
	HyRendererInterop		m_Renderer;
	HyAudio					m_Audio;

	HyGfxComms				m_GfxBuffer;
	HyCreator				m_Creator;

#ifndef HY_PLATFORM_GUI
	HyGuiComms				m_GuiComms;
#else
public:
#endif
	// Private ctor invoked from RunGame(), once.
	// If HY_PLATFORM_GUI, make this ctor public as GuiTool requires special usage.
	HyEngine(IHyApplication &gameRef);

public:
	~HyEngine();

	void *operator new(tMEMSIZE size);
	void operator delete (void *ptr);

	static void RunGame(IHyApplication &gameRef);

#ifndef HY_PLATFORM_GUI
private:
#endif
	HyRendererInterop &GetRenderer()	{ return m_Renderer; }

	bool Update();
};

#endif /* __HyEngine_h__ */
