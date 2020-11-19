/**************************************************************************
 *	HyEngine.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyEngine_h__
#define HyEngine_h__

#include "Afx/HyInteropAfx.h"
#include "Assets/HyAssets.h"
#include "Audio/HyAudioHarness.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Diagnostics/GuiComms/HyGuiComms.h"
#include "Input/HyInput.h"
#include "Renderer/Effects/HyStencil.h"
#include "Networking/HyNetworking.h"
#include "Networking/IHyNetworkClass.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrimitive2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyPrefab3d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyTexturedQuad2d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntityLeaf2d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.h"
#include "Threading/IHyThreadClass.h"
#include "Threading/IHyThreadClass.h"
#include "Time/HyTime.h"
#include "UI/IHy9Slice.h"
#include "UI/HyInfoPanel.h"
#include "UI/HyButton.h"
#include "UI/HyMeter.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyImage.h"
#include "Utilities/HyIO.h"
#include "Utilities/HyRand.h"
#include "Window/HyWindowManager.h"

class HyEngine
{
	static HyEngine *			sm_pInstance;

	// The order of these member declarations matter as some are used to initialize each other
	const HarmonyInit			m_Init;

	HyConsoleInterop			m_Console;
	HyWindowManager				m_WindowManager;
	HyNetworking				m_Networking;
	HyAudioHarness				m_Audio;
	HyScene						m_Scene;
	HyAssets 					m_Assets;
	HyGuiComms					m_GuiComms;
	HyTime						m_Time;
	HyDiagnostics				m_Diagnostics;
	HyInput						m_Input;
	HyRendererInterop			m_Renderer;

#ifdef HY_USE_SDL2
	SDL_Event					m_SdlEvent;
#endif

public:
	HyEngine(const HarmonyInit &initStruct);
	~HyEngine();

	HyRendererInterop &GetRenderer();

	int32 RunGame();

protected:
	// Derived game class overrides
	virtual bool OnUpdate() { return true; }

#if defined(HY_PLATFORM_GUI) || defined(HY_PLATFORM_BROWSER)
public:
#else
private:
#endif
	bool Update();
	bool PollPlatformApi();

	friend bool					Hy_IsInitialized();
	friend const HarmonyInit &	Hy_InitValues();
	friend float				Hy_UpdateStep();
	friend double				Hy_UpdateStepDbl();
	friend void					Hy_PauseGame(bool bPause);
	friend HyWindow &			Hy_Window();
	friend HyWindow &			Hy_Window(uint32 uiWindowIndex);
	friend HyInput &			Hy_Input();
	friend HyDiagnostics &		Hy_Diagnostics();
	friend HyShaderHandle		Hy_DefaultShaderHandle(HyType eType);
	friend std::string			Hy_DateTime();
	friend std::string			Hy_DataDir();
};

#endif /* HyEngine_h__ */
