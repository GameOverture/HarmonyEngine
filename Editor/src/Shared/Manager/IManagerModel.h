/**************************************************************************
 *	IManagerModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IMANAGERMODEL_H
#define IMANAGERMODEL_H

#include "Global.h"
#include "ITreeModel.h"
#include "IAssetItemData.h"
#include "BanksModel.h"

#include <QUuid>

class IManagerDraw;

class IManagerModel : public ITreeModel
{
	Q_OBJECT

protected:
	BanksModel									m_BanksModel;

	Project &									m_ProjectRef;
	const HyGuiItemType							m_eITEM_TYPE;

	QDir										m_MetaDir;
	QDir										m_DataDir;

	quint32										m_uiNextBankId;
	QJsonArray									m_ExpandedFiltersArray;

	QMap<QUuid, AssetItemData *>				m_AssetUuidMap;
	QMap<quint32, QList<AssetItemData *> >		m_AssetChecksumMap;

public:
	IManagerModel(Project &projRef, HyGuiItemType eItemType);
	virtual ~IManagerModel();

	void Init();

	Project &GetProjOwner();
	QAbstractListModel *GetBanksModel();

	QDir GetMetaDir();
	QDir GetDataDir();

	int GetNumBanks();
	QString GetBankName(uint uiBankIndex);
	QJsonObject GetBankSettings(uint uiBankIndex);
	void SetBankSettings(uint uiBankIndex, QJsonObject newSettingsObj);
	QList<AssetItemData *> GetBankAssets(uint uiBankIndex);

	bool ImportNewAssets(QStringList sImportList, quint32 uiBankId, HyGuiItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList);
	void RemoveItems(QList<AssetItemData *> assetsList, QList<TreeModelItemData *> filtersList);
	void ReplaceAssets(QList<AssetItemData *> assetsList);
	void Rename(TreeModelItemData *pItem, QString sNewName);
	bool TransferAssets(QList<AssetItemData *> assetsList, uint uiNewBankId);

	QJsonArray GetExpandedFiltersArray();
	QString AssembleFilter(AssetItemData *pAsset) const;
	TreeModelItemData *FindTreeItemFilter(TreeModelItemData *pItem) const;
	TreeModelItemData *ReturnFilter(QString sFilterPath); // Will create if doesn't exist

	bool RemoveLookup(AssetItemData *pAsset); // Returns true if no remaining duplicates exist
	AssetItemData *FindById(QUuid uuid);
	QList<AssetItemData *> FindByChecksum(quint32 uiChecksum);
	bool DoesAssetExist(quint32 uiChecksum);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AssetItemData *CreateAsset(QUuid uuid, quint32 uiCRC, int32 uiBankId, QString sName);//, QRect rAlphaCrop, AtlasItemType eFrameType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors);
	
	

	//QList<AssetItemData *> RequestAssets(ProjectItemData *pItem);
	QList<AssetItemData *> RequestAssets(ProjectItemData *pItem, QList<AssetItemData *> requestList);
	QList<AssetItemData *> RequestAssetsByUuid(ProjectItemData *pItem, QList<QUuid> requestList);
	void RelinquishAssets(ProjectItemData *pItem, QList<AssetItemData *> relinquishList);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	TreeModelItemData *CreateNewFilter(QString sName, TreeModelItemData *pParent);

	void CreateNewBank(QString sName);
	void RemoveBank(quint32 uiBankId);

	uint GetBankIndexFromBankId(quint32 uiBankId) const;
	quint32 GetBankIdFromBankIndex(uint uiBankIndex) const;

	void SaveMeta();
	void SaveRuntime(); // Saves meta, outputs runtime assets, and reloads Harmony in the editor

	virtual QVariant data(const QModelIndex &indexRef, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& indexRef) const override;
	virtual Qt::DropActions supportedDragActions() const override;
	virtual Qt::DropActions supportedDropActions() const override;
	virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
	virtual QStringList mimeTypes() const override;
	virtual bool canDropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) const override;
	virtual bool dropMimeData(const QMimeData *pData, Qt::DropAction eAction, int iRow, int iColumn, const QModelIndex &parentRef) override;

	virtual QString OnBankInfo(uint uiBankIndex) = 0;
	virtual bool OnBankSettingsDlg(uint uiBankIndex) = 0;
	virtual QStringList GetSupportedFileExtList() = 0;

	// Draw occur when the mouse hovers over the manager widget. ManagerWidget holds the ptr to IManagerDraw, but IManagerModel init/updates the actual concrete IDraw object
	virtual void OnAllocateDraw(IManagerDraw *&pDrawOut) = 0;

protected:
	void RegisterAsset(AssetItemData *pAsset);
	void DeleteAsset(AssetItemData *pAsset);
	void MoveAsset(AssetItemData *pAsset, quint32 uiNewBankId);

	virtual AssetItemData *OnAllocateAssetData(QJsonObject metaObj) = 0;

	virtual QList<AssetItemData *> OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType, QList<QUuid> correspondingUuidList) = 0; // Must call RegisterAsset() on each asset
	virtual bool OnRemoveAssets(QList<AssetItemData *> assetList) = 0; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) = 0;
	virtual bool OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) = 0; // Must call MoveAsset() on each asset

	virtual QJsonObject GetSaveJson() = 0;

private:
	AssetItemData *CreateAssetTreeItem(const QString sPrefix, const QString sName, QJsonObject metaObj);
	//TreeModelItem *FindFilterTreeItem(const QModelIndex &indexRef) const;
};

#endif // IMANAGERMODEL_H
