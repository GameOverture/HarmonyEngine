/**************************************************************************
 *	HyAtlas.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Loadables/HyGLTF.h"
#include "Renderer/IHyRenderer.h"
#include "Diagnostics/Console/HyConsole.h"
#include "HyEngine.h"

HyGLTF::HyGLTF(const std::string &sIdentifier) :	IHyLoadableData(HYLOADABLE_GLTF),
													m_sIDENTIFIER(sIdentifier)
{
}

HyGLTF::~HyGLTF()
{
}

const std::string &HyGLTF::GetIdentifier()
{
	return m_sIDENTIFIER;
}

void HyGLTF::OnLoadThread()
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		const std::string &sDataDir = Hy_DataDir();

		std::string sGameDataFilePath(sDataDir);
		sGameDataFilePath += HYASSETS_DataFile;

		std::string sGameDataFileContents;
		HyReadTextFile(sGameDataFilePath.c_str(), sGameDataFileContents);

		jsonxx::Object gameDataObj;
		bool bGameDataParsed = gameDataObj.parse(sGameDataFileContents);
		HyAssert(bGameDataParsed, "Could not parse game data");

		const jsonxx::Object &prefabObj = gameDataObj.get<jsonxx::Object>("Prefabs");
		const jsonxx::Object &gltfObj = prefabObj.get<jsonxx::Object>(m_sIDENTIFIER);
		std::string &sGltf = gltfObj.json();

		std::string sBaseDir = sDataDir + HYASSETS_PrefabDir + m_sIDENTIFIER;
		size_t uiIndex = sBaseDir.rfind("/");
		if(uiIndex != std::string::npos)
			sBaseDir.erase(uiIndex);

		tinygltf::TinyGLTF loader;
		std::string sError;
		bool bLoadSuccess = loader.LoadASCIIFromString(&m_ModelData, &sError, sGltf.c_str(), static_cast<uint32>(sGltf.length()), sBaseDir);
		if(bLoadSuccess == false)
		{
			HyLogError("HyGLTF::OnLoadThread failed: " << sError.c_str());
			return;
		}
	}
}

void HyGLTF::OnRenderThread(IHyRenderer &rendererRef)
{
	//m_Mutex_PixelData.lock();
	//if(GetLoadableState() == HYLOADSTATE_Queued)
	//{
	//	m_hTextureHandle = rendererRef.AddTexture(m_eTEXTURE_FORMAT, 0, m_uiWIDTH, m_uiHEIGHT, m_pPixelData, m_uiPixelDataSize, m_eTEXTURE_FORMAT);
	//	DeletePixelData();
	//}
	//else // GetLoadableState() == HYLOADSTATE_Discarded
	//{
	//	rendererRef.DeleteTexture(m_hTextureHandle);
	//}
	//m_Mutex_PixelData.unlock();
}
