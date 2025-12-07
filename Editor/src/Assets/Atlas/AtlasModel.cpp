/**************************************************************************
 *	AtlasModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasModel.h"
#include "AtlasDraw.h"
#include "AtlasTileSet.h"
#include "Project.h"
#include "AtlasRepackThread.h"
#include "MainWindow.h"
#include "DlgAtlasGroupSettings.h"
#include "DlgSliceSpriteSheet.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMimeData>

AtlasModel::AtlasModel(Project &projRef) :
	IManagerModel(projRef, ASSETMAN_Atlases),
	m_DefaultTextureInfo(HYTEXFILTER_BILINEAR, HYTEXTURE_Uncompressed, 4, 0)
{
	QFile tileSetMetaFile(m_MetaDir.absoluteFilePath(HyGlobal::ItemName(ITEM_AtlasTileSet, true) % HYGUIPATH_MetaExt));
	QFile tileSetDataFile(m_DataDir.absoluteFilePath(HyGlobal::ItemName(ITEM_AtlasTileSet, true) % HYGUIPATH_DataExt));
	if(tileSetMetaFile.exists() == false && tileSetDataFile.exists() == false)
		WriteTileSetsToDisk();

	if(tileSetMetaFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QJsonDocument tileSetMetaDoc = QJsonDocument::fromJson(tileSetMetaFile.readAll());
		m_TileSetsDataPair.m_Meta = tileSetMetaDoc.object();
	}
	else
		HyGuiLog("Failed to create or open tile set meta file: " + tileSetMetaFile.fileName(), LOGTYPE_Error);

	if(tileSetDataFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QJsonDocument tileSetDataDoc = QJsonDocument::fromJson(tileSetDataFile.readAll());
		m_TileSetsDataPair.m_Data = tileSetDataDoc.object();
	}
	else
		HyGuiLog("Failed to create or open tile set data file: " + tileSetDataFile.fileName(), LOGTYPE_Error);
}

/*virtual*/ AtlasModel::~AtlasModel()
{

}

QFileInfoList AtlasModel::GetExistingTextureInfoList(uint uiBankIndex) const
{
	QDir bankDir(m_BanksModel.GetBank(uiBankIndex)->m_sAbsPath);
	return bankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
}

int AtlasModel::GetNumTextures(uint uiBankIndex) const
{
	return GetExistingTextureInfoList(uiBankIndex).size();
}

QSize AtlasModel::GetMaxAtlasDimensions(uint uiBankIndex) const
{
	int iWidth = m_BanksModel.GetBank(uiBankIndex)->m_MetaObj["maxWidth"].toInt();
	int iHeight = m_BanksModel.GetBank(uiBankIndex)->m_MetaObj["maxHeight"].toInt();
	
	return QSize(iWidth, iHeight);
}

QSize AtlasModel::GetTextureSize(uint uiBankIndex, int iTextureIndex) const
{
	QJsonArray textureSizesArray = m_BanksModel.GetBank(uiBankIndex)->m_MetaObj["textureSizes"].toArray();
	QJsonArray texSizeArray = textureSizesArray[iTextureIndex].toArray();

	return QSize(texSizeArray[0].toInt(), texSizeArray[1].toInt());
}

bool AtlasModel::IsImageValid(QImage &image, quint32 uiBankId) const
{
	return IsImageValid(image.width(), image.height(), uiBankId);
}

bool AtlasModel::IsImageValid(int iWidth, int iHeight, quint32 uiBankId) const
{
	uint uiBankIndex = GetBankIndexFromBankId(uiBankId);
	return IsImageValid(iWidth, iHeight, m_BanksModel.GetBank(uiBankIndex)->m_MetaObj);
}

bool AtlasModel::IsImageValid(int iWidth, int iHeight, const QJsonObject &atlasSettings) const
{
	int iMarginWidth =  atlasSettings["sbFrameMarginLeft"].toInt();
	iMarginWidth +=     atlasSettings["sbFrameMarginRight"].toInt();
	int iMarginHeight = atlasSettings["sbFrameMarginBottom"].toInt();
	iMarginHeight +=    atlasSettings["sbFrameMarginTop"].toInt();

	QSize atlasMargins(iMarginWidth, iMarginHeight);
	QSize atlasDimensions(atlasSettings["maxWidth"].toInt(), atlasSettings["maxHeight"].toInt());

	if(iWidth >= (atlasDimensions.width() - atlasMargins.width()) ||
		iHeight >= (atlasDimensions.height() - atlasMargins.height()))
	{
		return false;
	}

	return true;
}

