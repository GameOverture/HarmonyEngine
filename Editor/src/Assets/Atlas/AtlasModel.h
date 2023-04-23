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

	QFileInfoList GetExistingTextureInfoList(uint uiBankIndex);

	int GetNumTextures(uint uiBankIndex);
	QSize GetAtlasDimensions(uint uiBankIndex);

	bool IsImageValid(QImage &image, quint32 uiBankId);
	bool IsImageValid(int iWidth, int iHeight, quint32 uiBankId);
	bool IsImageValid(int iWidth, int iHeight, const QJsonObject &atlasSettings);

	AtlasFrame *GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpIndex, ItemType eType);
	bool ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage);
	
	virtual QString OnBankInfo(uint uiBankIndex) override;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) override;
	virtual QStringList GetSupportedFileExtList() const override;

protected:
	virtual void OnInit() override;
	virtual void OnCreateNewBank(QJsonObject &newMetaBankObjRef) override;
	virtual IAssetItemData *OnAllocateAssetData(QJsonObject metaObj) override;

	virtual void OnGenerateAssetsDlg(const QModelIndex &indexDestination) override;
	virtual QList<IAssetItemData *> OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, ItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) override; // Must call RegisterAsset() on each asset
	virtual bool OnRemoveAssets(QStringList sPreviousFilterPaths, QList<IAssetItemData *> assetList) override; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<IAssetItemData *> assetList) override;
	virtual bool OnUpdateAssets(QList<IAssetItemData *> assetList) override;
	virtual bool OnMoveAssets(QList<IAssetItemData *> assetsList, quint32 uiNewBankId) override; // Must call MoveAsset() on each asset

	virtual void OnFlushRepack() override;

	virtual void OnSaveMeta(QJsonObject &metaObjRef) override;
	virtual QJsonObject GetSaveJson() override;

	void AddTexturesToRepack(BankData *pBankData, QSet<int> texIndicesSet);

private:
	AtlasFrame *ImportImage(QString sName, QImage &newImage, quint32 uiBankIndex, ItemType eType, QUuid uuid);
};

#endif // ATLASMODEL_H
