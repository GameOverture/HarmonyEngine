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
#include "Assets/Files/IHyFileData.h"
#include "Utilities/HyMath.h"

class HyAssets;

 // NOTE: Can't use std::bitset because the number of bits needed is not known at compile time
class HyAtlasIndices
{
	friend class HyAssets;

	uint32 *						m_pIndexFlags;				// Each bit represents the respective texture index
	static int32					sm_iIndexFlagsArraySize;	// How many 'uint32' are needed to account for every texture index in 'm_pIndexFlags'

public:
	HyAtlasIndices();
	~HyAtlasIndices();

	bool IsSet(uint32 uiAtlasIndex) const;
	bool IsSet(const HyAtlasIndices &otherRef) const;
	void Set(uint32 uiAtlasIndex);
	void Clear(uint32 uiAtlasIndex);

	bool IsEmpty() const;
};

class HyAtlas : public IHyFileData
{
	const uint32							m_uiBANK_ID;
	const uint32							m_uiINDEX_IN_GROUP;
	const uint32							m_uiMASTER_INDEX;
	const uint32							m_uiWIDTH;
	const uint32							m_uiHEIGHT;
	const HyTextureFormat					m_eTEXTURE_FORMAT;
	const HyTextureFiltering				m_eTEXTURE_FILTERING;

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
	HyAtlas(std::string sFilePath,
			uint32 uiBankId,
			uint32 uiIndexInGroup,
			uint32 uiMasterIndex,
			uint32 uiWidth,
			uint32 uiHeight,
			HyTextureFormat eTextureFormat,
			HyTextureFiltering eTextureFiltering,
			jsonxx::Array &srcFramesArrayRef);

	~HyAtlas();

	uint32 GetBankId() const;
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
