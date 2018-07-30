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
		tinygltf::TinyGLTF loader;
		std::string sError;
		std::string sFilePath = Hy_DataDir() + HYASSETS_PrefabDir + m_sIDENTIFIER + ".gltf";
		bool bLoadSuccess = loader.LoadASCIIFromFile(&m_ModelData, &sError, sFilePath);
		if(bLoadSuccess == false)
		{
			HyLogError("HyGLTF::OnLoadThread failed: " << sError.c_str());
			return;
		}
	}
}

void HyGLTF::OnRenderThread(IHyRenderer &rendererRef)
{
	uint32 i = static_cast<uint32>(m_ModelData.buffers.size());
	i++;
	//if(GetLoadableState() == HYLOADSTATE_Queued)
	//{
	//	m_ModelData.
	//	m_hTextureHandle = rendererRef.AddTexture(m_eTEXTURE_FORMAT, 0, m_uiWIDTH, m_uiHEIGHT, m_pPixelData, m_uiPixelDataSize, m_eTEXTURE_FORMAT);
	//	DeletePixelData();
	//}
	//else // GetLoadableState() == HYLOADSTATE_Discarded
	//{
	//	rendererRef.DeleteTexture(m_hTextureHandle);
	//}
}