AtlasFrame *AtlasModel::FindFrame(quint32 uiChecksum, quint32 uiBankId) const
{
	uint uiBankIndex = GetBankIndexFromBankId(uiBankId);
	const QList<IAssetItemData *> &assetListRef = m_BanksModel.GetBank(uiBankIndex)->m_AssetList;

	for(int i = 0; i < assetListRef.size(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetListRef[i]);
		if(pFrame->GetChecksum() == uiChecksum)
			return pFrame;
	}

	return nullptr;
}

AtlasFrame *AtlasModel::FindFrame(const QUuid &itemUuidRef, quint32 uiBankId) const
{
	uint uiBankIndex = GetBankIndexFromBankId(uiBankId);
	const QList<IAssetItemData *> &assetListRef = m_BanksModel.GetBank(uiBankIndex)->m_AssetList;

	for(int i = 0; i < assetListRef.size(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetListRef[i]);
		if(pFrame->GetUuid() == itemUuidRef)
			return pFrame;
	}

	return nullptr;
}

AtlasFrame *AtlasModel::GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiBankIndex, ItemType eSubAtlasType)
{
	if(IsImageValid(newImage, m_BanksModel.GetBank(uiBankIndex)->GetId()) == false)
		return nullptr;

	// This will also create a meta image and register asset
	AtlasFrame *pFrame = ImportImage(sName, newImage, m_BanksModel.GetBank(uiBankIndex)->GetId(), eSubAtlasType, QUuid::createUuid());

	InsertTreeItem(pItem->GetProject(), pFrame, nullptr);

	// This retrieves the newly created AtlasFrame and links it to its ProjectItemData
	QList<TreeModelItemData *> returnList = pItem->GetProject().IncrementDependencies(pItem, QList<QUuid>() << pFrame->GetUuid());
	if(returnList.empty() == false)
	{
		AddAssetsToRepack(m_BanksModel.GetBank(uiBankIndex), static_cast<AtlasFrame *>(returnList[0]));
		return static_cast<AtlasFrame *>(returnList[0]);
	}

	return nullptr;
}

bool AtlasModel::ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, ItemType eSubAtlasType)
{
	if(IsImageValid(newImage, pFrame->GetBankId()) == false)
		return false;

	QSet<int> textureIndexToReplaceSet;
	if(pFrame->GetTextureIndex() >= 0)
		textureIndexToReplaceSet.insert(pFrame->GetTextureIndex());

	// First remove the frame from the map
	if(RemoveLookup(pFrame))
		pFrame->DeleteMetaFile();

	// Determine the new checksum into the map
	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.sizeInBytes());
	pFrame->ReplaceImage(sName, uiChecksum, newImage, eSubAtlasType, m_MetaDir);

	// Re-enter the frame into the map
	RegisterAsset(pFrame);

	uint uiBankIndex = GetBankIndexFromBankId(pFrame->GetBankId());
	AddAssetsToRepack(m_BanksModel.GetBank(uiBankIndex), pFrame);

	return true;
}

AtlasTileSet *AtlasModel::GenerateTileSet(QString sName, TreeModelItemData *pParentTreeItemData, quint32 uiBankId)
{
	AtlasTileSet *pNewTileSet = new AtlasTileSet(*this,
												 QUuid::createUuid(),
												 0,
												 uiBankId,
												 sName,
												 m_DefaultTextureInfo,
												 0,
												 0,
												 -1,
												-1,
												-1,
												FileDataPair(),
												true,
												0);

	RegisterAsset(pNewTileSet);

	TreeModelItem *pTreeParent = pParentTreeItemData ? GetItem(FindIndex<TreeModelItemData *>(pParentTreeItemData, 0)) : nullptr;
	InsertTreeItem(m_ProjectRef, pNewTileSet, pTreeParent);

	return pNewTileSet;
}

