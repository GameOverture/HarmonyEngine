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

HyGLTF::HyGLTF(std::string sFilePath) :	IHyLoadableData(HYLOADABLE_Atlas),
										m_sFILEPATH(sFilePath)
{
}

HyGLTF::~HyGLTF()
{
}

void HyGLTF::OnLoadThread()
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		tinygltf::TinyGLTF loader;
		std::string sError;
		bool bLoadSuccess = loader.LoadASCIIFromFile(&m_ModelData, &sError, m_sFILEPATH);
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
