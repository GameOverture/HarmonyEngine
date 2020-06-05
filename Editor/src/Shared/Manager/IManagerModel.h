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
	const QUuid							m_UNIQUE_ID;

	QSet<ProjectItemData *>				m_DependencySet;
};

class IManagerModel : public ITreeModel
{
	Q_OBJECT

	class BanksModel : public QAbstractListModel
	{
		IManagerModel &m_ModelRef;

		struct BankData
		{
			QDir								m_Dir;
			QJsonObject							m_Settings;
		};
		QList<BankData *>						m_BankList;

	public:
		BanksModel(IManagerModel &modelRef) :
			m_ModelRef(modelRef)
		{
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

	QJsonArray									m_ExpandedFiltersArray;

	class AssetLookup
	{
		QMap<QUuid, AssetItemData *>		m_FrameIdMap;
		QMap<quint32, QList<AssetItemData *> >		m_FrameChecksumMap;

	public:
		void AddLookup(AssetItemData *pAsset);
		bool RemoveLookup(AssetItemData *pAsset);  // Returns true if no remaining duplicates exist
		AssetItemData *FindById(QUuid uuid);
		QList<AssetItemData *> FindByChecksum(quint32 uiChecksum);
		bool DoesImageExist(quint32 uiChecksum);
	};
	AssetLookup									m_AssetLookup;

public:
	IManagerModel(Project &projRef);
	virtual ~IManagerModel();

	Project *GetProjOwner(); 

	int GetNumBanks();
	QString GetBankName(uint uiIndex);
	QJsonObject GetBankSettings(uint uiIndex);
	void SetBankSettings(uint uiIndex, QJsonObject newSettingsObj);

	QJsonArray GetExpandedFiltersArray();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	AssetItemData *CreateAsset(QUuid uuid, quint32 uiCRC, int32 uiBankId, QString sName);//, QRect rAlphaCrop, AtlasItemType eFrameType, int iW, int iH, int iX, int iY, int iTextureIndex, uint uiErrors);
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
};

#endif // IMANAGERMODEL_H