void AtlasModel::SaveTileSet(QUuid tileSetUuid, const FileDataPair &tileSetFileDataPairRef, bool bWriteToDisk)
{
	QString sTileSetUuid = tileSetUuid.toString(QUuid::WithoutBraces);
	m_TileSetsDataPair.m_Data.insert(sTileSetUuid, tileSetFileDataPairRef.m_Data);
	m_TileSetsDataPair.m_Meta.insert(sTileSetUuid, tileSetFileDataPairRef.m_Meta);
		
	if(bWriteToDisk)
	{
		WriteTileSetsToDisk();
		FlushRepack();
	}
}

void AtlasModel::WriteTileSetsToDisk()
{
	// Save Meta Data
	QFile tileSetMetaFile(m_MetaDir.absoluteFilePath(HyGlobal::ItemName(ITEM_AtlasTileSet, true) % HYGUIPATH_MetaExt));
	if(!tileSetMetaFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
		HyGuiLog("Couldn't open TileSet meta file for writing: " % tileSetMetaFile.errorString(), LOGTYPE_Error);
	else
	{
		m_TileSetsDataPair.m_Meta.insert("$fileVersion", HYGUI_FILE_VERSION);

		QJsonDocument metaDoc;
		metaDoc.setObject(m_TileSetsDataPair.m_Meta);

//#ifdef HYGUI_UseBinaryMetaFiles
//		qint64 iBytesWritten = tileSetMetaFile.write(metaDoc.toBinaryData());
//#else
		qint64 iBytesWritten = tileSetMetaFile.write(metaDoc.toJson());
//#endif
		if(0 == iBytesWritten || -1 == iBytesWritten)
			HyGuiLog("Could not write to meta data file: " % tileSetMetaFile.errorString(), LOGTYPE_Error);

		tileSetMetaFile.close();
	}

	// Save Runtime Data
	QFile tileSetDataFile(m_DataDir.absoluteFilePath(HyGlobal::ItemName(ITEM_AtlasTileSet, true) % HYGUIPATH_DataExt));
	if(!tileSetDataFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
		HyGuiLog("Couldn't open TileSet data file for writing: " % tileSetDataFile.errorString(), LOGTYPE_Error);
	else
	{
		m_TileSetsDataPair.m_Data.insert("$fileVersion", HYGUI_FILE_VERSION);

		QJsonDocument dataDoc;
		dataDoc.setObject(m_TileSetsDataPair.m_Data);
		qint64 iBytesWritten = tileSetDataFile.write(dataDoc.toJson());
		if(0 == iBytesWritten || -1 == iBytesWritten)
			HyGuiLog("Could not write to data file: " % tileSetDataFile.errorString(), LOGTYPE_Error);
		tileSetDataFile.close();
	}
}

void AtlasModel::OnSliceSprite(const QModelIndex &indexDestination)
{
	DlgSliceSpriteSheet *pDlg = new DlgSliceSpriteSheet();
	if(QDialog::Accepted == pDlg->exec())
	{
	}
}

/*virtual*/ QString AtlasModel::OnBankInfo(uint uiBankIndex) /*override*/
{
	QString sInfo = "Num Textures: " % QString::number(GetNumTextures(uiBankIndex)) % " | " %
		"(" % QString::number(GetMaxAtlasDimensions(uiBankIndex).width()) % "x" % QString::number(GetMaxAtlasDimensions(uiBankIndex).height()) % ")";

	return sInfo;
}

/*virtual*/ bool AtlasModel::OnBankSettingsDlg(uint uiBankIndex) /*override*/
{
	QList<IAssetItemData *> assetList = m_BanksModel.GetBank(uiBankIndex)->m_AssetList;
	bool bBankHasAssets = assetList.size() > 0;
	bool bAccepted = false;
	DlgAtlasGroupSettings *pDlg = new DlgAtlasGroupSettings(bBankHasAssets, m_BanksModel.GetBank(uiBankIndex)->m_MetaObj);
	if(QDialog::Accepted == pDlg->exec())
	{
		// Ensure that all current images in atlas group aren't larger than the new atlas itself
		QJsonObject newPackerSettings = m_BanksModel.GetBank(uiBankIndex)->m_MetaObj;
		pDlg->ApplyCurrentSettingsToObj(newPackerSettings);
	
		bool bPackIsValid = true;
		for(int i = 0; i < assetList.size(); ++i)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[i]);
			if(IsImageValid(pFrame->GetSize().width(), pFrame->GetSize().height(), newPackerSettings) == false)
			{
				HyGuiLog("Could not save atlas bank settings because image '" % assetList[i]->GetName() % "' would no longer fit on the atlas", LOGTYPE_Warning);
				bPackIsValid = false;
				break;
			}
		}
	
		if(bPackIsValid)
		{
			BankData *pBankData = m_BanksModel.GetBank(uiBankIndex);
			pBankData->m_MetaObj = newPackerSettings;
	
			if(pDlg->IsSettingsDirty() && bBankHasAssets)
			{
				AddAssetsToRepack(pBankData);
				FlushRepack();
			}
			else
			{
				SaveMeta();
				SaveData();
			}
	
			bAccepted = true;
		}
	}
	
	delete pDlg;
	//RefreshInfo();
	
	return bAccepted;
}

