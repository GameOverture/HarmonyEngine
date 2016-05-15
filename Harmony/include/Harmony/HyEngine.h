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

#include "Renderer/Viewport/HyWindow.h"

#include "Scene/HyScene.h"
#include "Scene/Instances/HySprite2d.h"
#include "Scene/Instances/HySpine2d.h"
#include "Scene/Instances/HyPrimitive2d.h"
#include "Scene/Instances/HyText2d.h"
#include "Scene/Instances/HyTexturedQuad2d.h"
#include "Scene/HyPhysEntity2d.h"

#include "Assets/HyAssetManager.h"

#include "Utilities/HyMath.h"
#include "Utilities/HyFileIO.h"

class HyEngine
{
	friend class HyGuiRenderer;

	static HyEngine *		sm_pInstance;

	// The order of these member declarations matter
	IHyApplication &		m_AppRef;
	
	HyGfxComms				m_GfxBuffer;
	HyScene					m_Scene;
	HyAssetManager 			m_AssetManager;
	HyGuiComms				m_GuiComms;

	HyAudio					m_Audio;
	HyTimeInterop			m_Time;
	HyInputInterop			m_Input;
	HyRendererInterop		m_Renderer;

// If HY_PLATFORM_GUI, make this ctor public as gui tool requires special usage.
#ifdef HY_PLATFORM_GUI
public:
#endif
	// Otherwise, private ctor invoked from RunGame(), once.
	HyEngine(IHyApplication &gameRef);

public:
	~HyEngine();

	static void RunGame(IHyApplication &gameRef);

#ifndef HY_PLATFORM_GUI
private:
#endif
	bool Update();

	bool PollPlatformApi();

	void Shutdown();
};

#endif /* __HyEngine_h__ */
