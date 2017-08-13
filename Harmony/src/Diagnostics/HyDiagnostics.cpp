/**************************************************************************
*	HyDiagnostics.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Diagnostics/HyDiagnostics.h"
#include "IHyApplication.h"
#include "Assets/HyAssets.h"
#include "Scene/Nodes/Leafs/IHyLeafDraw2d.h"
#include "Scene/Nodes/Leafs/Draws/HyText2d.h"
#include "HyEngine.h"

HyDiagnostics::HyDiagnostics(HarmonyInit &initStruct, HyAssets &assetsRef, HyScene &sceneRef) :	m_InitStructRef(initStruct),
																								m_AssetsRef(assetsRef),
																								m_SceneRef(sceneRef),
																								m_sPlatform("Unknown"),
																								m_uiNumCpuCores(0),
																								m_uiTotalMemBytes(0),
																								m_sGfxApi("Unknown"),
																								m_sVersion("Unknown"),
																								m_sVendor("Unknown"),
																								m_sRenderer("Unknown"),
																								m_sShader("Unknown"),
																								m_iMaxTextureSize(0),
																								m_sCompressedTextures("Unknown"),
																								m_bInitialMemCheckpointSet(false),
																								m_fFrameTime_Low(999.0f),
																								m_fFrameTime_High(0.0f),
																								m_fFrameTime_Cumulative(0.0f),
																								m_uiFrameCount(0),
																								m_pDiagOutput(nullptr)
{
#if defined(HY_PLATFORM_WINDOWS)
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
	m_uiTotalMemBytes = static_cast<uint64>(meminfo.ullTotalPhys);
	m_uiVirtualMemBytes = static_cast<uint64>(meminfo.ullTotalVirtual);

#elif defined(HY_PLATFORM_OSX)
	// Set info
	m_sPlatform = "Mac OSX";
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
	delete m_pDiagOutput;
}

HyDiagnostics::DiagOutput::DiagOutput(const char *szPrefix, const char *szName) :	m_txtLastFrameTime(szPrefix, szName, this),
																					m_txtAvgFrameInfo(szPrefix, szName, this),
																					m_txtProfilerResults(szPrefix, szName, this)
{
	m_txtLastFrameTime.SetAsScaleBox(500.0f, 14.0, true);
	m_txtAvgFrameInfo.SetAsScaleBox(500.0f, 50.0, true);
	m_txtAvgFrameInfo.pos.Y(-50.0f);
	m_txtProfilerResults.SetAsScaleBox(500.0f, 50.0, true);
	m_txtProfilerResults.pos.Y(-250.0f);
}

void HyDiagnostics::InitText(const char *szTextPrefix, const char *szTextName)
{
	delete m_pDiagOutput;
	m_pDiagOutput = new HyDiagnostics::DiagOutput(szTextPrefix, szTextName);
	m_pDiagOutput->Load();
	m_pDiagOutput->SetEnabled(false);
}

HyDiagnostics::DiagOutput *HyDiagnostics::GetDiagResultsPtr()
{
	return m_pDiagOutput;
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
	HyLogTitle(sGameTitle << "\n\t" << HyDateTime());
	HyLog("Data Dir:         " << m_InitStructRef.sDataDir);
	HyLog("Default Unit:     " << (m_InitStructRef.eDefaultCoordinateUnit == HYCOORDUNIT_Pixels) ? "Pixels" : "Meters");
	HyLog("Pixels/Meter:     " << m_InitStructRef.fPixelsPerMeter);
	HyLog("Num Input Maps:   " << m_InitStructRef.uiNumInputMappings);
	
	HyLogSection("Platform");
	HyLog(m_sPlatform);
	HyLog("Num CPU Cores:    " << m_uiNumCpuCores);
	HyLog("System Memory:    " << (m_uiTotalMemBytes / 1024 / 1024) << " MB");
	HyLog("Available Memory: " << (m_uiVirtualMemBytes / 1024 / 1024) << " MB");
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

#ifdef HYSETTING_ProfilerEnabled
void HyDiagnostics::ProfileBegin(const char *szName)
{
	HyAssert(m_CurProfileState.szName == nullptr, "HyDiagnostics::ProfileBegin invoked again without closing a previous call with ProfileEnd");

	m_CurProfileState.szName = szName;
	m_CurProfileState.time = clock();
}

void HyDiagnostics::ProfileEnd()
{
	HyAssert(m_CurProfileState.szName != nullptr, "HyDiagnostics::ProfileEnd invoked without an initial call from ProfileBegin");
	m_CurProfileState.time = clock() - m_CurProfileState.time;

	m_TotalClockTicks += m_CurProfileState.time;

	m_ProfileStateList.push_back(m_CurProfileState);
	m_CurProfileState.szName = nullptr;
}
#endif

void HyDiagnostics::Show()
{
	if(m_pDiagOutput)
		m_pDiagOutput->SetEnabled(true);
}
void HyDiagnostics::Hide()
{
	if(m_pDiagOutput)
		m_pDiagOutput->SetEnabled(false);
}

void HyDiagnostics::DumpAtlasUsage()
{
	HyAtlasIndices *pLoadedAtlases = m_AssetsRef.GetLoadedAtlases();
	std::map<uint32, std::vector<bool>> atlasGrpLoadsMap;

	uint32 uiNumUsed = 0;
	for(uint32 i = 0; i < m_AssetsRef.GetNumAtlases(); ++i)
	{
		HyAtlas *pAtlas = m_AssetsRef.GetAtlas(i);
		uint32 uiAtlasGrpId = pAtlas->GetAtlasGroupId();

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
	std::vector<IHyLeafDraw2d *> loadedNodesList;
	m_SceneRef.CopyAllLoadedNodes(loadedNodesList);

	uint32 uiNumSound2d = 0;
	uint32 uiNumParticles2d = 0;
	uint32 uiNumSprite2d = 0;
	uint32 uiNumSpine2d = 0;
	uint32 uiNumTextureQuad2d = 0;
	uint32 uiNumPrimitive2d = 0;
	uint32 uiNumText2d = 0;
	uint32 uiNumMesh3d = 0;
	for(uint32 i = 0; i < static_cast<uint32>(loadedNodesList.size()); ++i)
	{
		switch(loadedNodesList[i]->GetType())
		{
		case HYTYPE_Sound2d:		uiNumSound2d++;			break;
		case HYTYPE_Particles2d:	uiNumParticles2d++;		break;
		case HYTYPE_Sprite2d:		uiNumSprite2d++;		break;
		case HYTYPE_Spine2d:		uiNumSpine2d++;			break;
		case HYTYPE_TexturedQuad2d:	uiNumTextureQuad2d++;	break;
		case HYTYPE_Primitive2d:	uiNumPrimitive2d++;		break;
		case HYTYPE_Text2d:			uiNumText2d++;			break;
		case HYTYPE_Mesh3d:			uiNumMesh3d++;			break;
		}
	}

	HyLogSection("Loaded Node Usage");
	HyLog("Sound Nodes:        " << uiNumSound2d);
	HyLog("Particle Nodes:     " << uiNumParticles2d);
	HyLog("Sprite Nodes:       " << uiNumSprite2d);
	HyLog("Spine Nodes:        " << uiNumSpine2d);
	HyLog("TexturedQuad Nodes: " << uiNumTextureQuad2d);
	HyLog("Primitive Nodes:    " << uiNumPrimitive2d);
	HyLog("Text Nodes:         " << uiNumText2d);
	HyLog("3D Mesh Nodes:      " << uiNumMesh3d);
}

void HyDiagnostics::DumpMemoryUsage()
{
	HyLogSection("Memory Usage");

#if defined(HY_PLATFORM_WINDOWS)
	PROCESS_MEMORY_COUNTERS memCounter;
	BOOL bResult = GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof(memCounter));
	if(bResult == false)
	{
		HyLogError("HyDiagnostics::DumpMemoryUsage - GetProcessMemoryInfo() failed and returned error: " << GetLastError());
		return;
	}
	uint64 uiUsedMem = static_cast<uint64>(memCounter.WorkingSetSize);

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

void HyDiagnostics::Update()
{
	float fCurFrameTime = Hy_LastFrameTime();
	m_fFrameTime_Cumulative += fCurFrameTime;
	fCurFrameTime *= 1000.0f;
	m_fFrameTime_Low = HyMin(m_fFrameTime_Low, fCurFrameTime);
	m_fFrameTime_High = HyMax(m_fFrameTime_High, fCurFrameTime);
	m_uiFrameCount++;

	if(m_pDiagOutput)
	{
		std::stringstream ss;
		ss << "Cur: " << fCurFrameTime << "ms";
		m_pDiagOutput->m_txtLastFrameTime.TextSet(ss.str());

#ifdef HYSETTING_ProfilerEnabled
		HyAssert(m_CurProfileState.szName == nullptr, "HyDiagnostics::Update invoked with an open Profile begin");

		std::string sText;
		uint32 uiNumProfileStates = static_cast<uint32>(m_ProfileStateList.size());
		for(uint32 i = 0; i < uiNumProfileStates; ++i)
		{
			sText += m_ProfileStateList[i].szName;
			sText += ": ";
			sText += std::to_string(static_cast<float>(m_ProfileStateList[i].time) / static_cast<float>(m_TotalClockTicks));
			sText += "\n";

		}
		m_pDiagOutput->m_txtProfilerResults.TextSet(sText);

		m_uiPro

		m_TotalClockTicks = 0;
#endif
	}

	if(m_fFrameTime_Cumulative >= 1.0f)
	{
		if(m_pDiagOutput)
		{
			std::stringstream ss;
			ss << "Avg: " << static_cast<float>(m_fFrameTime_Cumulative / m_uiFrameCount) * 1000.0f << "ms\n" <<
				  "Low: " << m_fFrameTime_Low << "ms\n" <<
				  "High: " << m_fFrameTime_High << "ms\n" <<
				  "FPS: " << m_uiFrameCount;
			m_pDiagOutput->m_txtAvgFrameInfo.TextSet(ss.str());
		}

		m_fFrameTime_Low = 9999.0f;	// Any large value that should be greater than any single frame time
		m_fFrameTime_High = 0.0f;
		m_fFrameTime_Cumulative = 0.0f;
		m_uiFrameCount = 0;
	}
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
