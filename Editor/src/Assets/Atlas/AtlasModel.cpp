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
#include "Project.h"
#include "AtlasRepackThread.h"
#include "MainWindow.h"
#include "DlgAtlasGroupSettings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMimeData>

AtlasModel::AtlasModel(Project &projRef) :
	IManagerModel(projRef, ASSETMAN_Atlases)
{

}

/*virtual*/ AtlasModel::~AtlasModel()
{

}

QFileInfoList AtlasModel::GetExistingTextureInfoList(uint uiBankIndex)
{
	QDir bankDir(m_BanksModel.GetBank(uiBankIndex)->m_sAbsPath);
	return bankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
}

int AtlasModel::GetNumTextures(uint uiBankIndex)
{
	return GetExistingTextureInfoList(uiBankIndex).size();
}

QSize AtlasModel::GetAtlasDimensions(uint uiBankIndex)
{
	int iWidth = m_BanksModel.GetBank(uiBankIndex)->m_MetaObj["maxWidth"].toInt();
	int iHeight = m_BanksModel.GetBank(uiBankIndex)->m_MetaObj["maxHeight"].toInt();
	
	return QSize(iWidth, iHeight);
}

bool AtlasModel::IsImageValid(QImage &image, quint32 uiBankId)
{
	return IsImageValid(image.width(), image.height(), uiBankId);
}

bool AtlasModel::IsImageValid(int iWidth, int iHeight, quint32 uiBankId)
{
	uint uiBankIndex = GetBankIndexFromBankId(uiBankId);
	return IsImageValid(iWidth, iHeight, m_BanksModel.GetBank(uiBankIndex)->m_MetaObj);
}

bool AtlasModel::IsImageValid(int iWidth, int iHeight, const QJsonObject &atlasSettings)
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

AtlasFrame *AtlasModel::GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiBankIndex, ItemType eType)
{
	if(IsImageValid(newImage, m_BanksModel.GetBank(uiBankIndex)->GetId()) == false)
		return nullptr;

	// This will also create a meta image and register asset
	AtlasFrame *pFrame = ImportImage(sName, newImage, m_BanksModel.GetBank(uiBankIndex)->GetId(), eType, QUuid::createUuid());

	// This retrieves the newly created AtlasFrame and links it to its ProjectItemData
	QList<TreeModelItemData *> returnList = pItem->GetProject().IncrementDependencies(pItem, QList<QUuid>() << pFrame->GetUuid());
	if(returnList.empty() == false)
	{
		AddAssetsToRepack(m_BanksModel.GetBank(uiBankIndex), static_cast<AtlasFrame *>(returnList[0]));
		return static_cast<AtlasFrame *>(returnList[0]);
	}

	return nullptr;
}

bool AtlasModel::ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage)
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
	pFrame->ReplaceImage(sName, uiChecksum, newImage, m_MetaDir);

	// Re-enter the frame into the map
	RegisterAsset(pFrame);

	uint uiBankIndex = GetBankIndexFromBankId(pFrame->GetBankId());
	AddAssetsToRepack(m_BanksModel.GetBank(uiBankIndex), pFrame);

	return true;
}

//void AtlasModel::RepackAll(uint uiBankIndex)
//{
//	int iNumTotalTextures = GetNumTextures(uiBankIndex);
//	
//	QSet<int> textureIndexSet;
//	for(int i = 0; i < iNumTotalTextures; ++i)
//		textureIndexSet.insert(i);
//
//	if(textureIndexSet.empty() == false)
//		Repack(uiBankIndex, textureIndexSet, QSet<AtlasFrame *>());
//	else
//		SaveRuntime();
//}

/*virtual*/ QString AtlasModel::OnBankInfo(uint uiBankIndex) /*override*/
{
	QString sInfo = "Num Textures: " % QString::number(GetNumTextures(uiBankIndex)) % " | " %
		"(" % QString::number(GetAtlasDimensions(uiBankIndex).width()) % "x" % QString::number(GetAtlasDimensions(uiBankIndex).height()) % ")";

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
				SaveRuntime();
	
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
		SaveRuntime();
}

