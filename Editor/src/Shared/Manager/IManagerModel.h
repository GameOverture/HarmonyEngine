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

#include <QUuid>

class IManagerModel : public ITreeModel
{
	Q_OBJECT

protected:
	struct BankData
	{
		QString									m_sAbsPath;
		QJsonObject								m_Settings;
		QList<AssetItemData *>					m_AssetList;

		BankData(QString sAbsDataDirPath, QJsonObject settingsObj) :
			m_sAbsPath(sAbsDataDirPath),
			m_Settings(settingsObj)
		{ }

		~BankData()
		{
			for(int i = 0; i < m_AssetList.size(); ++i)
				delete m_AssetList[i];
		}

		quint32 GetId() const {
			// TODO: rename to bankId
			if(m_Settings.contains("atlasGrpId") == false) {
				HyGuiLog("BankData::GetId could not find 'bankId' in bank's settings", LOGTYPE_Error);
			}
			// TODO: rename to bankId
			return m_Settings["atlasGrpId"].toInt();
		}

		QString GetName() const {
			// TODO: rename to bankName
			return m_Settings["txtName"].toString();
		}
	};
	class BanksModel : public QAbstractListModel
	{
		IManagerModel &							m_ModelRef;
		QList<BankData *>						m_BankList;

	public:
		BanksModel(IManagerModel &modelRef) :
			m_ModelRef(modelRef)
		{
		}

		virtual ~BanksModel()
		{
			for(int i = 0; i < m_BankList.size(); ++i)
				delete m_BankList[i];
		}

		BankData *GetBank(uint uiIndex)
		{
			return m_BankList[uiIndex];
		}

		void AppendBank(QString sAbsPath, QJsonObject settingsObj)
		{
			beginInsertRows(QModelIndex(), m_BankList.count(), m_BankList.count());
			BankData *pNewBank = new BankData(sAbsPath, settingsObj);
			m_BankList.push_back(pNewBank);
			endInsertRows();

			m_ModelRef.OnCreateBank(*pNewBank);
		}
		
		void RemoveBank(uint uiIndex)
		{
			BankData *pBankToBeRemoved = m_BankList[uiIndex];
			m_ModelRef.OnDeleteBank(*pBankToBeRemoved);
			
			beginRemoveRows(QModelIndex(), uiIndex, uiIndex);
			m_BankList.removeAt(uiIndex);
			endRemoveRows();

			delete pBankToBeRemoved;
			
		}

		int GetIndex(BankData *pData) const
		{
			for(uint32 i = 0; i < m_BankList.size(); ++i)
			{
				if(pData == m_BankList[i])
					return i;
			}
			return -1;
		}

		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const {
			return m_BankList.size();
		}
		virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
			if (role == Qt::TextAlignmentRole)
				return Qt::AlignLeft;

			if(role == Qt::DisplayRole || role == Qt::EditRole)
				return "Bank: " % QString::number(GetIndex(m_BankList[index.row()])) % " - " % m_BankList[index.row()]->m_Settings["Name"].toString();

			return QVariant();
		}
		virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const {
			return QVariant();
		}
	};
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

	Project &GetProjOwner();

	QDir GetMetaDir();
	QDir GetDataDir();

	int GetNumBanks();
	QString GetBankName(uint uiBankIndex);
	QJsonObject GetBankSettings(uint uiBankIndex);
	void SetBankSettings(uint uiBankIndex, QJsonObject newSettingsObj);

	QJsonArray GetExpandedFiltersArray();
	QString AssembleFilter(const AssetItemData *pAsset) const;

	bool RemoveLookup(AssetItemData *pAsset); // Returns true if no remaining duplicates exist
	AssetItemData *FindById(QUuid uuid);
	QList<AssetItemData *> FindByChecksum(quint32 uiChecksum);
	bool DoesAssetExist(quint32 uiChecksum);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AssetItemData *CreateAsset(QUuid uuid, quint32 uiCRC, int32 uiBankId, QString sName);//, QRect rAlphaCrop, AtlasItemType eFrameType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors);
	void RemoveAsset(AssetItemData *pAsset);
	bool TransferAsset(AssetItemData *pAsset, uint uiNewBankId);

	//QList<AssetItemData *> RequestAssets(ProjectItemData *pItem);
	QList<AssetItemData *> RequestAssets(ProjectItemData *pItem, QList<AssetItemData *> requestList);
	QList<AssetItemData *> RequestAssetsByUuid(ProjectItemData *pItem, QList<QUuid> requestList);
	void RelinquishAssets(ProjectItemData *pItem, QList<AssetItemData *> relinquishList);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QSet<AssetItemData *> ImportNewAssets(QStringList sImportList, quint32 uiBankId, HyGuiItemType eType, QList<TreeModelItem *> correspondingParentList);
	bool CreateNewFilter(QString sName, TreeModelItem *pParent);

	uint CreateNewBank(QString sName);
	void RemoveBank(quint32 uiBankId);

	uint GetBankIndexFromBankId(quint32 uiBankId);
	quint32 GetBankIdFromBankIndex(uint uiBankIndex);

	void SaveMeta();
	void SaveRuntime(); // Saves meta, outputs runtime assets, and reloads Harmony in the editor

protected:
	virtual void OnCreateBank(BankData &newBankRef) = 0;
	virtual void OnDeleteBank(BankData &bankToBeDeleted) = 0;
	
	virtual AssetItemData *OnAllocateAssetData(QJsonObject metaObj) = 0;
	virtual AssetItemData *OnAllocateAssetData(QString sFilePath, quint32 uiBankId, HyGuiItemType eType) = 0;

	virtual QJsonObject GetSaveJson() = 0;

private:
	AssetItemData *CreateAssetTreeItem(const QString sPrefix, const QString sName, QJsonObject metaObj);
	void RegisterAsset(AssetItemData *pAsset);
};

#endif // IMANAGERMODEL_H
