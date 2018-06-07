/**************************************************************************
 *	HyAtlas.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAtlas_h__
#define HyAtlas_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/IHyLoadableData.h"
#include "Utilities/HyMath.h"

class HyAssets;

class HyAtlas : public IHyLoadableData
{
	const std::string						m_sFILE_PATH;
	const uint32							m_uiATLAS_GROUP_ID;
	const uint32							m_uiINDEX_IN_GROUP;
	const uint32							m_uiMASTER_INDEX;
	const uint32							m_uiWIDTH;
	const uint32							m_uiHEIGHT;
	const HyTextureFormat					m_eTEXTURE_FORMAT;

	HyTextureHandle							m_hTextureHandle;

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
	uint32									m_uiPixelDataSize;

	HyRectangle<int32> *					m_pFrames;
	std::map<uint32, HyRectangle<int32> *>	m_ChecksumMap;

	std::mutex								m_Mutex_PixelData;

public:
	HyAtlas(std::string sFilePath, uint32 uiAtlasGroupId, uint32 uiIndexInGroup, uint32 uiMasterIndex, uint32 uiWidth, uint32 uiHeight, HyTextureFormat eTextureFormat, jsonxx::Array &srcFramesArrayRef);
	~HyAtlas();

	uint32 GetAtlasGroupId() const;
	uint32 GetIndexInGroup() const;
	uint32 GetMasterIndex() const;
	uint32 GetWidth() const;
	uint32 GetHeight() const;
	HyTextureHandle GetTextureHandle() const;
	bool GetUvRect(uint32 uiChecksum, HyRectangle<float> &UVRectOut) const;

	void DeletePixelData();

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

#endif /* HyAtlas_h__ */
