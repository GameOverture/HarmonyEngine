/**************************************************************************
*	HyDiagnostics.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Diagnostics/HyDiagnostics.h"
#include "Time/HyTime.h"
#include "Assets/HyAssets.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.h"
#include "HyEngine.h"

extern std::string Hy_DateTime();
extern HyWindow &Hy_Window();

HyDiagnostics::HyDiagnostics(const HarmonyInit &initStruct, HyTime &timeRef, HyAssets &assetsRef, HyScene &sceneRef) :
	m_InitStructRef(initStruct),
	m_TimeRef(timeRef),
	m_AssetsRef(assetsRef),
	m_SceneRef(sceneRef),
	m_sCompiler("Unknown"),
	m_sPlatform("Unknown"),
	m_uiNumCpuCores(0),
	m_uiL1CacheSizeBytes(0),
	m_uiTotalMemBytes(0),
	m_uiVirtualMemBytes(0),
	m_sGfxApi("Unknown"),
	m_sVersion("Unknown"),
	m_sVendor("Unknown"),
	m_sRenderer("Unknown"),
	m_sShader("Unknown"),
	m_iMaxTextureSize(0),
	m_sCompressedTextures("Unknown"),
	m_bInitialMemCheckpointSet(false)
{
#if defined(HY_COMPILER_MSVC)
	m_sCompiler = "MSVC";
#elif defined(HY_COMPILER_MWERKS)
	m_sCompiler = "MWERKS";
#elif defined(HY_COMPILER_GNU)
	m_sCompiler = "GNU";
#endif

#ifdef HY_USE_SDL2
	m_sPlatform = SDL_GetPlatform();
	m_uiNumCpuCores = SDL_GetCPUCount();
	m_uiL1CacheSizeBytes = SDL_GetCPUCacheLineSize();

#ifndef HY_PLATFORM_BROWSER // Not supported with SDL2's Emscripten
	m_uiTotalMemBytes = static_cast<uint64_t>(SDL_GetSystemRAM()) * 1024;
#endif

#elif defined(HY_PLATFORM_WINDOWS)
	m_sPlatform = "Windows";

	// Get number of CPUs
	SYSTEM_INFO sysinfo;
	#if defined(HY_ENV_32)
		GetSystemInfo(&sysinfo);
	#else
		GetNativeSystemInfo(&sysinfo);
	#endif
	m_uiNumCpuCores = sysinfo.dwNumberOfProcessors;

	// Get total memory size
	MEMORYSTATUSEX meminfo = {};
	meminfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&meminfo);
	m_uiTotalMemBytes = static_cast<uint64_t>(meminfo.ullTotalPhys);
	m_uiVirtualMemBytes = static_cast<uint64_t>(meminfo.ullTotalVirtual);

#elif defined(HY_PLATFORM_OSX)
	m_sPlatform = "Mac OSX";
#elif defined(HY_PLATFORM_LINUX)
	m_sPlatform = "Linux";
#elif defined(HY_PLATFORM_BROWSER)
	m_sPlatform = "Browser";
#endif

	// Confirm endian-ness with what is defined
	union
	{
		uint32 i;
		char c[4];
	} bint = { 0x01020304 };
#if defined(HY_ENDIAN_LITTLE)
	if(bint.c[0] == 1) {
		HyError("HY_ENDIAN_LITTLE was defined but did not pass calculation test");
	}
#else
	if(bint.c[0] != 1) {
		HyError("HY_ENDIAN_BIG was defined but did not pass calculation test");
	}
#endif
}

HyDiagnostics::~HyDiagnostics()
{
}

void HyDiagnostics::BootMessage()
{
	std::string sGameTitle = m_InitStructRef.sGameName;
#if defined(HY_DEBUG)
	sGameTitle += " [Debug]";
#else
	sGameTitle += " [Release]";
#endif

	HyLog("");
	HyLogTitle(sGameTitle << "\n\t" << Hy_DateTime());
	HyLog("Compiler:         " << m_sCompiler);
	HyLog("Data Dir:         " << m_InitStructRef.sDataDir);
	HyLog("Num Input Maps:   " << m_InitStructRef.uiNumInputMappings);
	
	HyLogSection("Platform");

	HyLog(m_sPlatform);
	if(m_uiNumCpuCores != 0) {
		HyLog("Num CPU Cores:    " << m_uiNumCpuCores);
	}
	else {
		HyLog("Num CPU Cores:    unknown");
	}
	if(m_uiL1CacheSizeBytes != 0) {
		HyLog("CPU L1 Cache:     " << m_uiL1CacheSizeBytes << " bytes")
	}
	else {
		HyLog("CPU L1 Cache:     unknown");
	}
	HyLog("System Memory:    " << (m_uiTotalMemBytes / 1024 / 1024) << " MB");
	if(m_uiVirtualMemBytes != 0) {
		HyLog("Available Memory: " << (m_uiVirtualMemBytes / 1024 / 1024) << " MB");
	}
#if defined(HY_ENDIAN_LITTLE)
	HyLog("Endian:           " << "Little");
#else
	HyLog("Endian:           " << "Big");
#endif

	HyLogSection(m_sGfxApi);

	HyLog("Version:          " << m_sVersion);
	HyLog("Vendor:           " << m_sVendor);
	HyLog("Renderer:         " << m_sRenderer);
	HyLog("Shader:           " << m_sShader);
	HyLog("Max Texture Size: " << m_iMaxTextureSize);
	HyLog("Compression:      " << m_sCompressedTextures);
	HyLog("");
}

void HyDiagnostics::Show(uint32 uiDiagFlags)
{
	if(uiDiagFlags != 0 && m_DiagOutput.IsLoaded() == false)
		m_DiagOutput.Load();

	m_DiagOutput.SetShowFlags(uiDiagFlags);
	m_DiagOutput.pos.Set(5, Hy_Window().GetHeight() - 5);
	m_DiagOutput.UseWindowCoordinates(0);
}

uint32 HyDiagnostics::GetShowFlags()
{
	return m_DiagOutput.GetShowFlags();
}

void HyDiagnostics::DumpAtlasUsage()
{
	HyFilesManifest *pLoadedAtlases = m_AssetsRef.GetLoadedAtlases();
	std::map<uint32, std::vector<bool>> atlasGrpLoadsMap;

	uint32 uiNumUsed = 0;
	for(uint32 i = 0; i < m_AssetsRef.GetNumAtlases(); ++i)
	{
		HyFileAtlas *pAtlas = static_cast<HyFileAtlas *>(m_AssetsRef.GetFile(HYFILE_Atlas, i));
		uint32 uiAtlasGrpId = pAtlas->GetBankId();

		if(atlasGrpLoadsMap.find(uiAtlasGrpId) == atlasGrpLoadsMap.end())
			atlasGrpLoadsMap[uiAtlasGrpId] = std::vector<bool>();

		if(pLoadedAtlases->IsSet(i))
		{
			atlasGrpLoadsMap[uiAtlasGrpId].push_back(true);
			uiNumUsed++;
		}
		else
			atlasGrpLoadsMap[uiAtlasGrpId].push_back(false);
	}
	
	HyLogSection("Atlas Usage");
	HyLog("Total:\t" << uiNumUsed << "/" << m_AssetsRef.GetNumAtlases() << " used (" << (static_cast<float>(uiNumUsed) / static_cast<float>(m_AssetsRef.GetNumAtlases())) * 100.0f << "%)");
	for(auto iter = atlasGrpLoadsMap.begin(); iter != atlasGrpLoadsMap.end(); ++iter)
	{
		uint32 uiNumUsedInGrp = 0;
		std::string sUsedList;
		for(uint32 i = 0; i < iter->second.size(); ++i)
		{
			if(iter->second[i])
			{
				sUsedList += "1";
				uiNumUsedInGrp++;
			}
			else
				sUsedList += "0";

			if(i != iter->second.size() - 1)
				sUsedList += ",";
		}

		HyLog("");
		HyLog("Grp " << iter->first << ":\t" << uiNumUsedInGrp << "/" << iter->second.size() << " used (" << (static_cast<float>(uiNumUsedInGrp) / static_cast<float>(iter->second.size())) * 100.0f << "%)");
		HyLog("\t" << sUsedList);
	}
}

void HyDiagnostics::DumpNodeUsage()
{
	std::vector<IHyInstance2d *> loadedNodesList;
	m_SceneRef.CopyAllLoadedNodes(loadedNodesList);

	uint32 uiNumAudio = 0;
	uint32 uiNumParticles2d = 0;
	uint32 uiNumSprite2d = 0;
	uint32 uiNumSpine2d = 0;
	uint32 uiNumTextureQuad2d = 0;
	uint32 uiNumPrimitive2d = 0;
	uint32 uiNumText2d = 0;
	uint32 uiNumPrefab3d = 0;
	for(uint32 i = 0; i < static_cast<uint32>(loadedNodesList.size()); ++i)
	{
		switch(loadedNodesList[i]->GetType())
		{
		case HYTYPE_Audio:			uiNumAudio++;			break;
		case HYTYPE_Particles:		uiNumParticles2d++;		break;
		case HYTYPE_Sprite:			uiNumSprite2d++;		break;
		case HYTYPE_Spine:			uiNumSpine2d++;			break;
		case HYTYPE_TexturedQuad:	uiNumTextureQuad2d++;	break;
		case HYTYPE_Primitive:		uiNumPrimitive2d++;		break;
		case HYTYPE_Text:			uiNumText2d++;			break;
		case HYTYPE_Prefab:			uiNumPrefab3d++;		break;

		default:
			break;
		}
	}

	HyLogSection("Loaded Node Usage");
	HyLog("Sound Nodes:        " << uiNumAudio);
	HyLog("Particle Nodes:     " << uiNumParticles2d);
	HyLog("Sprite Nodes:       " << uiNumSprite2d);
	HyLog("Spine Nodes:        " << uiNumSpine2d);
	HyLog("TexturedQuad Nodes: " << uiNumTextureQuad2d);
	HyLog("Primitive Nodes:    " << uiNumPrimitive2d);
	HyLog("Text Nodes:         " << uiNumText2d);
	HyLog("3D Prefab Nodes:    " << uiNumPrefab3d);
}

void HyDiagnostics::DumpMemoryUsage()
{
	HyLogSection("Memory Usage");

#if defined(HY_COMPILER_MSVC) && defined(HY_PLATFORM_WINDOWS)
	PROCESS_MEMORY_COUNTERS memCounter;
	BOOL bResult = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
	if(bResult == false)
	{
		HyLogError("HyDiagnostics::DumpMemoryUsage - GetProcessMemoryInfo() failed and returned error: " << GetLastError());
		return;
	}
	uint64_t uiUsedMem = static_cast<uint64_t>(memCounter.WorkingSetSize);

	HyLog("Working set memory: " << (static_cast<double>(uiUsedMem) / 1024.0 / 1024.0) << " MB (" << (static_cast<float>(uiUsedMem) / static_cast<float>(m_uiVirtualMemBytes)) * 100.0f << "%)");
	HyLog("Available Memory:   " << (m_uiVirtualMemBytes / 1024 / 1024) << " MB");

#else
	HyLogWarning("HyDiagnostics::DumpMemoryUsage not implemented for this platform");
#endif
}

void HyDiagnostics::StartMemoryCheckpoint()
{
#if defined(HY_DEBUG) && defined(HY_COMPILER_MSVC) && defined(HY_PLATFORM_WINDOWS)
	_CrtMemCheckpoint(&m_MemCheckpoint1);
	m_bInitialMemCheckpointSet = true;
	HyLog("Stored initial memory checkpoint");
#else
	HyLogWarning("HyDiagnostics::StartMemoryCheckpoint does not function with the 'Release' build configuration OR it may not be supported by the compiler that was used");
#endif
}

void HyDiagnostics::EndMemoryCheckpoint()
{
#if defined(HY_DEBUG) && defined(HY_COMPILER_MSVC) && defined(HY_PLATFORM_WINDOWS)
	if(m_bInitialMemCheckpointSet == false)
	{
		HyLogWarning("HyDiagnostics::EndMemoryCheckpoint - Initial memory checkpoint was not set. Invoke HyDiagnostics::StartMemoryCheckpoint first.");
		return;
	}

	_CrtMemCheckpoint(&m_MemCheckpoint2);

	_CrtMemState memDifference;
	if(_CrtMemDifference(&memDifference, &m_MemCheckpoint1, &m_MemCheckpoint2))
	{
		HyLog("Dumping memory difference between initial memory checkpoint and now...");
		_CrtMemDumpStatistics(&memDifference);
	}
	else {
		HyLog("No significant difference in memory checkpoints");
	}
#else
	HyLogWarning("HyDiagnostics::EndMemoryCheckpoint does not function with the 'Release' build configuration OR it may not be supported by the compiler that was used");
#endif
}

void HyDiagnostics::ProfileBegin(HyProfilerSection eSection)
{
	m_Profiler.BeginSection(eSection);
}

void HyDiagnostics::ProfileEnd()
{
	m_Profiler.EndSection();
}

void HyDiagnostics::ApplyTimeDelta()
{
	m_DiagOutput.ApplyTimeDelta(m_TimeRef.GetUpdateStepSecondsDbl());
}

void HyDiagnostics::SetRendererInfo(const std::string &sApi, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader, int32 iMaxTextureSize, const std::string &sCompressedTextures)
{
	m_sGfxApi = sApi;
	m_sVersion = sVersion;
	m_sVendor = sVendor;
	m_sRenderer = sRenderer;
	m_sShader = sShader;
	m_iMaxTextureSize = iMaxTextureSize;
	m_sCompressedTextures = sCompressedTextures;
}
