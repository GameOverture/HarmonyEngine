/**************************************************************************
 *	AtlasManager.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASMANAGER_H
#define ATLASMANAGER_H

#include "IManagerModel.h"

#include "AtlasFrame.h"
#include "AtlasTileSet.h"
#include "TileSetsTreeModel.h"

class AtlasManager : public IManagerModel
{
	Q_OBJECT

	friend class AtlasImportThread;

	QMap<BankData *, QSet<int>>			m_RepackTexIndicesMap;	// This exists to keep track of affected texture indices. When deleting a large number of frames where they encompass the entire texture, the repack will delete the frames before clearing out the affected textures. This caused those textures to be left behind
	
	HyTextureInfo						m_DefaultTextureInfo;
	
	TileSetsTreeModel					m_TileSetsTreeModel;
	QJsonObject							m_TileSetsMeta;			// The currently 'saved to disk' meta-data of the TileSets

public:
	AtlasManager(Project &projRef);
	virtual ~AtlasManager();

	QFileInfoList GetExistingTextureInfoList(uint uiBankIndex) const;

	int GetNumTextures(uint uiBankIndex) const;
	QSize GetMaxAtlasDimensions(uint uiBankIndex) const;
	QSize GetTextureSize(uint uiBankIndex, int iTextureIndex) const; // May be smaller (when trimmed) than the max atlas dimensions

	bool IsImageValid(QImage &image, quint32 uiBankId) const;
	bool IsImageValid(int iWidth, int iHeight, quint32 uiBankId) const;
	bool IsImageValid(int iWidth, int iHeight, const QJsonObject &atlasSettings) const;

	AtlasFrame *FindFrame(quint32 uiChecksum, quint32 uiBankId) const;
	AtlasFrame *FindFrame(const QUuid &itemUuidRef, quint32 uiBankId) const;
	AtlasFrame *GenerateFrame(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiAtlasGrpIndex, ItemType eSubAtlasType);
	bool ReplaceFrame(AtlasFrame *pFrame, QString sName, QImage &newImage, ItemType eSubAtlasType);

	const TileSetsTreeModel &GetTileSetsModel() const;
	TileSetsTreeModel &GetTileSetsModel();
	AtlasTileSet *GenerateTileSet(QString sName, TreeModelItemData *pParentTreeItemData, quint32 uiBankId);
	bool SaveTileSets();
	void WriteTileSetsToDisk();

	void OnSliceSprite(quint32 uiDestinationBankId, TreeModelItemData *pFirstSelection);
	
	virtual QString OnBankInfo(uint uiBankIndex) override;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) override;
	virtual QStringList GetSupportedFileExtList() const override;

protected:
	virtual void OnInit() override;
	virtual void OnCreateNewBank(QJsonObject &newMetaBankObjRef) override;
	virtual IAssetItemData *OnAllocateAssetData(QJsonObject metaObj) override;

	virtual bool OnRemoveAssets(QStringList sPreviousFilterPaths, QList<IAssetItemData *> assetList) override; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<IAssetItemData *> assetList) override;
	virtual bool OnUpdateAssets(QList<IAssetItemData *> assetList) override;
	virtual bool OnMoveAssets(QList<IAssetItemData *> assetsList, quint32 uiNewBankId) override; // Must call MoveAsset() on each asset

	virtual void OnFlushRepack() override;

	virtual void OnSaveMeta(QJsonObject &metaObjRef) override;
	virtual void OnSaveData(QJsonObject &dataObjRef) override;

	void AddTexturesToRepack(BankData *pBankData, QSet<int> texIndicesSet);

private:
	AtlasFrame *ImportImage(QString sName, QImage &newImage, quint32 uiBankIndex, ItemType eSubAtlasType, QUuid uuid);
};

#endif // ATLASMANAGER_H
