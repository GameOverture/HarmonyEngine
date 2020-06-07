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
#include "Project.h"
#include "AtlasWidget.h"
#include "AtlasRepackThread.h"
#include "MainWindow.h"
#include "DlgAtlasGroupSettings.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
	int iWidth = m_BanksModel.GetBank(uiBankIndex)->m_Settings["sbTextureWidth"].toInt();
	int iHeight = m_BanksModel.GetBank(uiBankIndex)->m_Settings["sbTextureHeight"].toInt();
	
	return QSize(iWidth, iHeight);
}

HyTextureFormat AtlasModel::GetAtlasTextureType(uint uiBankIndex)
{
	return static_cast<HyTextureFormat>(m_BanksModel.GetBank(uiBankIndex)->m_Settings["textureType"].toInt());
}

bool AtlasModel::IsImageValid(QImage &image, quint32 uiAtlasGrpId)
{
	return IsImageValid(image.width(), image.height(), uiAtlasGrpId);
}

bool AtlasModel::IsImageValid(int iWidth, int iHeight, quint32 uiAtlasGrpId)
{
	uint uiBankIndex = GetBankIndexFromBankId(uiAtlasGrpId);
	return IsImageValid(iWidth, iHeight, m_BanksModel.GetBank(uiBankIndex)->m_Settings);
}

bool AtlasModel::IsImageValid(int iWidth, int iHeight, const QJsonObject &atlasSettings)
{
	int iMarginWidth =  atlasSettings["sbFrameMarginLeft"].toInt();
	iMarginWidth +=     atlasSettings["sbFrameMarginRight"].toInt();
	int iMarginHeight = atlasSettings["sbFrameMarginBottom"].toInt();
	iMarginHeight +=    atlasSettings["sbFrameMarginTop"].toInt();

	QSize atlasMargins(iMarginWidth, iMarginHeight);
	QSize atlasDimensions(atlasSettings["sbTextureWidth"].toInt(), atlasSettings["sbTextureHeight"].toInt());

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

	// This will also create a meta image
	AtlasFrame *pFrame = ImportImage(sName, newImage, m_AtlasGrpList[uiAtlasGrpIndex]->GetId(), eType, nullptr);

	QSet<AtlasFrame *> newFrameSet;
	newFrameSet.insert(pFrame);
	Repack(uiAtlasGrpIndex, QSet<int>(), newFrameSet);

	// This retrieves the newly created AtlasFrame and links it to its ProjectItemData
	QList<QUuid> idList;
	idList.append(pFrame->GetId());
	QList<AtlasFrame *> returnList = RequestFramesById(pItem, idList);

	if(returnList.empty() == false)
		return returnList[0];

	return nullptr;
}

bool AtlasModel::ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack)
{
	if(IsImageValid(newImage, pFrame->GetAtlasGrpId()) == false)
		return false;

	QSet<int> textureIndexToReplaceSet;
	if(pFrame->GetTextureIndex() >= 0)
		textureIndexToReplaceSet.insert(pFrame->GetTextureIndex());

	// First remove the frame from the map
	if(m_FrameLookup.RemoveLookup(pFrame))
		pFrame->DeleteMetaImage(m_MetaDir);

	// Determine the new checksum into the map
	quint32 uiChecksum = HyGlobal::CRCData(0, newImage.bits(), newImage.byteCount());
	pFrame->ReplaceImage(sName, uiChecksum, newImage, m_MetaDir);

	// Re-enter the frame into the map
	m_FrameLookup.RegisterAsset(pFrame);

	if(bDoAtlasGroupRepack)
	{
		uint uiAtlasGrpIndex = GetAtlasGrpIndexFromAtlasGrpId(pFrame->GetAtlasGrpId());
		Repack(uiAtlasGrpIndex, textureIndexToReplaceSet, QSet<AtlasFrame *>());
	}

	return true;
}

void AtlasModel::RepackAll(uint uiAtlasGrpIndex)
{
	int iNumTotalTextures = m_AtlasGrpList[uiAtlasGrpIndex]->GetExistingTextureInfoList().size();
	
	QSet<int> textureIndexSet;
	for(int i = 0; i < iNumTotalTextures; ++i)
		textureIndexSet.insert(i);

	if(textureIndexSet.empty() == false)
		Repack(uiAtlasGrpIndex, textureIndexSet, QSet<AtlasFrame *>());
	else
		SaveRuntime();
}

