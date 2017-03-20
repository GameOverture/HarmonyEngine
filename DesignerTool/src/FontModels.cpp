/**************************************************************************
 *	FontModels.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "FontModels.h"

FontModel::FontModel(ProjectItem *pItem, QJsonObject stateArray) : QAbstractListModel(pItem)
{

}

/*virtual*/ FontModel::~FontModel()
{
}

/*virtual*/ int FontModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return 1;//m_StateList.size();
}

/*virtual*/ QVariant FontModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
//    if (role == Qt::TextAlignmentRole)
//        return Qt::AlignLeft;

//    if(role == Qt::DisplayRole || role == Qt::EditRole)
//        return QString::number(index.row()) % " - " % m_StateList[index.row()]->sName;

    return QVariant();
}

/*virtual*/ bool FontModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    return QAbstractItemModel::setData(index, value, role);
}

/*virtual*/ QVariant FontModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    return (iIndex == 0) ? QVariant(QString("State Names")) : QVariant();
}
