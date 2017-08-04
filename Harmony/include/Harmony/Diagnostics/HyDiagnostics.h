/**************************************************************************
*	HyDiagnostics.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyDiagnostics_h__
#define HyDiagnostics_h__

#include "Afx/HyStdAfx.h"

#include <time.h>

struct HarmonyInit;
class HyAssets;
class HyScene;
class HyText2d;

// Comment this out to disable profiler
//#define HYSETTING_ProfilerEnabled


#ifdef HY_PLATFORM_GUI
	#undef HYSETTING_ProfilerEnabled
#endif
#ifdef HYSETTING_ProfilerEnabled
	#define HY_PROFILE_BEGIN(name) HyGetDiagnostics().ProfileBegin(name);
	#define HY_PROFILE_END HyGetDiagnostics().ProfileEnd();
#else
	#define HY_PROFILE_BEGIN(name) 
	#define HY_PROFILE_END 
#endif

class HyDiagnostics
{
	friend class IHyRenderer;
	friend class IHyTime;

	HarmonyInit &				m_InitStructRef;
	HyAssets &					m_AssetsRef;
	HyScene &					m_SceneRef;

	std::string					m_sPlatform;
	uint32						m_uiNumCpuCores;
	uint64						m_uiTotalMemBytes;
	uint64						m_uiVirtualMemBytes;

	std::string					m_sGfxApi;
	std::string					m_sVersion;
	std::string					m_sVendor;
	std::string					m_sRenderer;
	std::string					m_sShader;
	int32						m_iMaxTextureSize;
	std::string					m_sCompressedTextures;

	bool						m_bInitialMemCheckpointSet;
#if defined(HY_DEBUG) && defined(HY_COMPILER_MSVC) && defined(HY_PLATFORM_WINDOWS)
	_CrtMemState				m_MemCheckpoint1;
	_CrtMemState				m_MemCheckpoint2;
#endif

#ifdef HYSETTING_ProfilerEnabled
	struct ProfileState
	{
		const char *	szName;
		clock_t			time;

		ProfileState() : szName(nullptr), time(0)
		{ }
	};
	ProfileState				m_CurProfileState;
	std::vector<ProfileState>	m_ProfileStateList;
	clock_t						m_TotalClockTicks;
#endif

	uint32						m_uiFps_Update;
	uint32						m_uiFps_Render;
	HyText2d *					m_pFpsText;
	bool						m_bPrintFpsToConsole;

public:
	HyDiagnostics(HarmonyInit &initStruct, HyAssets &assetsRef, HyScene &sceneRef);
	~HyDiagnostics();

	void InitText(const char *szTextPrefix, const char *szTextName);
	HyText2d *GetFpsText();

	void BootMessage();

#ifdef HYSETTING_ProfilerEnabled
	void ProfileBegin(const char *szName);
	void ProfileEnd();
#endif
	
	void ShowFps(bool bShowOnScreen, bool bShowConsole);

	void DumpAtlasUsage();
	void DumpNodeUsage();
	void DumpMemoryUsage();

	void StartMemoryCheckpoint();
	void EndMemoryCheckpoint();

	void Update();

private:
	void SetRendererInfo(const std::string &sApi, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);

	void SetCurrentFps(uint32 uiFps_Update, uint32 uiFps_Render);
};

#endif /* HyDiagnostics_h__ */
