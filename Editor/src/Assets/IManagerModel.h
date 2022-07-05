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
#include "IRepackThread.h"

#include <QUuid>

class IManagerDraw;

class IManagerModel : public ITreeModel
{
	Q_OBJECT

protected:
	BanksModel										m_BanksModel;

	Project &										m_ProjectRef;
	const AssetType									m_eASSET_TYPE;

	bool											m_bIsSingleBank;

	QDir											m_MetaDir;
	QDir											m_DataDir;

	quint32											m_uiNextBankId;

	QMap<QUuid, AssetItemData *>					m_AssetUuidMap;
	QMap<quint32, QList<AssetItemData *> >			m_AssetChecksumMap;

	QMap<BankData *, QSet<AssetItemData *>>			m_RepackAffectedAssetsMap;
	QList<ProjectItemData *>						m_RepackAffectedItemList; // Keep track of any linked/referenced items as they will need to be re-saved after asset repacking

public:
	IManagerModel(Project &projRef, AssetType eAssetType);
	virtual ~IManagerModel();

	void Init();	// Init() exists because we need to construct using virtual functions (or after derived ctor initialization)

	AssetType GetAssetType() const;
	Project &GetProjOwner() const;
	QAbstractListModel *GetBanksModel();
	bool IsSingleBank() const;

	QDir GetMetaDir();
	QDir GetDataDir();

	int GetNumBanks();
	QString GetBankName(uint uiBankIndex);
	QJsonObject GetBankSettings(uint uiBankIndex);
	void SetBankSettings(uint uiBankIndex, QJsonObject newSettingsObj);
	QList<AssetItemData *> GetBankAssets(uint uiBankIndex);

	void GenerateAssetsDlg(const QModelIndex &indexDestination);
	bool ImportNewAssets(QStringList sImportList, quint32 uiBankId, HyGuiItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList);
	void RemoveItems(QList<AssetItemData *> assetsList, QList<TreeModelItemData *> filtersList);
	void ReplaceAssets(QList<AssetItemData *> assetsList, bool bWithNewAssets);
	void Rename(TreeModelItemData *pItem, QString sNewName);
	bool TransferAssets(QList<AssetItemData *> assetsList, uint uiNewBankId);

	void AddAssetsToRepack(BankData *pBankData);
	void AddAssetsToRepack(BankData *pBankData, AssetItemData *pAsset);
	void AddAssetsToRepack(BankData *pBankData, QSet<AssetItemData *> &assetsSet);
	void FlushRepack();

	QString AssembleFilter(TreeModelItemData *pAsset, bool bIncludeSelfIfFilter) const;
	TreeModelItemData *FindTreeItemFilter(TreeModelItemData *pItem) const;
	TreeModelItemData *ReturnFilter(QString sFilterPath, bool bCreateNonExistingFilter = true);

	bool RemoveLookup(AssetItemData *pAsset); // Returns true if no remaining duplicates exist
	AssetItemData *FindById(QUuid uuid);
	QList<AssetItemData *> FindByChecksum(quint32 uiChecksum);
	bool DoesAssetExist(quint32 uiChecksum);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [ASSET-ITEM] DEPENDENCIES LINK FUNCTIONS
	QList<AssetItemData *> RequestAssetsByUuid(ProjectItemData *pItem, QList<QUuid> requestList);
	QList<AssetItemData *> RequestAssets(ProjectItemData *pItem, QList<AssetItemData *> requestList);
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
	virtual QStringList GetSupportedFileExtList() const = 0;

	// Draw occurs when the mouse hovers over the manager widget. ManagerWidget holds the ptr to IManagerDraw, but IManagerModel init/updates the actual concrete IDraw object
	virtual void OnAllocateDraw(IManagerDraw *&pDrawOut) = 0;

protected:
	void RegisterAsset(AssetItemData *pAsset);
	void DeleteAsset(AssetItemData *pAsset);
	void MoveAsset(AssetItemData *pAsset, quint32 uiNewBankId);

	void StartRepackThread(QString sLoadMessage, IRepackThread *pRepackThread);

	virtual void OnInit() = 0;
	virtual void OnCreateNewBank(QJsonObject &newBankMetaBankRef) = 0;

	virtual AssetItemData *OnAllocateAssetData(QJsonObject metaObj) = 0;

	virtual void OnGenerateAssetsDlg(const QModelIndex &indexDestination) = 0;
	virtual QList<AssetItemData *> OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, HyGuiItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) = 0; // Must call RegisterAsset() on each asset
	virtual bool OnRemoveAssets(QList<AssetItemData *> assetList) = 0; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<AssetItemData *> assetList) = 0;
	virtual bool OnUpdateAssets(QList<AssetItemData *> assetList) = 0;
	virtual bool OnMoveAssets(QList<AssetItemData *> assetsList, quint32 uiNewBankId) = 0; // Must call MoveAsset() on each asset

	virtual void OnFlushRepack() = 0;

	virtual void OnSaveMeta() = 0;
	virtual QJsonObject GetSaveJson() = 0;

private:
	AssetItemData *CreateAssetTreeItem(QString sPrefix, QString sName, QJsonObject metaObj);

protected Q_SLOTS:
	void OnLoadUpdate(QString sMsg, int iPercComplete);
	void OnRepackFinished();
};

#endif // IMANAGERMODEL_H
