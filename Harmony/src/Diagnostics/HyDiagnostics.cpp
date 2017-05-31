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
																								m_sShader("Unknown")
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
	HyLogTitle(sGameTitle);
	HyLog("Data Dir:         " << m_InitStructRef.sDataDir);
	HyLog("Default Coord:    " << (m_InitStructRef.eDefaultCoordinateType == HYCOORDTYPE_Camera) ? "Camera" : "Screen");
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
	HyLog("Version:        " << m_sVersion);
	HyLog("Vendor:         " << m_sVendor);
	HyLog("Renderer:       " << m_sRenderer);
	HyLog("Shader:         " << m_sShader);
	HyLog("");
}

void HyDiagnostics::DumpAtlasUsage()
{
	HyAtlasIndices *pLoadedAtlases = m_AssetsRef.GetLoadedAtlases();

	std::string sAtlasNumbering("");
	std::string sAtlasUsage("");
	uint32 uiNumUsed = 0;
	for(uint32 i = 0; i < m_AssetsRef.GetNumAtlases(); ++i)
	{
		sAtlasNumbering += std::to_string(i);
		sAtlasNumbering += " ";

		if(pLoadedAtlases->IsSet(i))
		{
			uiNumUsed++;
			sAtlasUsage += "O ";
		}
		else
			sAtlasUsage += "X ";
	}
	
	HyLogSection("Atlas Usage");
	HyLog(uiNumUsed << "/" << m_AssetsRef.GetNumAtlases() << " used. (" << (static_cast<float>(uiNumUsed) / static_cast<float>(m_AssetsRef.GetNumAtlases())) << "%)");
	HyLog(sAtlasNumbering);
	HyLog(sAtlasUsage);
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

void HyDiagnostics::SetRendererInfo(const std::string &sApi, const std::string &sVersion, const std::string &sVendor, const std::string &sRenderer, const std::string &sShader)
{
	m_sGfxApi = sApi;
	m_sVersion = sVersion;
	m_sVendor = sVendor;
	m_sRenderer = sRenderer;
	m_sShader = sShader;
}
