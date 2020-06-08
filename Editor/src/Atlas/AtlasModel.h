/**************************************************************************
 *	AtlasModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASMODEL_H
#define ATLASMODEL_H

#include "IManagerModel.h"

#include "AtlasFrame.h"

class AtlasModel : public IManagerModel
{
	Q_OBJECT

public:
	AtlasModel(Project &projRef);
	virtual ~AtlasModel();

	//void StashTreeWidgets(QList<AtlasTreeItem *> treeItemList);
	//QList<AtlasTreeItem *> GetTopLevelTreeItemList();

	QFileInfoList GetExistingTextureInfoList(uint uiBankIndex);

	int GetNumTextures(uint uiBankIndex);
	QSize GetAtlasDimensions(uint uiBankIndex);
	HyTextureFormat GetAtlasTextureType(uint uiBankIndex);

	bool IsImageValid(QImage &image, quint32 uiAtlasGrpId);
	bool IsImageValid(int iWidth, int iHeight, quint32 uiAtlasGrpId);
	bool IsImageValid(int iWidth, int iHeight, const QJsonObject &atlasSettings);

	AtlasFrame *GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpIndex, HyGuiItemType eType);
	bool ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

	void RepackAll(uint uiAtlasGrpIndex);
	void Repack(uint uiAtlasGrpIndex, QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet);
	
	virtual QString OnBankInfo(uint uiBankIndex) override;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) override;
	virtual QStringList GetSupportedFileExtList() override;

protected:
	virtual void OnCreateBank(BankData &newBankRef) override;
	virtual void OnDeleteBank(BankData &bankToBeDeleted) override;
	virtual AssetItemData *OnAllocateAssetData(QJsonObject metaObj) override;

	virtual QList<AssetItemData *> OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType) override; // Must call RegisterAsset() on each asset
	virtual bool OnRemoveAssets(QList<AssetItemData *> assetList) override; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) override;
	virtual bool OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) override; // Must call MoveAsset() on each asset

	virtual QJsonObject GetSaveJson() override;

private Q_SLOTS:
	void OnLoadUpdate(QString sMsg, int iPercComplete);
	void OnRepackFinished();
};

//struct AtlasGrp
//{
//	QDir										m_DataDir;
//
//	QJsonObject									m_PackerSettings;
//	
//	QList<AtlasFrame *>							m_FrameList;
//
//	AtlasGrp(QString sAbsDataDirPath) : m_DataDir(sAbsDataDirPath)
//	{ }
//
//	quint32 GetId()
//	{
//		return JSONOBJ_TOINT(m_PackerSettings, "atlasGrpId");
//	}
//
//	QFileInfoList GetExistingTextureInfoList()
//	{
//		return m_DataDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
//	}
//
//	
//};

#endif // ATLASMODEL_H
