/**************************************************************************
 *	HyFileTileSet.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyFileTileSet.h"
#include "Assets/HyAssets.h"
#include "HyEngine.h"
#include "Utilities/Crc32.h"

HyFileTileSet::HyFileTileSet(std::string sTileSetName, uint32 uiManifestIndex, HyJsonObj tileSetObj) :
	IHyFile(HYFILE_TileSet, sTileSetName, 0, uiManifestIndex),
	m_hCrcHandle(HyAssets::CalcTileSetHandle(sTileSetName)),
	m_bUseDescriptorEx(tileSetObj["isEx"].GetBool()),
	m_iSubAtlasCols(tileSetObj["subAtlasCols"].GetInt()),
	m_iSubAtlasRows(tileSetObj["subAtlasRows"].GetInt()),
	m_iNumDescriptorTexels(tileSetObj["numDescriptorTexels"].GetInt()),
	m_pDescriptorTexelData(nullptr),
	m_uiDescriptorSize(0),
	m_hDescriptorBufferPair(HY_UNUSED_HANDLE, HY_UNUSED_HANDLE),
	m_pDescriptorExTexelData(nullptr),
	m_uiDescriptorExSize(0),
	m_hDescriptorExBufferPair(HY_UNUSED_HANDLE, HY_UNUSED_HANDLE)
{
}

HyFileTileSet::~HyFileTileSet()
{
	DeleteTexelData();
}

HyTileSetHandle HyFileTileSet::GetHandle() const
{
	return m_hCrcHandle;
}

const std::string &HyFileTileSet::GetTileSetName() const
{
	return m_sFILE_NAME;
}

int32 HyFileTileSet::GetNumSubAtlasColumns() const
{
	return m_iSubAtlasCols;
}

int32 HyFileTileSet::GetNumSubAtlasRows() const
{
	return m_iSubAtlasRows;
}

void HyFileTileSet::DeleteTexelData()
{
	delete[] m_pDescriptorTexelData;
	m_pDescriptorTexelData = nullptr;
	m_uiDescriptorSize = 0;

	delete[] m_pDescriptorExTexelData;
	m_pDescriptorExTexelData = nullptr;
	m_uiDescriptorExSize = 0;
}

/*virtual*/ std::string HyFileTileSet::AssetTypeName() /*override*/
{
	return "TileSet";
}

/*virtual*/ void HyFileTileSet::OnLoadThread() /*override*/
{
	m_Mutex_PixelData.lock();

	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		if(m_pDescriptorTexelData && (m_bUseDescriptorEx == false || m_pDescriptorExTexelData))
		{
			m_Mutex_PixelData.unlock();
			return;
		}

		DeleteTexelData();

		std::string sAtlasFilePath = HyEngine::DataDir() + HYASSETS_TileSetDir + m_sFILE_NAME + HyImageInfo::GetExt(HYIMAGE_HYTX);
		HyImageInfo imageInfo = GenerateImageInfo();
		m_pDescriptorTexelData = HyIO::ReadImage(sAtlasFilePath, imageInfo, m_uiDescriptorSize);

		if(m_bUseDescriptorEx)
		{
			std::string sAtlasFilePathEx = HyEngine::DataDir() + HYASSETS_TileSetDir + m_sFILE_NAME + "Ex" + HyImageInfo::GetExt(HYIMAGE_HYTX);
			HyImageInfo imageInfoEx = GenerateImageInfo();
			m_pDescriptorExTexelData = HyIO::ReadImage(sAtlasFilePathEx, imageInfoEx, m_uiDescriptorExSize);
		}
	}

	m_Mutex_PixelData.unlock();
}

/*virtual*/ void HyFileTileSet::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
	m_Mutex_PixelData.lock();
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		m_hDescriptorBufferPair = rendererRef.AddTextureBufferObject(GenerateTextureInfo(), m_pDescriptorTexelData, m_uiDescriptorSize);
		if(m_bUseDescriptorEx)
			m_hDescriptorExBufferPair = rendererRef.AddTextureBufferObject(GenerateTextureInfo(), m_pDescriptorExTexelData, m_uiDescriptorExSize);

		DeleteTexelData();
	}
	else // GetLoadableState() == HYLOADSTATE_Discarded
	{
		rendererRef.DeleteTextureBufferObject(m_hDescriptorBufferPair);
		if(m_bUseDescriptorEx)
			rendererRef.DeleteTextureBufferObject(m_hDescriptorExBufferPair);
	}
	m_Mutex_PixelData.unlock();
}

/*virtual*/ std::string HyFileTileSet::GetAssetInfo() /*override*/
{
	std::stringstream ss;
	ss << m_sFILE_NAME;
	if(m_bUseDescriptorEx)
		ss << " + " + m_sFILE_NAME + "Ex";
	
	return ss.str();
}

HyImageInfo HyFileTileSet::GenerateImageInfo() const
{
	return HyImageInfo(m_iNumDescriptorTexels, 1, HYIMAGE_HYTX, false, 4, HYTEXFORMAT_UINT16, 0);
}

HyTextureIn HyFileTileSet::GenerateTextureInfo() const
{
	return HyTextureIn(HYTEXFILTER_NEAREST, HYTEXWRAP_Repeat, 4, HYTEXFORMAT_UINT16, 0);
}
