/**************************************************************************
 *	AssetBanksModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AssetBanksModel.h"

BanksModel::BanksModel()
{
}

/*virtual*/ BanksModel::~BanksModel()
{
	for(int i = 0; i < m_BankList.size(); ++i)
		delete m_BankList[i];
}

BankData *BanksModel::GetBank(uint uiIndex)
{
	return m_BankList[uiIndex];
}

const BankData *BanksModel::GetBank(uint uiIndex) const
{
	return m_BankList[uiIndex];
}

BankData *BanksModel::AppendBank(QString sAbsPath, QJsonObject settingsObj)
{
	beginInsertRows(QModelIndex(), m_BankList.count(), m_BankList.count());
	BankData *pNewBank = new BankData(sAbsPath, settingsObj);
	m_BankList.push_back(pNewBank);
	endInsertRows();

	return pNewBank;
}

void BanksModel::RemoveBank(uint uiIndex)
{
	BankData *pBankToBeRemoved = m_BankList[uiIndex];

	beginRemoveRows(QModelIndex(), uiIndex, uiIndex);
	m_BankList.removeAt(uiIndex);
	endRemoveRows();

	delete pBankToBeRemoved;
}

int BanksModel::GetIndex(BankData *pData) const
{
	for(int i = 0; i < m_BankList.size(); ++i)
	{
		if(pData == m_BankList[i])
			return i;
	}
	return -1;
}

/*virtual*/ int BanksModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return m_BankList.size();
}

/*virtual*/ QVariant BanksModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignLeft;

	if(role == Qt::DisplayRole || role == Qt::EditRole)
		return "Bank: " % QString::number(GetIndex(m_BankList[index.row()])) % " - " % m_BankList[index.row()]->m_MetaObj["txtName"].toString();

	return QVariant();
}

/*virtual*/ QVariant BanksModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	return QVariant();
}
