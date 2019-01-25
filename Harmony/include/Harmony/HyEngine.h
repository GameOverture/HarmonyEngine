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
#include "Window/HyWindowManager.h"
#include "Audio/IHyAudio.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Diagnostics/GuiComms/HyGuiComms.h"
#include "Input/HyInput.h"
#include "Time/HyTime.h"
#include "Renderer/Effects/HyStencil.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyPrimitive2d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyPrefab3d.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyTexturedQuad2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntityLeaf2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity3d.h"
#include "Assets/HyAssets.h"
#include "Threading/IHyThreadClass.h"
#include "UI/IHy9Slice.h"
#include "UI/HyInfoPanel.h"
#include "UI/HyButton.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyImage.h"
#include "Utilities/HyStrManip.h"

class IHyEngine
{
	static IHyEngine *			sm_pInstance;

	// The order of these member declarations matter as some are used to initialize the others
	const HarmonyInit			m_Init;

	HyWindowManager				m_WindowManager;
	HyConsoleInterop			m_Console;
	
	HyScene						m_Scene;
	HyAssets 					m_Assets;
	HyGuiComms					m_GuiComms;
	HyTime						m_Time;
	HyDiagnostics				m_Diagnostics;
	HyInput						m_Input;

	HyRendererInterop			m_Renderer;

public:
	IHyEngine(HarmonyInit &initStruct);
	~IHyEngine();

	HyRendererInterop &GetRenderer();

	void RunGame();

protected:
	virtual bool OnUpdate() = 0;

#ifdef HY_PLATFORM_GUI
public:
#else
private:
#endif
	bool Update();
	bool PollPlatformApi();

	friend const HarmonyInit &	Hy_Init();
	friend float				Hy_UpdateStep();
	friend double				Hy_UpdateStepDbl();
	friend void					Hy_PauseGame(bool bPause);
	friend HyWindow &			Hy_Window(uint32 uiWindowIndex = 0);
	friend HyInput &			Hy_Input();
	friend b2World &			Hy_Physics2d();
	friend HyDiagnostics &		Hy_Diagnostics();
	friend HyShaderHandle		Hy_DefaultShaderHandle(HyType eType);
	friend bool					Hy_IsDrawInst(HyType eType);
	friend std::string			Hy_DateTime();
	friend std::string			Hy_DataDir();
};

#endif /* HyEngine_h__ */
