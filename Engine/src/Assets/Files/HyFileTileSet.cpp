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
	m_iColumns(tileSetObj["cols"].GetInt()),
	m_iRows(tileSetObj["rows"].GetInt()),
	m_DescriptorTextureInfo(tileSetObj["descriptorTextureInfo"].GetUint()),
	m_pDescriptorTexelData(nullptr),
	m_uiDescriptorSize(0),
	m_hDescriptorBufferPair(HY_UNUSED_HANDLE, HY_UNUSED_HANDLE),
	m_DescriptorExTextureInfo(tileSetObj["descriptorExTextureInfo"].GetUint()),
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
	return m_iColumns;
}

int32 HyFileTileSet::GetNumSubAtlasRows() const
{
	return m_iRows;
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

		std::string sAtlasFilePath = HyEngine::DataDir() + HYASSETS_TileSetDir + m_sFILE_NAME + HYASSETS_TileSetExt;
		std::string sMagicNumber(&HYASSETS_TileSetExt[1]);
		HyIO::ParseRawTextureFile(sAtlasFilePath, sMagicNumber, m_DescriptorTextureInfo, m_pDescriptorTexelData, m_uiDescriptorSize);

		if(m_bUseDescriptorEx)
		{
			std::string sAtlasFilePathEx = HyEngine::DataDir() + HYASSETS_TileSetDir + m_sFILE_NAME + HYASSETS_TileSetExExt;
			std::string sExMagicNumber(&HYASSETS_TileSetExExt[1]);
			HyIO::ParseRawTextureFile(sAtlasFilePathEx, sExMagicNumber, m_DescriptorExTextureInfo, m_pDescriptorExTexelData, m_uiDescriptorExSize);
		}
	}

	m_Mutex_PixelData.unlock();
}

/*virtual*/ void HyFileTileSet::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
	m_Mutex_PixelData.lock();
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		m_hDescriptorBufferPair = rendererRef.AddTextureBufferObject(m_DescriptorTextureInfo, m_pDescriptorTexelData, m_uiDescriptorSize);
		if(m_bUseDescriptorEx)
			m_hDescriptorExBufferPair = rendererRef.AddTextureBufferObject(m_DescriptorExTextureInfo, m_pDescriptorExTexelData, m_uiDescriptorExSize);

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
	ss << m_sFILE_NAME + HYASSETS_TileSetExt;
	if(m_bUseDescriptorEx)
		ss << " + " + m_sFILE_NAME + HYASSETS_TileSetExExt;
	
	return ss.str();
}
