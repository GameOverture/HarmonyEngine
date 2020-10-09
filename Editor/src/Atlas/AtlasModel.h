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

	AtlasFrame *GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpIndex, HyGuiItemType eType);
	bool ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, bool bDoAtlasGroupRepack);

	void RepackAll(uint uiBankIndex);
	void Repack(uint uiBankIndex, QSet<int> repackTexIndicesSet, QSet<AtlasFrame *> newFramesSet);
	
	virtual void OnNewBankDefaults(QJsonObject &bankObjRef) override;
	virtual QString OnBankInfo(uint uiBankIndex) override;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) override;
	virtual QStringList GetSupportedFileExtList() override;

	virtual void OnAllocateDraw(IManagerDraw *&pDrawOut) override;

protected:
	virtual AssetItemData *OnAllocateAssetData(QJsonObject metaObj) override;

	virtual QList<AssetItemData *> OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType, QList<QUuid> correspondingUuidList) override; // Must call RegisterAsset() on each asset
	virtual bool OnRemoveAssets(QList<AssetItemData *> assetList) override; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) override;
	virtual bool OnUpdateAssets(QList<AssetItemData *> assetList) override;
	virtual bool OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) override; // Must call MoveAsset() on each asset

	virtual QJsonObject GetSaveJson() override;

private:
	AtlasFrame *ImportImage(QString sName, QImage &newImage, quint32 uiBankIndex, HyGuiItemType eType, QUuid uuid);
};

#endif // ATLASMODEL_H
