/**************************************************************************
 *	HyAtlas.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Loadables/HyAtlas.h"

#include "Assets/HyAssets.h"
#include "Renderer/IHyRenderer.h"

#include "Utilities/stb_image.h"

#include <set>

HyAtlas::HyAtlas(std::string sFilePath, uint32 uiIndex, uint32 uiWidth, uint32 uiHeight, uint32 uiNumClrChannels, jsonxx::Array &srcFramesArrayRef) :	IHyLoadableData(HYGFXTYPE_AtlasGroup),
																																						m_uiINDEX(uiIndex),
																																						m_sFILE_PATH(sFilePath),
																																						m_uiWIDTH(uiWidth),
																																						m_uiHEIGHT(uiHeight),
																																						m_uiNUM_8BIT_CHANNELS(uiNumClrChannels),
																																						m_uiGfxApiHandle(0),
																																						m_uiNUM_FRAMES(static_cast<uint32>(srcFramesArrayRef.size())),
																																						m_pPixelData(NULL)
{
	m_pFrames = HY_NEW HyRectangle<int32>[m_uiNUM_FRAMES];

	for(uint32 k = 0; k < m_uiNUM_FRAMES; ++k)
	{
		jsonxx::Object srcFrameObj = srcFramesArrayRef.get<jsonxx::Object>(k);

		m_pFrames[k].bottom = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("bottom"));
		m_pFrames[k].right = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("right"));
		m_pFrames[k].left = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("left"));
		m_pFrames[k].top = static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("top"));

		m_ChecksumMap[static_cast<uint32>(srcFrameObj.get<jsonxx::Number>("checksum"))] = &m_pFrames[k];
	}
}

HyAtlas::~HyAtlas()
{
	delete [] m_pFrames;
	DeletePixelData();
}

uint32 HyAtlas::GetIndex() const
{
	return m_uiINDEX;
}

uint32 HyAtlas::GetWidth() const
{
	return m_uiWIDTH;
}

uint32 HyAtlas::GetHeight() const
{
	return m_uiHEIGHT;
}

uint32 HyAtlas::GetGfxApiHandle() const
{
	return m_uiGfxApiHandle;
}

bool HyAtlas::GetUvRect(uint32 uiChecksum, HyRectangle<float> &UVRectOut) const
{
	float fTexWidth = static_cast<float>(m_uiWIDTH);
	float fTexHeight = static_cast<float>(m_uiHEIGHT);

	const HyRectangle<int32> *pSrcRect = nullptr;
	std::map<uint32, HyRectangle<int32> *>::const_iterator iter = m_ChecksumMap.find(uiChecksum);
	if(iter != m_ChecksumMap.end())
		pSrcRect = iter->second;

	if(pSrcRect)
	{
		UVRectOut.left = static_cast<float>(pSrcRect->left) / fTexWidth;
		UVRectOut.top = static_cast<float>(pSrcRect->top) / fTexHeight;
		UVRectOut.right = static_cast<float>(pSrcRect->right) / fTexWidth;
		UVRectOut.bottom = static_cast<float>(pSrcRect->bottom) / fTexHeight;

		return true;
	}

	return false;
}

void HyAtlas::DeletePixelData()
{
	stbi_image_free(m_pPixelData);
	m_pPixelData = NULL;
}

void HyAtlas::OnLoadThread()
{
	m_csPixelData.Lock();

	if(GetLoadState() == HYLOADSTATE_Queued)
	{
		if(m_pPixelData)
		{
			m_csPixelData.Unlock();
			return;
		}

		int iWidth, iHeight, iNum8bitClrChannels;
		m_pPixelData = stbi_load(m_sFILE_PATH.c_str(), &iWidth, &iHeight, &iNum8bitClrChannels, 0);

		HyAssert(m_pPixelData != NULL, "HyAtlas failed to load image data");
	}

	m_csPixelData.Unlock();
}

void HyAtlas::OnRenderThread(IHyRenderer &rendererRef)
{
	m_csPixelData.Lock();
	if(GetLoadState() == HYLOADSTATE_Queued)
	{
		m_uiGfxApiHandle = rendererRef.AddTexture(m_uiNUM_8BIT_CHANNELS, m_uiWIDTH, m_uiHEIGHT, m_pPixelData);
		DeletePixelData();
	}
	else // GetLoadState() == HYLOADSTATE_Discarded
	{
		rendererRef.DeleteTextureArray(m_uiGfxApiHandle);
	}
	m_csPixelData.Unlock();
}
