/**************************************************************************
 *	BanksModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef BANKSMODEL_H
#define BANKSMODEL_H

#include "Global.h"
#include "ITreeModel.h"
#include "IAssetItemData.h"

#include <QUuid>

struct BankData
{
	QString									m_sAbsPath;
	QJsonObject								m_MetaObj;
	QList<IAssetItemData *>					m_AssetList;

	BankData(QString sAbsDataDirPath, QJsonObject settingsObj) :
		m_sAbsPath(sAbsDataDirPath),
		m_MetaObj(settingsObj)
	{ }

	~BankData()
	{
		for(int i = 0; i < m_AssetList.size(); ++i)
			delete m_AssetList[i];
	}

	quint32 GetId() const {
		if(m_MetaObj.contains("bankId") == false) {
			HyGuiLog("BankData::GetId could not find 'bankId' in bank's settings", LOGTYPE_Error);
		}
		return m_MetaObj["bankId"].toInt();
	}

	QString GetName() const {
		return m_MetaObj["bankName"].toString();
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

#endif // BANKSMODEL_H