/*virtual*/ QStringList AtlasModel::GetSupportedFileExtList() const /*override*/
{
	return QStringList() << ".png";
}

/*virtual*/ void AtlasModel::OnInit() /*override*/
{
	// Create data manifest file if one doesn't exist
	QFile manifestFile(m_DataDir.absoluteFilePath(HyGlobal::AssetName(m_eASSET_TYPE) % HYGUIPATH_DataExt));
	if(manifestFile.exists() == false)
	{
		SaveMeta();
		SaveData();
	}
}

/*virtual*/ void AtlasModel::OnCreateNewBank(QJsonObject &newMetaBankObjRef) /*override*/
{
	newMetaBankObjRef.insert("cmbSortOrder", 0);
	newMetaBankObjRef.insert("sbFrameMarginTop", 0);
	newMetaBankObjRef.insert("sbFrameMarginLeft", 0);
	newMetaBankObjRef.insert("sbFrameMarginRight", 1);
	newMetaBankObjRef.insert("sbFrameMarginBottom", 1);
	newMetaBankObjRef.insert("maxWidth", 4096);
	newMetaBankObjRef.insert("maxHeight", 4096);
	newMetaBankObjRef.insert("cmbHeuristic", 1);
	newMetaBankObjRef.insert("squareTexturesOnly", false);
	newMetaBankObjRef.insert("cropUnusedSpace", true);
	newMetaBankObjRef.insert("aggressiveResizing", false);
	newMetaBankObjRef.insert("minimumFillRate", 80);
}

/*virtual*/ IAssetItemData *AtlasModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
{
	ItemType eAssetItemType = HyGlobal::GetTypeFromString(metaObj["itemType"].toString());
	if(eAssetItemType == ITEM_AtlasFrame)
	{
		AtlasFrame *pNewFrame = new AtlasFrame(eAssetItemType,
											   *this,
											   HyGlobal::GetTypeFromString(metaObj["subAtlasType"].toString()),
											   QUuid(metaObj["assetUUID"].toString()),
											   JSONOBJ_TOINT(metaObj, "checksum"),
											   JSONOBJ_TOINT(metaObj, "bankId"),
											   metaObj["name"].toString(),
											   metaObj["cropLeft"].toInt(),
											   metaObj["cropTop"].toInt(),
											   metaObj["cropRight"].toInt(),
											   metaObj["cropBottom"].toInt(),
											   HyTextureInfo(JSONOBJ_TOINT(metaObj, "textureInfo")),
											   metaObj["width"].toInt(),
											   metaObj["height"].toInt(),
											   metaObj["x"].toInt(),
											   metaObj["y"].toInt(),
											   metaObj["textureIndex"].toInt(),
											   metaObj["errors"].toInt(0));

		return pNewFrame;
	}
	else if(eAssetItemType == ITEM_AtlasTileSet)
	{
		FileDataPair tileSetFileDataPair;
		tileSetFileDataPair.m_Data = m_TileSetsDataPair.m_Data[metaObj["assetUUID"].toString()].toObject();
		tileSetFileDataPair.m_Meta = m_TileSetsDataPair.m_Meta[metaObj["assetUUID"].toString()].toObject();

		AtlasTileSet *pNewTileSet = new AtlasTileSet(*this,
													 QUuid(metaObj["assetUUID"].toString()),
													 JSONOBJ_TOINT(metaObj, "checksum"),
													 JSONOBJ_TOINT(metaObj, "bankId"),
													 metaObj["name"].toString(),
													 HyTextureInfo(JSONOBJ_TOINT(metaObj, "textureInfo")),
													 metaObj["width"].toInt(),
													 metaObj["height"].toInt(),
													 metaObj["x"].toInt(),
													 metaObj["y"].toInt(),
													 metaObj["textureIndex"].toInt(),
													 tileSetFileDataPair,
													 false,
													 metaObj["errors"].toInt(0));
		return pNewTileSet;
	}

	HyGuiLog("AtlasModel::OnAllocateAssetData() - Unknown asset type: " % metaObj["itemType"].toString(), LOGTYPE_Error);
	return nullptr;
}

