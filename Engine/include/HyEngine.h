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
#include "Utilities/HyMain.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Diagnostics/GuiComms/HyGuiComms.h"
#include "Input/HyInput.h"
#include "Input/IHyController.h"
#include "Renderer/Effects/HyStencil.h"
#include "Networking/IHyNetworkClass.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrefab3d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntityLeaf2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyPhysicsGrid2d.h"
#include "Threading/IHyThreadClass.h"
#include "Threading/IHyThreadClass.h"
#include "Time/HyTime.h"
#include "UI/HyUiContainer.h"
#include "UI/Widgets/HyButton.h"
#include "UI/Widgets/HyCheckBox.h"
#include "UI/Widgets/HyComboBox.h"
#include "UI/Widgets/HyLineEdit.h"
#include "UI/Widgets/HyRackMeter.h"
#include "UI/Widgets/HyProgressBar.h"
#include "UI/Widgets/HyRichText.h"
#include "UI/Widgets/HySlider.h"
#include "UI/Localization/HyLocale.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyImage.h"
#include "Utilities/HyIO.h"
#include "Utilities/HyJson.h"
#include "Utilities/HyRand.h"
#include "Window/HyWindowManager.h"

class HyEngine
{
	static HyEngine *			sm_pInstance;

	// The order of these member declarations matter as some are used to initialize each other
	const HarmonyInit			m_Init;
	HyConsoleInterop			m_Console;
	HyWindowManager				m_WindowManager;
	HyInput						m_Input;
	HyAudioCore 				m_Audio;
	HyNetworkInterop			m_Networking;
	HyScene						m_Scene;
	HyAssets 					m_Assets;
	HyTime						m_Time;
	HyDiagnostics				m_Diagnostics;
	HyRendererInterop			m_Renderer;

public:
	HyEngine(const HarmonyInit &initStruct);
	~HyEngine();

	HyRendererInterop &GetRenderer();

	int32 RunGame();

	virtual void OnWindowResized(HyWindow &windowRef) {
		windowRef.SetFramebufferSize(windowRef.GetWindowSize());
	}
	virtual void OnWindowMoved(HyWindow &windowRef) { }

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

#if defined(HY_PLATFORM_GUI)
	void SetWidgetMousePos(glm::vec2 ptMousePos);
#endif

public:
	static bool IsInitialized();
	static const HarmonyInit &InitValues();
	static float DeltaTime();
	static double DeltaTimeD();
	static void PauseGame(bool bPause);
	static uint32 NumWindows();
	static HyWindow &Window(uint32 uiWindowIndex = 0);
	static HyInput &Input();
	static HyRendererInterop &Renderer();
	static HyAudioCore &Audio();
	static void LoadingStatus(uint32 &uiNumQueuedOut, uint32 &uiTotalOut);
	static HyDiagnostics &Diagnostics();
	static HyShaderHandle DefaultShaderHandle(HyType eType);
	static std::string DateTime();
	static std::string DataDir();
	static HyTextureQuadHandle CreateTexture(std::string sFilePath, HyTextureInfo textureInfo);
	static HyAudioHandle CreateAudio(std::string sFilePath, bool bIsStreaming = false, int32 iInstanceLimit = 0, int32 iCategoryId = 0);
};

#endif /* HyEngine_h__ */
