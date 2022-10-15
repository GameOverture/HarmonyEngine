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
#include "Diagnostics/HyProfiler.h"

struct HarmonyInit;
class HyAssets;
class HyScene;
class HyTime;

class HyDiagnostics
{
	friend class HyEngine;
	friend class IHyRenderer;

	const HarmonyInit &					m_InitStructRef;
	HyTime &							m_TimeRef;
	HyAssets &							m_AssetsRef;
	HyScene &							m_SceneRef;

	std::string							m_sCompiler;
	std::string							m_sPlatform;
	uint32								m_uiNumCpuCores;
	uint32								m_uiL1CacheSizeBytes;
	uint64_t							m_uiTotalMemBytes;
	uint64_t							m_uiVirtualMemBytes;

	std::string							m_sGfxApi;
	std::string							m_sVersion;
	std::string							m_sVendor;
	std::string							m_sRenderer;
	std::string							m_sShader;
	int32								m_iMaxTextureSize;
	std::string							m_sCompressedTextures;

	bool								m_bInitialMemCheckpointSet;
#if defined(HY_DEBUG) && defined(HY_COMPILER_MSVC) && defined(HY_PLATFORM_WINDOWS)
	_CrtMemState						m_MemCheckpoint1;
	_CrtMemState						m_MemCheckpoint2;
#endif

	HyProfiler *						m_pProfiler;
	std::function<void()>				m_fpBeginFrame;
	std::function<void()>				m_fpBeginUpdate;
	std::function<void()>				m_fpBeginRenderPrep;
	std::function<void()>				m_fpBeginRender;

public:
	HyDiagnostics(const HarmonyInit &initStruct, HyTime &timeRef, HyAssets &assetsRef, HyScene &sceneRef);
	~HyDiagnostics();

	void Init(std::string sTextPrefix, std::string sTextName, uint32 uiTextState);
	void Show(uint32 uiDiagFlags);
	uint32 GetShowFlags();

	void DumpAtlasUsage();
	void DumpNodeUsage();
	void DumpMemoryUsage();

	void StartMemoryCheckpoint();
	void EndMemoryCheckpoint();

private:
	void BootMessage();
	void SetRendererInfo(const std::string &sApi, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures);

	void BeginFrame();
	void BeginUpdate();
	void BeginRenderPrep();
	void BeginRender();
};

#endif /* HyDiagnostics_h__ */
