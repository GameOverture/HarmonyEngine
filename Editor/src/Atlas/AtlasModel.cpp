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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void AtlasModel::FrameLookup::AddLookup(AtlasFrame *pFrame)
//{
//	m_FrameIdMap[pFrame->GetId()] = pFrame;
//
//	uint32 uiChecksum = pFrame->GetImageChecksum();
//	
//	if(m_FrameChecksumMap.contains(uiChecksum))
//	{
//		m_FrameChecksumMap.find(uiChecksum).value().append(pFrame);
//		HyGuiLog("'" % pFrame->GetName() % "' is a duplicate of '" % m_FrameChecksumMap.find(uiChecksum).value()[0]->GetName() % "' with the checksum: " % QString::number(uiChecksum) % " totaling: " % QString::number(m_FrameChecksumMap.find(uiChecksum).value().size()), LOGTYPE_Info);
//	}
//	else
//	{
//		QList<AtlasFrame *> newFrameList;
//		newFrameList.append(pFrame);
//		m_FrameChecksumMap[uiChecksum] = newFrameList;
//	}
//}
//bool AtlasModel::FrameLookup::RemoveLookup(AtlasFrame *pFrame)  // Returns true if no remaining duplicates exist
//{
//	m_FrameIdMap.remove(pFrame->GetId());
//
//	auto iter = m_FrameChecksumMap.find(pFrame->GetImageChecksum());
//	if(iter == m_FrameChecksumMap.end())
//		HyGuiLog("AtlasModel::RemoveLookup could not find frame", LOGTYPE_Error);
//	
//	iter.value().removeOne(pFrame);
//	if(iter.value().size() == 0)
//	{
//		m_FrameChecksumMap.remove(pFrame->GetImageChecksum());
//		return true;
//	}
//	
//	return false;
//}
//AtlasFrame *AtlasModel::FrameLookup::FindById(QUuid uuid)
//{
//	auto iter = m_FrameIdMap.find(uuid);
//	if(iter == m_FrameIdMap.end())
//		return nullptr;
//	else
//		return iter.value();
//}
//QList<AtlasFrame *> AtlasModel::FrameLookup::FindByChecksum(quint32 uiChecksum)
//{
//	auto iter = m_FrameChecksumMap.find(uiChecksum);
//	if(iter == m_FrameChecksumMap.end())
//		return QList<AtlasFrame *>();
//	else
//		return iter.value();
//}
//bool AtlasModel::FrameLookup::DoesImageExist(quint32 uiChecksum)
//{
//	return m_FrameChecksumMap.contains(uiChecksum);
//}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AtlasModel::AtlasModel(Project &projRef) :
	IManagerModel(projRef, ITEM_AtlasImage)
{

}

/*virtual*/ AtlasModel::~AtlasModel()
{

}

//void AtlasModel::StashTreeWidgets(QList<AtlasTreeItem *> treeItemList)
//{
//	m_TopLevelTreeItemList = treeItemList;
//}
//
//QList<AtlasTreeItem *> AtlasModel::GetTopLevelTreeItemList()
//{
//	return m_TopLevelTreeItemList;
//}

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

AtlasFrame *AtlasModel::GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiBankIndex, HyGuiItemType eType)
{
	if(IsImageValid(newImage, m_BanksModel.GetBank(uiBankIndex)->GetId()) == false)
		return nullptr;

	// This will also create a meta image and register asset
	AtlasFrame *pFrame = ImportImage(sName, newImage, m_BanksModel.GetBank(uiBankIndex)->GetId(), eType, QUuid::createUuid());

	QSet<AtlasFrame *> newFrameSet;
	newFrameSet.insert(pFrame);
	Repack(uiBankIndex, QSet<int>(), newFrameSet);

	// This retrieves the newly created AtlasFrame and links it to its ProjectItemData
	QList<QUuid> idList;
	idList.append(pFrame->GetUuid());
	QList<AssetItemData *> returnList = RequestAssetsByUuid(pItem, idList);

	if(returnList.empty() == false)
		return static_cast<AtlasFrame *>(returnList[0]);

	return nullptr;
}

