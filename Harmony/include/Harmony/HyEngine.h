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
#include "IHyApplication.h"
#include "Audio/IHyAudio.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Diagnostics/GuiComms/HyGuiComms.h"
#include "Input/HyInput.h"
#include "Time/HyTime.h"
#include "Renderer/Components/HyWindow.h"
#include "Renderer/Effects/HyStencil.h"
#include "Renderer/Effects/HyPortal2d.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Misc/HyAudio2d.h"
#include "Scene/Nodes/Draws/Instances/HySprite2d.h"
#include "Scene/Nodes/Draws/Instances/HySpine2d.h"
#include "Scene/Nodes/Draws/Instances/HyPrimitive2d.h"
#include "Scene/Nodes/Draws/Instances/HyText2d.h"
#include "Scene/Nodes/Draws/Instances/HyTexturedQuad2d.h"
#include "Scene/Nodes/Draws/Entities/HyEntityLeaf2d.h"
#include "Assets/HyAssets.h"
#include "Threading/IHyThreadClass.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyStrManip.h"
#include "Utilities/HyImage.h"

class HyEngine
{
	static HyEngine *			sm_pInstance;

	// The order of these member declarations matter
	IHyApplication &			m_AppRef;
	
	HyScene						m_Scene;
	HyAssets 					m_Assets;
	HyGuiComms					m_GuiComms;
	HyTime						m_Time;
	HyDiagnostics				m_Diagnostics;
	HyInput						m_Input;

	HyRendererInterop			m_Renderer;
	

// If HY_PLATFORM_GUI, make this ctor public as gui tool requires special usage.
#ifdef HY_PLATFORM_GUI
public:
	static HyEngine *GuiCreate(IHyApplication &projectRef);
	static void GuiDelete();
#endif

	// Otherwise, private ctor invoked from RunGame(), once.
	HyEngine(IHyApplication &gameRef);

public:
	~HyEngine();

	static void RunGame(IHyApplication *pDynamicallyAllocatedGame);

#ifndef HY_PLATFORM_GUI
private:
#endif
	bool IsInitialized();
	bool Update();
	void Shutdown();

	bool PollPlatformApi();

	HyRendererInterop &GetRenderer();

	friend float			Hy_UpdateStep();
	friend double			Hy_UpdateStepDbl();
	friend void				Hy_PauseGame(bool bPause);
	friend HyInput &		Hy_Input();
	friend b2World &		Hy_Physics2d();
	friend HyDiagnostics &	Hy_Diagnostics();
	friend HyShaderHandle	Hy_DefaultShaderHandle(HyType eType);
	friend bool				Hy_IsDrawInst(HyType eType);
	friend std::string		Hy_DateTime();
};

#endif /* HyEngine_h__ */
