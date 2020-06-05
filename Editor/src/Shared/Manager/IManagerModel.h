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
#include "ProjectItemData.h"
#include "TreeModelItemData.h"

#include <QUuid>

class AssetItemData : public TreeModelItemData
{
	IManagerModel &						m_ModelRef;
	const QUuid							m_UNIQUE_ID;
	quint32								m_uiChecksum;
	quint32								m_uiBankId;
	QString								m_sName;
	QString								m_sMetaFileExt; // includes period

	QSet<ProjectItemData *>				m_DependencySet;
	uint								m_uiErrors;

public:
	AssetItemData(IManagerModel &modelRef, QUuid uuid, quint32 uiChecksum, quint32 uiBankId, QString sName, QString sMetaFileExt, uint uiErrors) :
		m_ModelRef(modelRef),
		m_UNIQUE_ID(uuid),
		m_uiChecksum(uiChecksum),
		m_uiBankId(uiBankId),
		m_sName(sName),
		m_sMetaFileExt(sMetaFileExt),
		m_uiErrors(uiErrors) // '0' when there is no error
	{
	}

	const QUuid &GetUuid() {
		return m_UNIQUE_ID;
	}

	quint32 GetChecksum() {
		return m_uiChecksum;
	}

	quint32 GetBankId() {
		return m_uiBankId;
	}
	void SetBankId(quint32 uiNewBankId) {
		m_uiBankId = uiNewBankId;
	}

	QString GetFilter() const
	{
		return m_ModelRef.AssembleFilter(this);
	}

	QString GetName() const {
		return m_sName;
	}
	void SetName(QString sNewName) {
		m_sName = sNewName;
	}

	QSet<ProjectItemData *> GetLinks() {
		return m_DependencySet;
	}

	QString ConstructMetaFileName()
	{
		QString sMetaName;
		sMetaName = sMetaName.sprintf("%010u", m_uiChecksum);
		sMetaName += m_sMetaFileExt;

		return sMetaName;
	}

	void SetError(AtlasFrameError eError)
	{
		if(eError == ATLASFRAMEERROR_CannotFindMetaImg)
			HyGuiLog(m_sName % " - GUIFRAMEERROR_CannotFindMetaImg", LOGTYPE_Error);

		m_uiErrors |= (1 << eError);

		//UpdateTreeItemIconAndToolTip();
	}

	void ClearError(AtlasFrameError eError)
	{
		m_uiErrors &= ~(1 << eError);

		//UpdateTreeItemIconAndToolTip();
	}

	uint GetErrors()
	{
		return m_uiErrors;
	}
};

class IManagerModel : public ITreeModel
{
	Q_OBJECT

	struct BankData
	{
		QDir								m_Dir;
		QJsonObject							m_Settings;
		QList<AssetItemData *>				m_AssetList;

		BankData(QString sAbsDataDirPath, QJsonObject settingsObj) :
			m_Dir(sAbsDataDirPath),
			m_Settings(settingsObj)
		{ }
	};
	class BanksModel : public QAbstractListModel
	{
		IManagerModel &						m_ModelRef;
		QList<BankData *>					m_BankList;

	public:
		BanksModel(IManagerModel &modelRef) :
			m_ModelRef(modelRef)
		{
		}

		void CreateBank(QString sAbsPath, QJsonObject settingsObj)
		{
			BankData *pNewBank = new BankData(sAbsPath, settingsObj);
			m_BankList.push_back(pNewBank);
		}
		
