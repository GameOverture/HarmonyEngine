/**************************************************************************
 *	HyTexture.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FileIO/Atlas/HyAtlas.h"
#include "FileIO/stb_image.h"
#include "Utilities/HyStrManip.h"
#include "Renderer/IHyRenderer.h"

HyAtlas::HyAtlas(const std::string &sPath, HyRectangle *pSrcRects /*= NULL*/, uint32 uiNumRects /*= 0*/) :	m_ksPath(sPath),
																												m_iWidth(0),
																												m_iHeight(0),
																												m_iNum8bitClrChannels(0),
																												m_pPixelData(NULL),
																												m_uiId(0),
																												m_pSrcRects(pSrcRects),
																												m_uiNumRects(uiNumRects)
{
	m_pPixelData = stbi_load(m_ksPath.c_str(), &m_iWidth, &m_iHeight, &m_iNum8bitClrChannels, 0);
	HyAssert(m_pPixelData != NULL, "HyTexture failed to load image data");
}

//HyTexture::HyTexture(const std::string &sPath, int32 iWidth, int32 iHeight, int32 iNum8bitClrChannels, unsigned char *pPixelData) : m_ksPath(sPath),
//																																	m_iWidth(iWidth),
//																																	m_iHeight(iHeight),
//																																	m_iNum8bitClrChannels(iNum8bitClrChannels),
//																																	m_uiId(0),
//																																	m_bLoadedWithStbi(false),
//																																	m_pSrcRects(NULL),
//																																	m_uiNumRects(0)
//{
//	HyAssert(m_pPixelData != NULL, "HyTexture failed to load image data");
//
//	uint32 uiDataSizeBytes = m_iWidth * m_iHeight * m_iNum8bitClrChannels;
//	m_pPixelData = new unsigned char[uiDataSizeBytes];
//	memcpy(m_pPixelData, pPixelData, uiDataSizeBytes);
//}

HyAtlas::~HyAtlas(void)
{
	DeletePixelData();
}

// To be invoked on the render thread
void HyAtlas::Upload(IHyRenderer &gfxApi)
{
	m_uiId = gfxApi.AddTexture(m_iNum8bitClrChannels, m_iWidth, m_iHeight, m_pPixelData);
	DeletePixelData();
}

void HyAtlas::DeletePixelData()
{
	stbi_image_free(m_pPixelData);
	m_pPixelData = NULL;
}
