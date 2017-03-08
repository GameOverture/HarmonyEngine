/**************************************************************************
 *	HyAtlasGroup.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAtlasGroup_h__
#define __HyAtlasGroup_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Loadables/IHyLoadableData.h"
#include "Threading/Threading.h"

#include "Utilities/HyMath.h"

class HyAssets;

class HyAtlas : public IHyLoadableData
{
	const std::string						m_sFILE_PATH;
	const uint32							m_uiINDEX;
	const uint32							m_uiWIDTH;
	const uint32							m_uiHEIGHT;
	const uint32							m_uiNUM_8BIT_CHANNELS;

	uint32									m_uiGfxApiHandle;

	const uint32							m_uiNUM_FRAMES;

	// The return value from the 'stb_image' loader is an 'unsigned char *' which points
	// to the pixel data. The pixel data consists of *y scanlines of *x pixels,
	// with each pixel consisting of N interleaved 8-bit components; the first
	// pixel pointed to is top-left-most in the image. There is no padding between
	// image scanlines or between pixels, regardless of format. The number of
	// components N is 'req_comp' if req_comp is non-zero, or *comp otherwise.
	// If req_comp is non-zero, *comp has the number of components that _would_
	// have been output otherwise. E.g. if you set req_comp to 4, you will always
	// get RGBA output, but you can check *comp to easily see if it's opaque.
	unsigned char *							m_pPixelData;

	HyRectangle<int32> *					m_pFrames;
	std::map<uint32, HyRectangle<int32> *>	m_ChecksumMap;

	BasicSection							m_csPixelData;

public:
	HyAtlas(std::string sFilePath, uint32 uiIndex, uint32 uiWidth, uint32 uiHeight, uint32 uiNumClrChannels, jsonxx::Array &srcFramesArrayRef);
	~HyAtlas();

	uint32 GetIndex() const;
	uint32 GetWidth() const;
	uint32 GetHeight() const;
	uint32 GetGfxApiHandle() const;
	bool GetUvRect(uint32 uiChecksum, HyRectangle<float> &UVRectOut) const;

	void DeletePixelData();

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

//class HyAtlasGroup
//{
//	friend class HyAtlasContainer;
//
//	const uint32				m_uiLOADGROUPID;
//	const uint32				m_uiWIDTH;
//	const uint32				m_uiHEIGHT;
//	
//
//	HyAtlas *					m_pAtlases;
//	const uint32				m_uiNUM_ATLASES;
//
//public:
//	HyAtlasGroup(std::string sTexturePath, uint32 uiLoadGroupId, uint32 uiWidth, uint32 uiHeight, uint32 uiNumClrChannels, jsonxx::Array &texturesArrayRef);
//	~HyAtlasGroup();
//
//	uint32 GetGfxApiHandle(uint32 uiAtlasGroupTextureIndex);
//	uint32 GetActualGfxApiTextureIndex(uint32 uiAtlasGroupTextureIndex);
//
//	uint32 GetId() const;
//	uint32 GetNumColorChannels() const;
//	uint32 GetWidth() const;
//	uint32 GetHeight() const;
//	uint32 GetNumTextures() const;
//
//	void GetUvRect(uint32 uiChecksum, uint32 &uiTextureIndexOut, HyRectangle<float> &UVRectOut) const;
//};

#endif /* __HyAtlasGroup_h__ */