void AtlasModel::Repack(uint uiAtlasGrpIndex, QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet)
{
	// Always repack the last texture to ensure it gets filled as much as it can
	QFileInfoList existingTexturesInfoList = m_AtlasGrpList[uiAtlasGrpIndex]->GetExistingTextureInfoList();
	for(int i = HyClamp(existingTexturesInfoList.size() - 1, 0, existingTexturesInfoList.size()); i < existingTexturesInfoList.size(); ++i)
		repackTexIndicesSet.insert(i);

	QList<int> textureIndexList = repackTexIndicesSet.toList();

	// Get all the affected frames into a list
	QList<AtlasFrame *> &atlasGrpFrameListRef = m_AtlasGrpList[uiAtlasGrpIndex]->m_FrameList;
	for(int i = 0; i < atlasGrpFrameListRef.size(); ++i)
	{
		for(int j = 0; j < textureIndexList.size(); ++j)
		{
			if(atlasGrpFrameListRef[i]->GetTextureIndex() == textureIndexList[j])
				newFramesSet.insert(atlasGrpFrameListRef[i]);
		}
	}

	QList<AtlasFrame *>newFramesList = newFramesSet.toList();


	AtlasRepackThread *pWorkerThread = new AtlasRepackThread(m_AtlasGrpList[uiAtlasGrpIndex], textureIndexList, newFramesList, m_MetaDir);
	connect(pWorkerThread, &AtlasRepackThread::finished, pWorkerThread, &QObject::deleteLater);
	connect(pWorkerThread, &AtlasRepackThread::LoadUpdate, this, &AtlasModel::OnLoadUpdate);
	connect(pWorkerThread, &AtlasRepackThread::RepackIsFinished, this, &AtlasModel::OnRepackFinished);

	MainWindow::SetLoading("Repacking Atlases", 0);
	pWorkerThread->start();
}

/*virtual*/ QString AtlasModel::OnBankInfo(uint uiBankIndex) /*override*/
{
	QString sInfo = "Num Textures: " % QString::number(GetNumTextures(uiBankIndex)) % " | " %
		"(" % QString::number(GetAtlasDimensions(uiBankIndex).width()) % "x" % QString::number(GetAtlasDimensions(uiBankIndex).height()) % ")" % " | " %
		HyGlobal::AtlasTextureTypeString(GetAtlasTextureType(uiBankIndex));

	return sInfo;
}

/*virtual*/ bool AtlasModel::OnBankSettingsDlg(uint uiBankIndex) /*override*/
{
	QList<AssetItemData *> assetList = m_BanksModel.GetBank(uiBankIndex)->m_AssetList;
	bool bBankHasAssets = assetList.size() > 0;
	bool bAccepted = false;
	DlgAtlasGroupSettings *pDlg = new DlgAtlasGroupSettings(bBankHasAssets, m_BanksModel.GetBank(uiBankIndex)->m_Settings);
	if(QDialog::Accepted == pDlg->exec())
	{
		// Ensure that all current images in atlas group aren't larger than the new atlas itself
		QJsonObject newPackerSettings;
		pDlg->ApplyCurrentSettingsToObj(newPackerSettings);
	
		bool bPackIsValid = true;
		for(int i = 0; i < assetList.size(); ++i)
		{
			AtlasFrame *pFrame = static_cast<AtlasFrame *>(assetList[i]);
			if(IsImageValid(pFrame->GetSize().width(), pFrame->GetSize().height(), newPackerSettings) == false)
			{
				HyGuiLog("Could not save atlas bank settings because image '" % assetList[i]->GetName() % "' will no longer fit on the atlas", LOGTYPE_Warning);
				bPackIsValid = false;
				break;
			}
		}
	
		if(bPackIsValid)
		{
			m_BanksModel.GetBank(uiBankIndex)->m_Settings = newPackerSettings;
	
			if(pDlg->IsSettingsDirty() && bBankHasAssets)
				RepackAll(uiBankIndex);
			else if(pDlg->IsNameChanged() || pDlg->IsSettingsDirty())
				SaveMeta();
	
			bAccepted = true;
		}
	}
	
	delete pDlg;
	//RefreshInfo();
	
	return bAccepted;
}

/*virtual*/ void AtlasModel::OnCreateBank(BankData &newBankRef) /*override*/
{
	m_DataDir.mkdir(HyGlobal::MakeFileNameFromCounter(newBankRef.GetId()));
}

/*virtual*/ void AtlasModel::OnDeleteBank(BankData &bankToBeDeleted) /*override*/
{
	m_DataDir.rmdir(HyGlobal::MakeFileNameFromCounter(bankToBeDeleted.GetId()));
}

