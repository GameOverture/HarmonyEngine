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
#include "Diagnostics/HyGuiComms.h"

#include "Scene/HyScene.h"
#include "Scene/Instances/HySprite2d.h"
#include "Scene/Instances/HySpine2d.h"
#include "Scene/Instances/HyPrimitive2d.h"
#include "Scene/Instances/HyText2d.h"
#include "Scene/Instances/HyTexturedQuad2d.h"
#include "Scene/HyPhysEntity2d.h"

#include "Assets/HyAssetManager.h"

class HyEngine
{
	friend class HyGuiRenderer;

	static HyEngine *		sm_pInstance;
	static HyMemoryHeap &	sm_Mem;

	// The order of these member declarations matter
	IHyApplication &		m_AppRef;
	
	HyGfxComms				m_GfxBuffer;
	HyScene					m_Scene;
	HyAssetManager			m_AssetManager;
	HyGuiComms				m_GuiComms;

	HyAudio					m_Audio;
	HyTimeInterop			m_Time;
	HyInputInterop			m_Input;
	HyRendererInterop		m_Renderer;

// If HY_PLATFORM_GUI, make this ctor public as GuiTool requires special usage.
#ifdef HY_PLATFORM_GUI
public:
#endif
	// Otherwise, private ctor invoked from RunGame(), once.
	HyEngine(IHyApplication &gameRef);

public:
	~HyEngine();

	void *operator new(size_t size);
	void operator delete (void *ptr);

	static void RunGame(IHyApplication &gameRef);

#ifndef HY_PLATFORM_GUI
private:
#endif
	bool Update();

	bool PollPlatformApi();
};

#endif /* __HyEngine_h__ */
