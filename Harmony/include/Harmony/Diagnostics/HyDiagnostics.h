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
#include "Diagnostics/Output/HyDiagOutput.h"

struct HarmonyInit;
class HyAssets;
class HyScene;

// Comment this out to disable profiler
#define HYSETTING_ProfilerEnabled

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
	friend class HyEngine;
	friend class IHyRenderer;

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

	HyDiagOutput				m_DiagOutput;

public:
	HyDiagnostics(HarmonyInit &initStruct, HyAssets &assetsRef, HyScene &sceneRef);
	~HyDiagnostics();

	void BootMessage();
	
	void Show(uint32 uiDiagFlags, float fX = 0.0f, float fY = 0.0f);
	uint32 GetShowFlags();

	void DumpAtlasUsage();
	void DumpNodeUsage();
	void DumpMemoryUsage();

	void StartMemoryCheckpoint();
	void EndMemoryCheckpoint();

	void ProfileBegin(const char *szName);
	void ProfileEnd();

private:
	void ApplyTimeDelta();
	void SetRendererInfo(const std::string &sApi, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);
};

#endif /* HyDiagnostics_h__ */
