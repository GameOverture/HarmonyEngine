/**************************************************************************
 *	AssetBanksModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ASSETBANKSMODEL_H
#define ASSETBANKSMODEL_H

#include "Global.h"
#include "ITreeModel.h"
#include "IAssetItemData.h"

#include <QUuid>

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
	Q_OBJECT

	QList<BankData *>						m_BankList;

public:
	BanksModel();
	virtual ~BanksModel();

	BankData *GetBank(uint uiIndex);
	const BankData *GetBank(uint uiIndex) const;

	BankData *AppendBank(QString sAbsPath, QJsonObject settingsObj);
	void RemoveBank(uint uiIndex);

	int GetIndex(BankData *pData) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

#endif // ASSETBANKSMODEL_H