bool AtlasModel::ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack)
{
	if(IsImageValid(newImage, pFrame->GetBankId()) == false)
		return false;

	QSet<int> textureIndexToReplaceSet;
	if(pFrame->GetTextureIndex() >= 0)
		textureIndexToReplaceSet.insert(pFrame->GetTextureIndex());

	// First remove the frame from the map
	if(RemoveLookup(pFrame))
		pFrame->DeleteMetaFile();
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		if(pFrame->GetBankId() == m_BanksModel.GetBank(i)->GetId())
		{
			m_BanksModel.GetBank(i)->m_AssetList.removeOne(pFrame);
			break;
		}
	}

	// Determine the new checksum into the map
	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.sizeInBytes());
	pFrame->ReplaceImage(sName, uiChecksum, newImage, m_MetaDir);

	// Re-enter the frame into the map
	RegisterAsset(pFrame);

	if(bDoAtlasGroupRepack)
	{
		uint uiBankIndex = GetBankIndexFromBankId(pFrame->GetBankId());
		Repack(uiBankIndex, textureIndexToReplaceSet, QSet<AtlasFrame *>());
	}

	return true;
}

void AtlasModel::RepackAll(uint uiBankIndex)
{
	int iNumTotalTextures = GetNumTextures(uiBankIndex);
	
	QSet<int> textureIndexSet;
	for(int i = 0; i < iNumTotalTextures; ++i)
		textureIndexSet.insert(i);

	if(textureIndexSet.empty() == false)
		Repack(uiBankIndex, textureIndexSet, QSet<AtlasFrame *>());
	else
		SaveRuntime();
}

void AtlasModel::Repack(uint uiBankIndex, QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet)
{
	// Always repack the last texture to ensure it gets filled as much as it can
	QFileInfoList existingTexturesInfoList = GetExistingTextureInfoList(uiBankIndex);
	for(int i = HyClamp(existingTexturesInfoList.size() - 1, 0, existingTexturesInfoList.size()); i < existingTexturesInfoList.size(); ++i)
		repackTexIndicesSet.insert(i);

	QList<int> textureIndexList = repackTexIndicesSet.values();

	// Get all the affected frames into a list
	QList<AssetItemData *> &atlasGrpFrameListRef = m_BanksModel.GetBank(uiBankIndex)->m_AssetList;
	for(int i = 0; i < atlasGrpFrameListRef.size(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(atlasGrpFrameListRef[i]);
		for(int j = 0; j < textureIndexList.size(); ++j)
		{
			if(pFrame->GetTextureIndex() == textureIndexList[j])
				newFramesSet.insert(pFrame);
		}
	}

	QList<AtlasFrame *>newFramesList = newFramesSet.values();


	AtlasRepackThread *pWorkerThread = new AtlasRepackThread(*m_BanksModel.GetBank(uiBankIndex), textureIndexList, newFramesList, m_MetaDir);
	connect(pWorkerThread, &AtlasRepackThread::finished, pWorkerThread, &QObject::deleteLater);
	connect(pWorkerThread, &AtlasRepackThread::LoadUpdate, this, &AtlasModel::OnLoadUpdate);
	connect(pWorkerThread, &AtlasRepackThread::RepackIsFinished, this, &AtlasModel::OnRepackFinished);

	MainWindow::SetLoading("Repacking Atlases", 0);
	pWorkerThread->start();
}

/*virtual*/ void AtlasModel::OnNewBankDefaults(QJsonObject &bankObjRef) /*override*/
{
	bankObjRef.insert("cmbSortOrder", 0);
	bankObjRef.insert("sbFrameMarginTop", 0);
	bankObjRef.insert("sbFrameMarginLeft", 0);
	bankObjRef.insert("sbFrameMarginRight", 1);
	bankObjRef.insert("sbFrameMarginBottom", 1);
	bankObjRef.insert("extrude", 1);
	bankObjRef.insert("chkMerge", true);
	bankObjRef.insert("chkSquare", true);
	bankObjRef.insert("chkAutosize", true);
	bankObjRef.insert("minFillRate", 80);
	bankObjRef.insert("maxWidth", 2048);
	bankObjRef.insert("maxHeight", 2048);
	bankObjRef.insert("cmbHeuristic", 1);
	bankObjRef.insert("textureFormat", QString(HyAssets::GetTextureFormatName(HYTEXTURE_R8G8B8A8).c_str()));
	bankObjRef.insert("textureFiltering", QString(HyAssets::GetTextureFilteringName(HYTEXFILTER_BILINEAR).c_str()));
}

/*virtual*/ QString AtlasModel::OnBankInfo(uint uiBankIndex) /*override*/
{
	QString sInfo = "Num Textures: " % QString::number(GetNumTextures(uiBankIndex)) % " | " %
		"(" % QString::number(GetAtlasDimensions(uiBankIndex).width()) % "x" % QString::number(GetAtlasDimensions(uiBankIndex).height()) % ")";

	return sInfo;
}

/*virtual*/ bool AtlasModel::OnBankSettingsDlg(uint uiBankIndex) /*override*/
{
	QList<AssetItemData *> assetList = m_BanksModel.GetBank(uiBankIndex)->m_AssetList;
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
			m_BanksModel.GetBank(uiBankIndex)->m_MetaObj = newPackerSettings;
	
			if(pDlg->IsSettingsDirty() && bBankHasAssets)
				RepackAll(uiBankIndex);
			else
				SaveRuntime();
	
			bAccepted = true;
		}
	}
	
	delete pDlg;
	//RefreshInfo();
	
	return bAccepted;
}