/*virtual*/ AssetItemData *AtlasModel::OnAllocateAssetData(QJsonObject metaObj) /*override*/
{
	QRect rAlphaCrop(QPoint(metaObj["cropLeft"].toInt(), metaObj["cropTop"].toInt()),
					 QPoint(metaObj["cropRight"].toInt(), metaObj["cropBottom"].toInt()));

	

	AtlasFrame *pNewFrame = new AtlasFrame(*this,
										   HyGlobal::GetItemFromAtlasItem(static_cast<AtlasItemType>(metaObj["type"].toInt())),
										   QUuid(metaObj["frameUUID"].toString()),
										   JSONOBJ_TOINT(metaObj, "checksum"),
										   JSONOBJ_TOINT(metaObj, "atlasGrpId"),
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

/*virtual*/ AssetItemData *AtlasModel::OnAllocateAssetData(QString sFilePath, quint32 uiBankId, HyGuiItemType eType) /*override*/
{
	//QList<QImage *> newImageList;
	//for(int i = 0; i < sImportList.size(); ++i)
	//{
	QFileInfo fileInfo(sFilePath);

	QImage *pNewImage = new QImage(fileInfo.absoluteFilePath());
	//newImageList.push_back(pNewImage);

	QSize atlasDimensions = GetAtlasDimensions(GetBankIndexFromBankId(uiBankId));
	if(IsImageValid(*pNewImage, uiBankId) == false)
	{
		HyGuiLog("Importing image " % fileInfo.fileName() % " will not fit in atlas bank '" % QString::number(uiBankId) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);
		delete pNewImage;
		return nullptr;
	}
	//}


	//for(int i = 0; i < newImageList.size(); ++i)
		//returnSet.insert(ImportImage(QFileInfo(sImportImgList[i]).baseName(), *newImageList[i], uiAtlasGrpId, eType, correspondingParentList[i]));

	quint32 uiChecksum = HyGlobal::CRCData(0, pNewImage->bits(), pNewImage->byteCount());

	AtlasItemType eAtlasItemType = HyGlobal::GetAtlasItemFromItem(eType);

	QRect rAlphaCrop(0, 0, pNewImage->width(), pNewImage->height());
	if(eAtlasItemType == ATLASITEM_Image) // 'sub-atlases' should not be cropping their alpha because they rely on their own UV coordinates
		rAlphaCrop = ImagePacker::crop(*pNewImage);

	AtlasFrame *pNewFrame = new AtlasFrame(*this,
										   eType,
										   QUuid::createUuid(),
										   uiChecksum,
										   uiBankId,
										   fileInfo.baseName(),
										   rAlphaCrop,
										   pNewImage->width(),
										   pNewImage->height(),
										   -1,
										   -1,
										   -1,
										   0);

	pNewImage->save(m_MetaDir.absoluteFilePath(pNewFrame->ConstructMetaFileName()));
	delete pNewImage;

	return pNewFrame;
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
}

/*virtual*/ bool AtlasModel::OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) /*override*/
{
	// Ensure all new replacement images will fit on the specified atlas
	QList<QImage *> newReplacementImageList;
	for(int i = 0; i < assetList.count(); ++i)
	{
		QFileInfo fileInfo(sImportAssetList[i]);
		QImage *pNewImage = new QImage(fileInfo.absoluteFilePath());
		QSize atlasDimensions = GetAtlasDimensions(GetBankIndexFromBankId(assetList[i]->GetBankId()));
		if(IsImageValid(*pNewImage, assetList[i]->GetBankId()) == false)
		{
			HyGuiLog("Replacement image " % fileInfo.fileName() % " will not fit in atlas group '" % QString::number(assetList[i]->GetBankId()) % "' (" % QString::number(atlasDimensions.width()) % "x" % QString::number(atlasDimensions.height()) % ")", LOGTYPE_Warning);

			for(int j = 0; j < newReplacementImageList.size(); ++j)
				delete newReplacementImageList[j];

			return;
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
	QJsonArray atlasGrpArray;
	for(int i = 0; i < m_BanksModel.rowCount(); ++i)
	{
		QJsonObject atlasGrpObj;
		atlasGrpObj.insert("width", m_BanksModel.GetBank(i)->m_Settings["sbTextureWidth"].toInt());
		atlasGrpObj.insert("height", m_BanksModel.GetBank(i)->m_Settings["sbTextureHeight"].toInt());
		atlasGrpObj.insert("atlasGrpId", m_BanksModel.GetBank(i)->m_Settings["atlasGrpId"].toInt());
		atlasGrpObj.insert("textureType", m_BanksModel.GetBank(i)->m_Settings["textureType"].toInt());

		QJsonArray textureArray;
		QList<QJsonArray> frameArrayList;
		QList<AssetItemData *> &atlasGrpFrameListRef = m_BanksModel.GetBank(i)->m_AssetList;
		for(int i = 0; i < atlasGrpFrameListRef.size(); ++i)
		{
			AtlasFrame *pAtlasFrame = static_cast<AtlasFrame *>(atlasGrpFrameListRef[i]);
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

		for(int i = 0; i < frameArrayList.size(); ++i)
			textureArray.append(frameArrayList[i]);

		atlasGrpObj.insert("textures", textureArray);

		atlasGrpArray.append(atlasGrpObj);
	}

	QJsonObject atlasInfoObj;
	atlasInfoObj.insert("$fileVersion", HYGUI_FILE_VERSION);
	atlasInfoObj.insert("atlasGroups", atlasGrpArray);

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
