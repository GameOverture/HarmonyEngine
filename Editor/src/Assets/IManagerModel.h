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
	const AssetManagerType									m_eASSET_TYPE;

	bool											m_bIsSingleBank;

	QDir											m_MetaDir;
	QDir											m_DataDir;

	quint32											m_uiNextBankId;

	QMap<quint32, QList<IAssetItemData *> >			m_AssetChecksumMap;

	QMap<BankData *, QSet<IAssetItemData *>>			m_RepackAffectedAssetsMap;
	QList<ProjectItemData *>						m_RepackAffectedItemList; // Keep track of any linked/referenced items as they will need to be re-saved after asset repacking

public:
	IManagerModel(Project &projRef, AssetManagerType eAssetType);
	virtual ~IManagerModel();

	void Init();	// Init() exists because we need to construct using virtual functions (or after derived ctor initialization)

	AssetManagerType GetAssetType() const;
	Project &GetProjOwner() const;
	QAbstractListModel *GetBanksModel();
	bool IsSingleBank() const;

	QDir GetMetaDir();
	QDir GetDataDir();

	int GetNumBanks();
	QString GetBankName(uint uiBankIndex);
	QJsonObject GetBankSettings(uint uiBankIndex);
	void SetBankSettings(uint uiBankIndex, QJsonObject newSettingsObj);
	QList<IAssetItemData *> GetBankAssets(uint uiBankIndex);

	void GenerateAssetsDlg(const QModelIndex &indexDestination);
	bool ImportNewAssets(QStringList sImportList, quint32 uiBankId, ItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList);
	void RemoveItems(QList<IAssetItemData *> assetsList, QList<TreeModelItemData *> filtersList);
	bool CanReplaceAssets(QList<IAssetItemData *> assetsList, QList<ProjectItemData *> &affectedItemListOut) const;
	void ReplaceAssets(QList<IAssetItemData *> assetsList, bool bWithNewAssets);
	void Rename(TreeModelItemData *pItem, QString sNewName);
	bool TransferAssets(QList<IAssetItemData *> assetsList, uint uiNewBankId);

	void AddAssetsToRepack(BankData *pBankData);
	void AddAssetsToRepack(BankData *pBankData, IAssetItemData *pAsset);
	void AddAssetsToRepack(BankData *pBankData, QSet<IAssetItemData *> &assetsSet);
	void FlushRepack();

	QString AssembleFilter(TreeModelItemData *pAsset, bool bIncludeSelfIfFilter) const;
	TreeModelItemData *FindTreeItemFilter(TreeModelItemData *pItem) const;
	TreeModelItemData *ReturnFilter(QString sFilterPath, bool bCreateNonExistingFilter = true);

	bool RemoveLookup(IAssetItemData *pAsset); // Returns true if no remaining duplicates exist
	QList<IAssetItemData *> FindByChecksum(quint32 uiChecksum);
	bool DoesAssetExist(quint32 uiChecksum);

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

protected:
	void RegisterAsset(IAssetItemData *pAsset);
	void DeleteAsset(IAssetItemData *pAsset);
	void MoveAsset(IAssetItemData *pAsset, quint32 uiNewBankId);

	void StartRepackThread(QString sLoadMessage, IRepackThread *pRepackThread);

	virtual void OnInit() = 0;
	virtual void OnCreateNewBank(QJsonObject &newBankMetaBankRef) = 0;

	virtual IAssetItemData *OnAllocateAssetData(QJsonObject metaObj) = 0;

	virtual void OnGenerateAssetsDlg(const QModelIndex &indexDestination) = 0;
	virtual QList<IAssetItemData *> OnImportAssets(QStringList sImportAssetList, quint32 uiBankId, ItemType eType, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList) = 0; // Must call RegisterAsset() on each asset
	virtual bool OnRemoveAssets(QStringList sPreviousFilterPaths, QList<IAssetItemData *> assetList) = 0; // Must call DeleteAsset() on each asset
	virtual bool OnReplaceAssets(QStringList sImportAssetList, QList<IAssetItemData *> assetList) = 0;
	virtual bool OnUpdateAssets(QList<IAssetItemData *> assetList) = 0;
	virtual bool OnMoveAssets(QList<IAssetItemData *> assetsList, quint32 uiNewBankId) = 0; // Must call MoveAsset() on each asset

	virtual void OnFlushRepack() = 0;

	virtual void OnSaveMeta(QJsonObject &metaObjRef) = 0;
	virtual QJsonObject GetSaveJson() = 0;

private:
	IAssetItemData *CreateAssetTreeItem(QString sPrefix, QString sName, QJsonObject metaObj);

protected Q_SLOTS:
	void OnLoadUpdate(QString sMsg, int iPercComplete);
	void OnRepackFinished();
};

#endif // IMANAGERMODEL_H
