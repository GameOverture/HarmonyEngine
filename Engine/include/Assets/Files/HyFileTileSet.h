/**************************************************************************
 *	HyFileTileSet.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyFileTileSet_h__
#define HyFileTileSet_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/IHyFile.h"
#include "Utilities/HyJson.h"

class HyAssets;

class HyFileTileSet : public IHyFile
{
	HyTileSetHandle								m_hCrcHandle;

	bool										m_bUseDescriptorEx;

	int32										m_iColumns;
	int32										m_iRows;

	HyImageInfo									m_DescriptorImageInfo;
	uint8 *										m_pDescriptorTexelData;
	uint32										m_uiDescriptorSize;
	std::pair<HyTextureHandle, HyTextureHandle>	m_hDescriptorBufferPair;

	HyImageInfo									m_DescriptorExImageInfo;
	uint8 *										m_pDescriptorExTexelData;
	uint32										m_uiDescriptorExSize;
	std::pair<HyTextureHandle, HyTextureHandle>	m_hDescriptorExBufferPair;
												
	std::mutex									m_Mutex_PixelData;

public:
	HyFileTileSet(std::string sTileSetName, uint32 uiManifestIndex, HyJsonObj tileSetObj);
	~HyFileTileSet();
	
	HyTileSetHandle GetHandle() const;
	const std::string &GetTileSetName() const;

	int32 GetNumSubAtlasColumns() const;
	int32 GetNumSubAtlasRows() const;

	void DeleteTexelData();

	virtual std::string AssetTypeName() override;
	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;

	virtual std::string GetAssetInfo() override;
};

#endif /* HyFileTileSet_h__ */
