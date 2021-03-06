/**************************************************************************
*	HyDiagnostics.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyDiagnostics_h__
#define HyDiagnostics_h__

#include "Afx/HyStdAfx.h"
#include "Diagnostics/Output/HyDiagOutput.h"

struct HarmonyInit;
class HyTime;
class HyAssets;
class HyScene;

#if defined(HY_COMPILER_MSVC)
	// Comment this out to disable profiler
	#define HYSETTING_ProfilerEnabled
#endif

#if defined(HY_PLATFORM_GUI) || defined(HY_PLATFORM_BROWSER)
	#undef HYSETTING_ProfilerEnabled
#endif
#ifdef HYSETTING_ProfilerEnabled
	#define HY_PROFILE_BEGIN(name) HyEngine::Diagnostics().ProfileBegin(name);
	#define HY_PROFILE_END HyEngine::Diagnostics().ProfileEnd();
#else
	#define HY_PROFILE_BEGIN(name) 
	#define HY_PROFILE_END 
#endif

class HyDiagnostics
{
	friend class HyEngine;
	friend class IHyRenderer;

	const HarmonyInit &			m_InitStructRef;
	HyTime &					m_TimeRef;
	HyAssets &					m_AssetsRef;
	HyScene &					m_SceneRef;

	std::string					m_sCompiler;
	std::string					m_sPlatform;
	uint32						m_uiNumCpuCores;
	uint32						m_uiL1CacheSizeBytes;
	uint64_t					m_uiTotalMemBytes;
	uint64_t					m_uiVirtualMemBytes;

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

	HyProfiler					m_Profiler;
	HyDiagOutput *				m_pDiagOutput;

public:
	HyDiagnostics(const HarmonyInit &initStruct, HyTime &timeRef, HyAssets &assetsRef, HyScene &sceneRef);
	~HyDiagnostics();

	void BootMessage();
	
	void Show(uint32 uiDiagFlags);
	uint32 GetShowFlags();

	void DumpAtlasUsage();
	void DumpNodeUsage();
	void DumpMemoryUsage();

	void StartMemoryCheckpoint();
	void EndMemoryCheckpoint();

	void ProfileBegin(HyProfilerSection eSection);
	void ProfileEnd();

private:
	void ApplyTimeDelta();
	void SetRendererInfo(const std::string &sApi, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);
};

#endif /* HyDiagnostics_h__ */