/*virtual*/ void AtlasModel::OnCreateNewBank(QJsonObject &newMetaBankObjRef) /*override*/
{
	newMetaBankObjRef.insert("cmbSortOrder", 0);
	newMetaBankObjRef.insert("sbFrameMarginTop", 0);
	newMetaBankObjRef.insert("sbFrameMarginLeft", 0);
	newMetaBankObjRef.insert("sbFrameMarginRight", 1);
	newMetaBankObjRef.insert("sbFrameMarginBottom", 1);
	newMetaBankObjRef.insert("maxWidth", 2048);
	newMetaBankObjRef.insert("maxHeight", 2048);
	newMetaBankObjRef.insert("cmbHeuristic", 1);
}

/*virtual*/ IAssetItemData *AtlasModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
{
	QRect rAlphaCrop(QPoint(metaObj["cropLeft"].toInt(), metaObj["cropTop"].toInt()),
					 QPoint(metaObj["cropRight"].toInt(), metaObj["cropBottom"].toInt()));

	

	AtlasFrame *pNewFrame = new AtlasFrame(*this,
										   HyGlobal::GetTypeFromString(metaObj["itemType"].toString()),
										   QUuid(metaObj["assetUUID"].toString()),
										   JSONOBJ_TOINT(metaObj, "checksum"),
										   JSONOBJ_TOINT(metaObj, "bankId"),
										   metaObj["name"].toString(),
										   rAlphaCrop,
										   HyTextureInfo(JSONOBJ_TOINT(metaObj, "textureInfo")),
										   metaObj["width"].toInt(),
										   metaObj["height"].toInt(),
										   metaObj["x"].toInt(),
										   metaObj["y"].toInt(),
										   metaObj["textureIndex"].toInt(),
										   metaObj["errors"].toInt(0));

	return pNewFrame;
}

/*virtual*/ void AtlasModel::OnGenerateAssetsDlg(const QModelIndex &indexDestination) /*override*/
{
}

/*virtual*/ QList<IAssetItemData *> AtlasModel::OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, ItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) /*override*/
{
	QList<IAssetItemData *> returnList;

	// Error check all the imported assets before adding them, and cancel entire import if any fail
	QList<QImage *> newImageList;
	for(int i = 0; i < sImportAssetList.size(); ++i)
	{
		QFileInfo fileInfo(sImportAssetList[i]);

		QImage *pNewImage = new QImage(fileInfo.absoluteFilePath());
		newImageList.push_back(pNewImage);

		QSize atlasDimensions = GetAtlasDimensions(GetBankIndexFromBankId(uiBankId));
		if(IsImageValid(*pNewImage, uiBankId) == false)
		{
			HyGuiLog("Importing image " % fileInfo.fileName() % " will not fit in atlas bank '" % QString::number(uiBankId) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);
			
			for(auto image : newImageList)
				delete image;
			return returnList;
		}
	}

	// Passed error check: proceed with import
	for(int i = 0; i < sImportAssetList.size(); ++i)
		returnList.append(ImportImage(QFileInfo(sImportAssetList[i]).baseName(), *newImageList[i], uiBankId, eType, correspondingUuidList[i]));

	if(returnList.empty() == false)
	{
		QSet<IAssetItemData *> returnListAsSet(returnList.begin(), returnList.end());
		AddAssetsToRepack(m_BanksModel.GetBank(GetBankIndexFromBankId(uiBankId)), returnListAsSet);
	}

	for(auto image : newImageList)
		delete image;
	return returnList;
}