		BankData *GetBank(uint uiIndex)
		{
			return m_BankList[uiIndex];
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

	QDir										m_MetaDir;
	QDir										m_DataDir;

	quint32										m_uiNextBankId;
	QJsonArray									m_ExpandedFiltersArray;

	class AssetLookup
	{
		QMap<QUuid, AssetItemData *>				m_AssetUuidMap;
		QMap<quint32, QList<AssetItemData *> >		m_AssetChecksumMap;

	public:
		void AddLookup(AssetItemData *pAsset) {
			m_AssetUuidMap[pAsset->GetUuid()] = pAsset;

			uint32 uiChecksum = pAsset->GetChecksum();

			if(m_AssetChecksumMap.contains(uiChecksum))
			{
				m_AssetChecksumMap.find(uiChecksum).value().append(pAsset);
				HyGuiLog("'" % pAsset->GetName() % "' is a duplicate of '" % m_AssetChecksumMap.find(uiChecksum).value()[0]->GetName() % "' with the checksum: " % QString::number(uiChecksum) % " totaling: " % QString::number(m_AssetChecksumMap.find(uiChecksum).value().size()), LOGTYPE_Info);
			}
			else
			{
				QList<AssetItemData *> newFrameList;
				newFrameList.append(pAsset);
				m_AssetChecksumMap[uiChecksum] = newFrameList;
			}
		}
		bool RemoveLookup(AssetItemData *pAsset) {  // Returns true if no remaining duplicates exist
			m_AssetUuidMap.remove(pAsset->GetUuid());

			auto iter = m_AssetChecksumMap.find(pAsset->GetChecksum());
			if(iter == m_AssetChecksumMap.end())
				HyGuiLog("AtlasModel::RemoveLookup could not find frame", LOGTYPE_Error);

			iter.value().removeOne(pAsset);
			if(iter.value().size() == 0)
			{
				m_AssetChecksumMap.remove(pAsset->GetChecksum());
				return true;
			}

			return false;
		}
		AssetItemData *FindById(QUuid uuid) {
			auto iter = m_AssetUuidMap.find(uuid);
			if(iter == m_AssetUuidMap.end())
				return nullptr;
			else
				return iter.value();
		}
		QList<AssetItemData *> FindByChecksum(quint32 uiChecksum) {
			auto iter = m_AssetChecksumMap.find(uiChecksum);
			if(iter == m_AssetChecksumMap.end())
				return QList<AssetItemData *>();
			else
				return iter.value();
		}
		bool DoesAssetExist(quint32 uiChecksum) {
			return m_AssetChecksumMap.contains(uiChecksum);
		}
	};
	AssetLookup									m_AssetLookup;

public:
	IManagerModel(Project &projRef, HyGuiItemType eItemType);
	virtual ~IManagerModel();

	Project &GetProjOwner();

	int GetNumBanks();
	QString GetBankName(uint uiBankIndex);
	QJsonObject GetBankSettings(uint uiBankIndex);
	void SetBankSettings(uint uiBankIndex, QJsonObject newSettingsObj);

	QList<AssetItemData *> GetAssets(uint uiBankIndex);

	QJsonArray GetExpandedFiltersArray();

	QString AssembleFilter(const AssetItemData *pAsset) const;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//AssetItemData *CreateAsset(QUuid uuid, quint32 uiCRC, int32 uiBankId, QString sName);//, QRect rAlphaCrop, AtlasItemType eFrameType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors);
	void RemoveAsset(AssetItemData *pAsset);
	bool TransferAsset(AssetItemData *pAsset, uint uiBankIndex);
	AssetItemData *GenerateAsset(ProjectItemData *pItem, QString sName, QImage &newImage, quint32 uiBankIndex, HyGuiItemType eType);
	bool ReplaceAsset(AssetItemData *pAsset, QString sName, QImage &newImage, bool bDoBankRepack);

	QList<AssetItemData *> RequestAssets(ProjectItemData *pItem);
	QList<AssetItemData *> RequestAssets(ProjectItemData *pItem, QList<AssetItemData *> requestList);
	QList<AssetItemData *> RequestAssetsByUuid(ProjectItemData *pItem, QList<QUuid> requestList);

	void RelinquishFrames(ProjectItemData *pItem, QList<AtlasFrame *> relinquishList);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	QSet<AssetItemData *> ImportAssets(QStringList sImportList, uint uiBankIndex, HyGuiItemType eType, QList<AssetItemData *> correspondingParentList);

	AssetItemData *CreateFilter(QString sName, AssetItemData *pParent);

	bool DoesAssetExist(quint32 uiChecksum);

	uint CreateNewBank(QString sName);
	void RemoveBank(uint uiBankIndex);

	uint GetBankIndexFromBankUuid(QUuid uiAtlasGrpId);
	QUuid GetBankUuidFromBankIndex(uint uiBankIndex);

	void WriteMeta();
	void WriteData();

protected:
	virtual AssetItemData *OnAllocateAssetData(QJsonObject metaObj) = 0;

private:
	TreeModelItemData *AddTreeItem(bool bIsFilter, const QString sPrefix, const QString sName, QJsonObject metaObj);
	bool InsertTreeItem(TreeModelItemData *pNewItem, TreeModelItem *pParentTreeItem, int iRow = -1);
};

#endif // IMANAGERMODEL_H