/*virtual*/ bool AtlasModel::OnRemoveAssets(QStringList sPreviousFilterPaths, QList<IAssetItemData *> assetList) /*override*/
{
	QMap<BankData *, QSet<int> > repackTexIndexMap;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[i]);

		if(pFrame->GetSubAtlasType() == ITEM_AtlasTileSet)
		{
			QString sTileSetUuid = pFrame->GetUuid().toString(QUuid::WithoutBraces);
			m_TileSetsDataPair.m_Data.remove(sTileSetUuid);
			m_TileSetsDataPair.m_Meta.remove(sTileSetUuid);

			WriteTileSetsToDisk();
		}

		repackTexIndexMap[m_BanksModel.GetBank(GetBankIndexFromBankId(pFrame->GetBankId()))].insert(pFrame->GetTextureIndex());

		DeleteAsset(pFrame);
	}

	for(auto iter = repackTexIndexMap.begin(); iter != repackTexIndexMap.end(); ++iter)
		AddTexturesToRepack(iter.key(), iter.value());

	return true;
}

/*virtual*/ bool AtlasModel::OnReplaceAssets(QStringList sImportAssetList, QList<IAssetItemData *> assetList) /*override*/
{
	// Error check all the replacement assets before adding them, and cancel entire replace if any fail
	QList<QImage *> newReplacementImageList;
	for(int i = 0; i < assetList.count(); ++i)
	{
		// Ensure all new replacement images will fit on the specified atlas
		QFileInfo fileInfo(sImportAssetList[i]);
		QImage *pNewImage = new QImage(fileInfo.absoluteFilePath());
		QSize atlasDimensions = GetMaxAtlasDimensions(GetBankIndexFromBankId(assetList[i]->GetBankId()));

		bool bPassesChecks = true;
		if(IsImageValid(*pNewImage, assetList[i]->GetBankId()) == false)
		{
			HyGuiLog("Replacement image " % fileInfo.fileName() % " will not fit in atlas group '" % QString::number(assetList[i]->GetBankId()) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);
			bPassesChecks = false;
		}
		if(static_cast<AtlasFrame *>(assetList[i])->GetSubAtlasType() != ITEM_None)
		{
			HyGuiLog("Replacing " % assetList[i]->GetName() % " is invalid because it is a sub-atlas for an existing item", LOGTYPE_Warning);
			bPassesChecks = false;
		}

		if(bPassesChecks == false)
		{
			delete pNewImage;
			for(int j = 0; j < newReplacementImageList.size(); ++j)
				delete newReplacementImageList[j];

			return false;
		}

		newReplacementImageList.push_back(pNewImage);
	}

	QMap<BankData *, QSet<int> > repackTexIndexMap;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[i]);
		HyGuiLog("Replacing: " % pFrame->GetName() % " -> " % sImportAssetList[i], LOGTYPE_Info);

		repackTexIndexMap[m_BanksModel.GetBank(GetBankIndexFromBankId(pFrame->GetBankId()))].insert(pFrame->GetTextureIndex());

		QFileInfo fileInfo(sImportAssetList[i]);
		ReplaceFrame(pFrame, fileInfo.baseName(), *newReplacementImageList[i], ITEM_None);
	}
	for(int j = 0; j < newReplacementImageList.size(); ++j)
		delete newReplacementImageList[j];

	for(auto iter = repackTexIndexMap.begin(); iter != repackTexIndexMap.end(); ++iter)
		AddTexturesToRepack(iter.key(), iter.value());

	return true;
}