/*virtual*/ bool AtlasModel::OnRemoveAssets(QStringList sPreviousFilterPaths, QList<IAssetItemData *> assetList) /*override*/
{
	QMap<BankData *, QSet<int> > repackTexIndexMap;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[i]);
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
		QSize atlasDimensions = GetAtlasDimensions(GetBankIndexFromBankId(assetList[i]->GetBankId()));
		if(IsImageValid(*pNewImage, assetList[i]->GetBankId()) == false)
		{
			HyGuiLog("Replacement image " % fileInfo.fileName() % " will not fit in atlas group '" % QString::number(assetList[i]->GetBankId()) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);

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
		ReplaceFrame(pFrame, fileInfo.baseName(), *newReplacementImageList[i]);
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

		QSize atlasDimensions = GetAtlasDimensions(GetBankIndexFromBankId(uiNewBankId));
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
	// Always repack the unfilled textures to ensure it gets filled as much as it can
	QSet<int> repackTexIndicesSet;

	for(auto iter = m_RepackAffectedAssetsMap.begin(); iter != m_RepackAffectedAssetsMap.end(); ++iter)
	{
		BankData *pBank = iter.key();
		QJsonArray unfilledTextureIndicesArray = pBank->m_MetaObj["unfilledIndices"].toArray();
		for(auto unfilledTextureIndex : unfilledTextureIndicesArray)
			repackTexIndicesSet.insert(unfilledTextureIndex.toInt());

		QSet<IAssetItemData *> affectedSet(iter.value());
		for(auto *pAsset : affectedSet)
			repackTexIndicesSet.insert(static_cast<AtlasFrame *>(pAsset)->GetTextureIndex());

		AddTexturesToRepack(pBank, repackTexIndicesSet);

		// Delete all affected textures. The following AtlasRepackThread will regenerate all the remaining/modified assets
		QList<int> textureIndexList = repackTexIndicesSet.values();
		QDir runtimeBankDir(pBank->m_sAbsPath);
		for(int i = 0; i < textureIndexList.size(); ++i)
		{
			QFile::remove(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".png"));
			QFile::remove(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".dds"));
			QFile::remove(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".astc"));
		}
	}
}

/*virtual*/ void AtlasModel::OnSaveMeta(QJsonObject &metaObjRef) /*override*/
{
}

/*virtual*/ QJsonObject AtlasModel::GetSaveJson() /*override*/
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
			frameObj.insert("left", QJsonValue(pAtlasFrame->GetX()));
			frameObj.insert("top", QJsonValue(pAtlasFrame->GetY()));
			frameObj.insert("right", QJsonValue(pAtlasFrame->GetX() + pAtlasFrame->GetCrop().width()));
			frameObj.insert("bottom", QJsonValue(pAtlasFrame->GetY() + pAtlasFrame->GetCrop().height()));

			assetArrayList[pAtlasFrame->GetTextureIndex()].append(frameObj);
			textureInfoList[pAtlasFrame->GetTextureIndex()] = pAtlasFrame->GetTextureInfo();
		}

		QJsonArray textureArray;
		for(int j = 0; j < assetArrayList.size(); ++j)
		{
			QJsonObject textureObj;
			textureObj.insert("width", m_BanksModel.GetBank(i)->m_MetaObj["maxWidth"].toInt());
			textureObj.insert("height", m_BanksModel.GetBank(i)->m_MetaObj["maxHeight"].toInt());

			textureObj.insert("textureInfo", QJsonValue(static_cast<qint64>(textureInfoList[j].GetBucketId())));
			textureObj.insert("assets", assetArrayList[j]);

			textureArray.append(textureObj);
		}

		bankObj.insert("textures", textureArray);

		banksArray.append(bankObj);
	}

	QJsonObject atlasInfoObj;
	atlasInfoObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	atlasInfoObj.insert("banks", banksArray);

	return atlasInfoObj;
}

void AtlasModel::AddTexturesToRepack(BankData *pBankData, QSet<int> texIndicesSet)
{
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

AtlasFrame *AtlasModel::ImportImage(QString sName, QImage &newImage, quint32 uiBankId, ItemType eType, QUuid uuid)
{
	QFileInfo fileInfo(sName);

	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.sizeInBytes());

	QRect rAlphaCrop(0, 0, newImage.width(), newImage.height());
	if(eType == ITEM_AtlasFrame) // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		rAlphaCrop = ImagePacker::crop(newImage);

	HyTextureInfo info(HYTEXFILTER_BILINEAR, HYTEXTURE_Uncompressed, 4, 0);
	AtlasFrame *pNewAsset = new AtlasFrame(*this,
		eType,
		uuid,
		uiChecksum,
		uiBankId,
		fileInfo.baseName(),
		rAlphaCrop,
		info,
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
