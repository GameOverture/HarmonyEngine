/**************************************************************************
 *	HyFileAtlas.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyFileAtlas_h__
#define HyFileAtlas_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/IHyFile.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyJson.h"

class HyAssets;

class HyFileAtlas : public IHyFile
{
	const uint32							m_uiINDEX_IN_BANK;
	uint32									m_uiWidth;
	uint32									m_uiHeight;

	const HyTextureInfo						m_TextureInfo;
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
	HyFileAtlas(std::string sFileName, uint32 uiBankId, uint32 uiIndexInBank, uint32 uiManifestIndex, HyJsonObj textureObj);
	HyFileAtlas(HyExtrinsicFileHandle hGivenHandle, std::string sFileName, HyTextureInfo textureInfo);

	~HyFileAtlas();

	uint32 GetIndexInBank() const;
	
	uint32 GetWidth() const;
	uint32 GetHeight() const;
	HyTextureHandle GetTextureHandle() const;
	bool GetUvRect(uint32 uiChecksum, HyRectangle<float> &UVRectOut) const;

	void DeletePixelData();

	virtual std::string AssetTypeName() override;
	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;

	virtual std::string GetAssetInfo() override;

protected:
	unsigned char *LoadAstc(std::string sAtlasFilePath, uint32 &uiPixelDataSizeOut);
};

#endif /* HyFileAtlas_h__ */