/*virtual*/ QStringList AtlasModel::GetSupportedFileExtList() /*override*/
{
	return QStringList() << ".png";
}

/*virtual*/ void AtlasModel::OnAllocateDraw(IManagerDraw *&pDrawOut) /*override*/
{
	pDrawOut = new AtlasDraw(*this);
}

/*virtual*/ AssetItemData *AtlasModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
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
										   metaObj["width"].toInt(),
										   metaObj["height"].toInt(),
										   metaObj["x"].toInt(),
										   metaObj["y"].toInt(),
										   metaObj["textureIndex"].toInt(),
										   metaObj["errors"].toInt(0));

	return pNewFrame;
}

/*virtual*/ QList<AssetItemData *> AtlasModel::OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType, QList<QUuid> correspondingUuidList) /*override*/
{
	QList<AssetItemData *> returnList;

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
		QSet<AtlasFrame *> newSet;
		for(auto pItem : returnList.toSet())
			newSet.insert(static_cast<AtlasFrame *>(pItem));
		Repack(GetBankIndexFromBankId(uiBankId), QSet<int>(), newSet);
	}

	for(auto image : newImageList)
		delete image;
	return returnList;
}

/*virtual*/ bool AtlasModel::OnRemoveAssets(QList<AssetItemData *> assetList) /*override*/
{
	QMap<uint, QSet<int> > affectedTextureIndexMap;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[i]);
		affectedTextureIndexMap[GetBankIndexFromBankId(pFrame->GetBankId())].insert(pFrame->GetTextureIndex());

		DeleteAsset(pFrame);
	}

	if(affectedTextureIndexMap.empty() == false)
	{
		for(auto iter = affectedTextureIndexMap.begin(); iter != affectedTextureIndexMap.end(); ++iter)
			Repack(iter.key(), iter.value(), QSet<AtlasFrame *>());
	}

	return true;
}

/*virtual*/ bool AtlasModel::OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) /*override*/
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

	QMap<uint, QSet<int> > affectedTextureIndexMap;
	for(int i = 0; i < assetList.count(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[i]);
		HyGuiLog("Replacing: " % pFrame->GetName() % " -> " % sImportAssetList[i], LOGTYPE_Info);

		affectedTextureIndexMap[GetBankIndexFromBankId(pFrame->GetBankId())].insert(pFrame->GetTextureIndex());

		QFileInfo fileInfo(sImportAssetList[i]);
		ReplaceFrame(pFrame, fileInfo.baseName(), *newReplacementImageList[i], false);
	}

	for(int j = 0; j < newReplacementImageList.size(); ++j)
		delete newReplacementImageList[j];

	if(affectedTextureIndexMap.empty() == false)
	{
		for(auto iter = affectedTextureIndexMap.begin(); iter != affectedTextureIndexMap.end(); ++iter)
			Repack(iter.key(), iter.value(), QSet<AtlasFrame *>());
	}
}