/*virtual*/ bool AtlasModel::OnUpdateAssets(QList<IAssetItemData *> assetList) /*override*/
{
	QMap<BankData *, QSet<int> > repackTexIndexMap;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[i]);
		repackTexIndexMap[m_BanksModel.GetBank(GetBankIndexFromBankId(pFrame->GetBankId()))].insert(pFrame->GetTextureIndex());
	}

	for(auto iter = repackTexIndexMap.begin(); iter != repackTexIndexMap.end(); ++iter)
		AddTexturesToRepack(iter.key(), iter.value());

	return true;
}

/*virtual*/ bool AtlasModel::OnMoveAssets(QList<IAssetItemData *> assetsList, quint32 uiNewBankId) /*override*/
{
	// Ensure all transferred assets (images) can fit on new atlas
	for(int i = 0; i < assetsList.count(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetsList[i]);
		if(pFrame->GetBankId() == uiNewBankId)
			continue;

		QSize atlasDimensions = GetMaxAtlasDimensions(GetBankIndexFromBankId(uiNewBankId));
		if(IsImageValid(pFrame->GetSize().width(), pFrame->GetSize().height(), uiNewBankId) == false)
		{
			HyGuiLog("Cannot transfer image " % pFrame->GetName() % " because it will not fit in atlas group '" % QString::number(uiNewBankId) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);
			return false;
		}
	}

	QMap<uint, QSet<int> > affectedTextureIndexMap; // old
	QSet<IAssetItemData *> framesGoingToNewAtlasGrpSet; // new

	for(int i = 0; i < assetsList.count(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetsList[i]);
		if(pFrame->GetBankId() == uiNewBankId)
			continue;

		affectedTextureIndexMap[GetBankIndexFromBankId(pFrame->GetBankId())].insert(pFrame->GetTextureIndex());
		framesGoingToNewAtlasGrpSet.insert(pFrame);

		pFrame->ClearTextureIndex(); // These frames moved to a new bank and do not have a texture index anymore

		MoveAsset(pFrame, uiNewBankId);
	}

	// Repack all old affected atlas groups
	if(affectedTextureIndexMap.empty() == false)
	{
		for(auto iter = affectedTextureIndexMap.begin(); iter != affectedTextureIndexMap.end(); ++iter)
			AddTexturesToRepack(m_BanksModel.GetBank(iter.key()), iter.value());
	}

	// Repack new affected atlas group
	AddAssetsToRepack(m_BanksModel.GetBank(GetBankIndexFromBankId(uiNewBankId)), framesGoingToNewAtlasGrpSet);

	return true;
}

