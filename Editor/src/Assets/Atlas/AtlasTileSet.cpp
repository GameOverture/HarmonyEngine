/**************************************************************************
 *	AtlasTileSet.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasTileSet.h"

AtlasTileSet::AtlasTileSet(IManagerModel &modelRef,
						   QUuid uuid,
						   quint32 uiChecksum,
						   quint32 uiBankId,
						   QString sName,
						   HyTextureInfo texInfo,
						   quint16 uiW,
						   quint16 uiH,
						   quint16 uiX,
						   quint16 uiY,
						   int iTextureIndex,
						   const FileDataPair &initFileDataRef,
						   bool bIsPendingSave,
						   uint uiErrors) :
	AtlasFrame(ITEM_AtlasTileSet, modelRef, ITEM_AtlasTileSet, uuid, uiChecksum, uiBankId, sName, 0, 0, 0, 0, texInfo, uiW, uiH, uiX, uiY, iTextureIndex, uiErrors),
	m_FileDataPair(initFileDataRef),
	m_bExistencePendingSave(bIsPendingSave)
{
}

AtlasTileSet::~AtlasTileSet()
{
}

int AtlasTileSet::GetNumTiles() const
{
	return m_TileMap.count();
}

QSize AtlasTileSet::GetTileSize() const
{
	return m_TileSize;
}

QString AtlasTileSet::GetTileSetInfo() const
{
	QString sInfo;
	if(GetNumTiles() == 0)
		sInfo += "Empty";
	else
	{
		sInfo += QString::number(GetNumTiles()) + " Tiles (";
		sInfo += QString::number(m_TileSize.width()) + "x" + QString::number(m_TileSize.height()) + ")";
	}

	return sInfo;
}

void AtlasTileSet::GetLatestFileData(FileDataPair &fileDataPairOut) const
{
	fileDataPairOut = m_FileDataPair;

	// TODO: Get current member data and write to fileDataPairOut

	//// Assemble stateArray
	//QJsonArray metaStateArray;
	//QJsonArray dataStateArray;
	//for(int i = 0; i < m_pModel->GetNumStates(); ++i)
	//{
	//	FileDataPair stateFileData = m_pModel->GetStateFileData(i);
	//	metaStateArray.append(stateFileData.m_Meta);
	//	dataStateArray.append(stateFileData.m_Data);
	//}
	//itemFileDataOut.m_Meta["stateArray"] = metaStateArray;
	//itemFileDataOut.m_Data["stateArray"] = dataStateArray;

	//// Replace camera data if a draw instance is instantiated
	//if(m_pDraw)
	//{
	//	glm::vec2 ptCamPos(0.0f, 0.0f);
	//	float fCamZoom = 1.0f;
	//	m_pDraw->GetCameraInfo(ptCamPos, fCamZoom);

	//	QJsonArray cameraPosArray;
	//	cameraPosArray.append(ptCamPos.x);
	//	cameraPosArray.append(ptCamPos.y);
	//	itemFileDataOut.m_Meta["CameraPos"] = cameraPosArray;
	//	itemFileDataOut.m_Meta["CameraZoom"] = fCamZoom;

	//	itemFileDataOut.m_Meta["guideHorzArray"] = m_pDraw->GetGuideArray(HYORIENT_Horizontal);
	//	itemFileDataOut.m_Meta["guideVertArray"] = m_pDraw->GetGuideArray(HYORIENT_Vertical);
	//}

	//// Assemble item specific data
	//m_pModel->InsertItemSpecificData(itemFileDataOut);
}

void AtlasTileSet::GetSavedFileData(FileDataPair &itemFileDataOut) const
{
	itemFileDataOut = m_FileDataPair;
}

bool AtlasTileSet::Save(bool bWriteToDisk)
{
	GetLatestFileData(m_FileDataPair);

	// Register the item's file data into the project
	GetProject().SaveItemData(m_eTYPE, GetName(true), m_ItemFileData, bWriteToDisk);

	m_bExistencePendingSave = false;
	m_pUndoStack->setClean();

	if(bWriteToDisk)
	{
		if(m_pWidget)
			m_pWidget->update();
		MainWindow::GetExplorerWidget().update();
		MainWindow::GetAuxWidget(AUXTAB_DopeSheet)->update();
	}

	return true;
}

bool AtlasTileSet::IsExistencePendingSave() const
{
}

bool AtlasTileSet::IsSaveClean() const
{
}

void AtlasTileSet::DiscardChanges()
{
}

/*virtual*/ void AtlasTileSet::InsertUniqueJson(QJsonObject &frameObj) /*override*/
{
	frameObj.insert("subAtlasType", QJsonValue(HyGlobal::ItemName(m_eSubAtlasType, false)));
	frameObj.insert("width", QJsonValue(GetSize().width()));
	frameObj.insert("height", QJsonValue(GetSize().height()));
	frameObj.insert("textureIndex", QJsonValue(GetTextureIndex()));
	frameObj.insert("textureInfo", QJsonValue(static_cast<qint64>(m_TexInfo.GetBucketId())));
	frameObj.insert("x", QJsonValue(GetX()));
	frameObj.insert("y", QJsonValue(GetY()));
}