/*virtual*/ bool AtlasModel::OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) /*override*/
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
	QSet<AtlasFrame *> framesGoingToNewAtlasGrpSet; // new

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
			Repack(iter.key(), iter.value(), QSet<AtlasFrame *>());
	}

	// Repack new affected atlas group
	Repack(GetBankIndexFromBankId(uiNewBankId), QSet<int>(), framesGoingToNewAtlasGrpSet);
}

/*virtual*/ QJsonObject AtlasModel::GetSaveJson() /*override*/
{
	QJsonArray banksArray;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		QJsonObject bankObj;

		bankObj.insert("bankId", m_BanksModel.GetBank(i)->m_MetaObj["bankId"].toInt());
		
		QList<QJsonArray> frameArrayList;
		QList<AssetItemData *> &atlasGrpFrameListRef = m_BanksModel.GetBank(i)->m_AssetList;
		for(int j = 0; j < atlasGrpFrameListRef.size(); ++j)
		{
			AtlasFrame *pAtlasFrame = static_cast<AtlasFrame *>(atlasGrpFrameListRef[j]);
			if(pAtlasFrame->GetTextureIndex() < 0)
				continue;

			while(frameArrayList.empty() || frameArrayList.size() <= pAtlasFrame->GetTextureIndex())
				frameArrayList.append(QJsonArray());

			QJsonObject frameObj;
			frameObj.insert("checksum", QJsonValue(static_cast<qint64>(pAtlasFrame->GetChecksum())));
			frameObj.insert("left", QJsonValue(pAtlasFrame->GetX()));
			frameObj.insert("top", QJsonValue(pAtlasFrame->GetY()));
			frameObj.insert("right", QJsonValue(pAtlasFrame->GetX() + pAtlasFrame->GetCrop().width()));
			frameObj.insert("bottom", QJsonValue(pAtlasFrame->GetY() + pAtlasFrame->GetCrop().height()));

			frameArrayList[pAtlasFrame->GetTextureIndex()].append(frameObj);
		}

		QJsonArray textureArray;
		for(int j = 0; j < frameArrayList.size(); ++j)
		{
			QJsonObject textureObj;
			textureObj.insert("width", m_BanksModel.GetBank(i)->m_MetaObj["maxWidth"].toInt());
			textureObj.insert("height", m_BanksModel.GetBank(i)->m_MetaObj["maxHeight"].toInt());
			textureObj.insert("format", m_BanksModel.GetBank(i)->m_MetaObj["textureFormat"].toString());
			textureObj.insert("filtering", m_BanksModel.GetBank(i)->m_MetaObj["textureFiltering"].toString());
			textureObj.insert("assets", frameArrayList[j]);

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

/*slot*/ void AtlasModel::OnLoadUpdate(QString sMsg, int iPercComplete)
{
	MainWindow::SetLoading(sMsg, iPercComplete);
}

/*slot*/ void AtlasModel::OnRepackFinished()
{
	SaveRuntime();
}

AtlasFrame *AtlasModel::ImportImage(QString sName, QImage &newImage, quint32 uiBankId, HyGuiItemType eType, QUuid uuid)
{
	QFileInfo fileInfo(sName);

	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.sizeInBytes());

	QRect rAlphaCrop(0, 0, newImage.width(), newImage.height());
	if(eType == ITEM_AtlasImage) // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		rAlphaCrop = ImagePacker::crop(newImage);

	AtlasFrame *pNewAsset = new AtlasFrame(*this,
		eType,
		uuid,//QUuid::createUuid(),
		uiChecksum,
		uiBankId,
		fileInfo.baseName(),
		rAlphaCrop,
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