/*virtual*/ void AtlasModel::OnFlushRepack() /*override*/
{
	// Ensure 'm_RepackAffectedAssetsMap' contains the BankData *'s that 'm_RepackTexIndicesMap' contains
	for(auto iter = m_RepackTexIndicesMap.begin(); iter != m_RepackTexIndicesMap.end(); ++iter)
	{
		BankData *pBank = iter.key();
		if(m_RepackAffectedAssetsMap.contains(pBank) == false)
			m_RepackAffectedAssetsMap[pBank] = QSet<IAssetItemData *>();
	}

	QSet<int> repackTexIndicesSet;
	for(auto iter = m_RepackAffectedAssetsMap.begin(); iter != m_RepackAffectedAssetsMap.end(); ++iter)
	{
		BankData *pBank = iter.key();

		// Always repack the unfilled textures to ensure it gets filled as much as it can
		QJsonArray unfilledTextureIndicesArray = pBank->m_MetaObj["unfilledIndices"].toArray();
		for(auto unfilledTextureIndex : unfilledTextureIndicesArray)
			repackTexIndicesSet.insert(unfilledTextureIndex.toInt());

		QSet<IAssetItemData *> affectedSet(iter.value());
		for(auto *pAsset : affectedSet)
			repackTexIndicesSet.insert(static_cast<AtlasFrame *>(pAsset)->GetTextureIndex());

		AddTexturesToRepack(pBank, repackTexIndicesSet);

		// Delete all affected textures. The following AtlasRepackThread will regenerate all the remaining/modified assets
		if(m_RepackTexIndicesMap.contains(pBank))
			repackTexIndicesSet.unite(m_RepackTexIndicesMap[pBank]);

		QList<int> textureIndexList = repackTexIndicesSet.values();

		QDir runtimeBankDir(pBank->m_sAbsPath);
		for(int i = 0; i < textureIndexList.size(); ++i)
		{
			QFile pngFile(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".png"));
			if(pngFile.exists())
			{
				if(!pngFile.remove())
					HyGuiLog("Failed to delete PNG atlas texture '" % pngFile.fileName() % "':" % pngFile.errorString(), LOGTYPE_Error);
			}
			else
			{
				QFile ddsFile(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".dds"));
				if(ddsFile.exists())
				{
					if(!ddsFile.remove())
						HyGuiLog("Failed to delete DDS atlas texture '" % ddsFile.fileName() % "':" % ddsFile.errorString(), LOGTYPE_Error);
				}
				else
				{
					QFile astcFile(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".astc"));
					if(astcFile.exists())
					{
						if(!astcFile.remove())
							HyGuiLog("Failed to delete ASTC atlas texture '" % astcFile.fileName() % "':" % astcFile.errorString(), LOGTYPE_Error);
					}
				}
			}
		}
	}

	m_RepackTexIndicesMap.clear();
}

/*virtual*/ void AtlasModel::OnSaveMeta(QJsonObject &metaObjRef) /*override*/
{
	//// Just ensure TileSet.meta file exists - don't try to save over an existing one
	//QFile tileSetMetaFile(m_MetaDir.absoluteFilePath(HyGlobal::ItemName(ITEM_AtlasTileSet, true) % HYGUIPATH_MetaExt));
	//if(tileSetMetaFile.exists() == false)
	//	SaveTileSets();
}

