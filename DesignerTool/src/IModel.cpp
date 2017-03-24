/**************************************************************************
 *	IModel.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "IModel.h"
#include "AtlasModel.h"
#include "Project.h"

IStateData::IStateData(IModel &modelRef, QString sName) :   m_ModelRef(modelRef),
                                                            m_sName(sName)
{ }

/*virtual*/ IStateData::~IStateData()
{ }

QString IStateData::GetName()
{
    return m_sName;
}

void IStateData::SetName(QString sNewName)
{
    m_sName = sNewName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IModel::IModel(ProjectItem *pItem) :    QAbstractListModel(pItem),
                                        m_pItem(pItem)
{
}

/*virtual*/ IModel::~IModel()
{
}

int IModel::GetNumStates()
{
    return m_StateList.size();
}

IStateData *IModel::GetStateData(int iStateIndex)
{
    return m_StateList[iStateIndex];
}

QString IModel::SetStateName(int iStateIndex, QString sNewName)
{
    QString sOldName = m_StateList[iStateIndex]->GetName();
    m_StateList[iStateIndex]->SetName(sNewName);

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);

    return sOldName;
}

void IModel::MoveStateBack(int iStateIndex)
{
    if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex - 1) == false)
        return;

    m_StateList.swap(iStateIndex, iStateIndex - 1);
    endMoveRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

void IModel::MoveStateForward(int iStateIndex)
{
    if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex + 2) == false)    // + 2 is here because Qt is retarded
        return;

    m_StateList.swap(iStateIndex, iStateIndex + 1);
    endMoveRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

QList<AtlasFrame *> IModel::RequestFrames(IStateData *pState, QList<quint32> requestList)
{
    QList<AtlasFrame *> returnedAtlasFramesList = m_pItem->GetProject().GetAtlasModel().RequestFrames(m_pItem, requestList);
    
    if(pState)
    {
        for(int i = 0; i < returnedAtlasFramesList.size(); ++i)
            pState->AddFrame(returnedAtlasFramesList[i]);
    }
    
    return returnedAtlasFramesList;
}

QList<AtlasFrame *> IModel::RequestFrames(int iStateIndex, QList<AtlasFrame *> requestList)
{
    QList<AtlasFrame *> returnedAtlasFramesList = m_pItem->GetProject().GetAtlasModel().RequestFrames(m_pItem, requestList);

    if(iStateIndex >= 0)
    {
        for(int i = 0; i < returnedAtlasFramesList.size(); ++i)
            m_StateList[iStateIndex]->AddFrame(returnedAtlasFramesList[i]);
    }

    return returnedAtlasFramesList;
}

void IModel::RelinquishFrames(int iStateIndex, QList<AtlasFrame *> relinquishList)
{
    if(iStateIndex >= 0)
    {
        for(int i = 0; i < relinquishList.size(); ++i)
            m_StateList[iStateIndex]->RelinquishFrame(relinquishList[i]);
    }
    
    m_pItem->GetProject().GetAtlasModel().RelinquishFrames(m_pItem, relinquishList);
}

void IModel::RefreshFrame(AtlasFrame *pFrame)
{
    for(int i = 0; i < m_StateList.size(); ++i)
        m_StateList[i]->RefreshFrame(pFrame);
}

/*virtual*/ int IModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_StateList.size();
}

/*virtual*/ QVariant IModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;

    if(role == Qt::DisplayRole || role == Qt::EditRole)
        return QString::number(index.row()) % " - " % m_StateList[index.row()]->GetName();

    return QVariant();
}

/*virtual*/ bool IModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    return QAbstractItemModel::setData(index, value, role);
}

/*virtual*/ QVariant IModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    return (iIndex == 0) ? QVariant(QString("State Names")) : QVariant();
}