/*virtual*/ void AtlasModel::OnSaveData(QJsonObject &dataObjRef) /*override*/
{
	QJsonArray banksArray;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		QJsonObject bankObj;

		bankObj.insert("bankId", m_BanksModel.GetBank(i)->m_MetaObj["bankId"].toInt());
		
		// These List indices correspond to each other
		QList<QJsonArray> assetArrayList;
		QList<HyTextureInfo> textureInfoList;

		QList<IAssetItemData *> &entireBankAssetsListRef = m_BanksModel.GetBank(i)->m_AssetList;
		for(int j = 0; j < entireBankAssetsListRef.size(); ++j)
		{
			AtlasFrame *pAtlasFrame = static_cast<AtlasFrame *>(entireBankAssetsListRef[j]);
			if(pAtlasFrame->GetTextureIndex() < 0)
				continue;

			while(assetArrayList.empty() || assetArrayList.size() <= pAtlasFrame->GetTextureIndex())
			{
				assetArrayList.append(QJsonArray());
				textureInfoList.append(HyTextureInfo());
			}

			QJsonObject frameObj;
			frameObj.insert("checksum", QJsonValue(static_cast<qint64>(pAtlasFrame->GetChecksum())));

			quint64 uiCropMask = pAtlasFrame->GetCropMask();
			frameObj.insert("cropMaskHi", QJsonValue(static_cast<qint64>(uiCropMask >> 32)));
			frameObj.insert("cropMaskLo", QJsonValue(static_cast<qint64>(uiCropMask & 0xFFFFFFFF)));
			//frameObj.insert("left", QJsonValue(pAtlasFrame->GetX()));
			//frameObj.insert("top", QJsonValue(pAtlasFrame->GetY()));
			//frameObj.insert("right", QJsonValue(pAtlasFrame->GetX() + pAtlasFrame->GetCrop().width()));
			//frameObj.insert("bottom", QJsonValue(pAtlasFrame->GetY() + pAtlasFrame->GetCrop().height()));
			
			quint64 uiFrameMask = pAtlasFrame->GetFrameMask();
			frameObj.insert("frameMaskHi", QJsonValue(static_cast<qint64>(uiFrameMask >> 32)));
			frameObj.insert("frameMaskLo", QJsonValue(static_cast<qint64>(uiFrameMask & 0xFFFFFFFF)));

			assetArrayList[pAtlasFrame->GetTextureIndex()].append(frameObj);
			textureInfoList[pAtlasFrame->GetTextureIndex()] = pAtlasFrame->GetTextureInfo();
		}

		QJsonArray textureArray;
		for(int j = 0; j < assetArrayList.size(); ++j)
		{
			QJsonObject textureObj;

			QSize textureSize = GetTextureSize(i, j);
			textureObj.insert("width", textureSize.width());
			textureObj.insert("height", textureSize.height());

			textureObj.insert("textureInfo", QJsonValue(static_cast<qint64>(textureInfoList[j].GetBucketId())));
			textureObj.insert("assets", assetArrayList[j]);

			textureArray.append(textureObj);
		}

		bankObj.insert("textures", textureArray);

		banksArray.append(bankObj);
	}
	
	// TileSets ///////////
	QJsonArray tileSetsArray;
	for(auto iter = m_TileSetsDataPair.m_Data.begin(); iter != m_TileSetsDataPair.m_Data.end(); ++iter)
		tileSetsArray.append(iter.value());

	dataObjRef.insert("banks", banksArray);
	dataObjRef.insert("tileSets", tileSetsArray);
}

void AtlasModel::AddTexturesToRepack(BankData *pBankData, QSet<int> texIndicesSet)
{
	m_RepackTexIndicesMap[pBankData].unite(texIndicesSet);

	QList<int> repackTexIndicesList = texIndicesSet.values();

	// Make sure all the affected frames within 'repackTexIndicesList' are added to 'AddAssetsToRepack'
	QList<IAssetItemData *> entireAssetList = pBankData->m_AssetList;
	for(int i = 0; i < entireAssetList.size(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(entireAssetList[i]);
		for(int j = 0; j < repackTexIndicesList.size(); ++j)
		{
			if(pFrame->GetTextureIndex() == repackTexIndicesList[j])
			{
				AddAssetsToRepack(pBankData, pFrame);
				break;
			}
		}
	}
}

AtlasFrame *AtlasModel::ImportImage(QString sName, QImage &newImage, quint32 uiBankId, ItemType eSubAtlasType, QUuid uuid)
{
	QFileInfo fileInfo(sName);

	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.sizeInBytes());

	QRect rAlphaCrop(0, 0, newImage.width(), newImage.height());
	if(eSubAtlasType == ITEM_None) // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		rAlphaCrop = HyGlobal::AlphaCropImage(newImage);

	// NOTE: QRect (rAlphaCrop) needs to be converted to L,T,R,B margins
	quint16 uiCropLeft = rAlphaCrop.left();
	quint16 uiCropTop = rAlphaCrop.top();
	quint16 uiCropRight = newImage.width() - (rAlphaCrop.left() + rAlphaCrop.width());
	quint16 uiCropBottom = newImage.height() - (rAlphaCrop.top() + rAlphaCrop.height());

	AtlasFrame *pNewAsset = new AtlasFrame(ITEM_AtlasFrame,
											*this,
											eSubAtlasType,
											uuid,
											uiChecksum,
											uiBankId,
											fileInfo.baseName(),
											uiCropLeft,
											uiCropTop,
											uiCropRight,
											uiCropBottom,
											m_DefaultTextureInfo,
											newImage.width(),
											newImage.height(),
											-1,
											-1,
											-1,
											0);

	newImage.save(m_MetaDir.absoluteFilePath(pNewAsset->ConstructMetaFileName()));
	RegisterAsset(pNewAsset);
	
	return pNewAsset;
}
