/**************************************************************************
 *	SpriteModels.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "SpriteModels.h"
#include "Project.h"

#include <QJsonArray>

SpriteFramesModel::SpriteFramesModel(QObject *parent) : QAbstractTableModel(parent)
{
}

int SpriteFramesModel::Add(AtlasFrame *pFrame)
{
    SpriteFrame *pFrameToInsert = NULL;

    // See if this frame has been recently removed, and re-add if possible. Otherwise, create a new Frame
    QMap<quint32, SpriteFrame *>::iterator iter = m_RemovedFrameMap.find(pFrame->GetChecksum());
    if(iter == m_RemovedFrameMap.end())
        pFrameToInsert = new SpriteFrame(pFrame, m_FramesList.count());
    else
    {
        pFrameToInsert = iter.value();
        m_RemovedFrameMap.remove(pFrame->GetChecksum());
    }

    beginInsertRows(QModelIndex(), pFrameToInsert->m_iRowIndex, pFrameToInsert->m_iRowIndex);
    m_FramesList.insert(pFrameToInsert->m_iRowIndex, pFrameToInsert);
    endInsertRows();

    return pFrameToInsert->m_iRowIndex;
}

void SpriteFramesModel::Remove(AtlasFrame *pFrame)
{
    for(int i = 0; i < m_FramesList.count(); ++i)
    {
        // NOTE: Don't delete this frame as it may be 'undone'
        if(m_FramesList[i]->m_pFrame == pFrame)
        {
            m_RemovedFrameMap[pFrame->GetChecksum()] = m_FramesList[i];

            beginRemoveRows(QModelIndex(), i, i);
            m_FramesList.removeAt(i);
            endRemoveRows();
            break;
        }
    }
}

void SpriteFramesModel::MoveRowUp(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
        return;

    m_FramesList.swap(iIndex, iIndex - 1);
    endMoveRows();
}

void SpriteFramesModel::MoveRowDown(int iIndex)
{
    if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is retarded
        return;

    m_FramesList.swap(iIndex, iIndex + 1);
    endMoveRows();
}

void SpriteFramesModel::RefreshFrame(AtlasFrame *pFrame)
{
    bool bFound = false;
    for(int i = 0; i < m_FramesList.count(); ++i)
    {
        if(m_FramesList[i]->m_pFrame == pFrame)
        {
            bFound = true;
            break;
        }
    }

    if(bFound)
        dataChanged(createIndex(0, 0), createIndex(m_FramesList.count() - 1, NUMCOLUMNS - 1));
}

// iIndex of -1 will apply to all
void SpriteFramesModel::OffsetFrame(int iIndex, QPoint vOffset)
{
    if(iIndex == -1)
    {
        for(int i = 0; i < m_FramesList.count(); ++i)
            m_FramesList[i]->m_vOffset = vOffset;

        dataChanged(createIndex(0, COLUMN_OffsetX), createIndex(m_FramesList.count() - 1, COLUMN_OffsetY));
    }
    else
    {
        m_FramesList[iIndex]->m_vOffset = vOffset;
        dataChanged(createIndex(iIndex, COLUMN_OffsetX), createIndex(iIndex, COLUMN_OffsetY));
    }
}

// iIndex of -1 will apply to all
void SpriteFramesModel::DurationFrame(int iIndex, float fDuration)
{
    if(iIndex == -1)
    {
        for(int i = 0; i < m_FramesList.count(); ++i)
            m_FramesList[i]->m_fDuration = fDuration;

        dataChanged(createIndex(0, COLUMN_Duration), createIndex(m_FramesList.count() - 1, COLUMN_Duration));
    }
    else
    {
        m_FramesList[iIndex]->m_fDuration = fDuration;
        dataChanged(createIndex(iIndex, COLUMN_Duration), createIndex(iIndex, COLUMN_Duration));
    }
}

QJsonArray SpriteFramesModel::GetFramesInfo(float &fTotalDurationRef)
{
    QJsonArray framesArray;
    fTotalDurationRef = 0.0f;

    for(int i = 0; i < m_FramesList.count(); ++i)
    {
        QJsonObject frameObj;
        fTotalDurationRef += m_FramesList[i]->m_fDuration;

        frameObj.insert("atlasIndex", m_FramesList[i]->m_pFrame->GetTextureIndex());
        frameObj.insert("checksum", QJsonValue(static_cast<qint64>(m_FramesList[i]->m_pFrame->GetChecksum())));
        frameObj.insert("duration", m_FramesList[i]->m_fDuration);
        frameObj.insert("offsetX", m_FramesList[i]->m_vOffset.x());
        frameObj.insert("offsetY", m_FramesList[i]->m_vOffset.y());

        framesArray.append(frameObj);
    }

    return framesArray;
}

SpriteFrame *SpriteFramesModel::GetFrameAt(int iIndex)
{
    if(iIndex < 0)
        return NULL;

    return m_FramesList[iIndex];
}

/*virtual*/ int SpriteFramesModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_FramesList.count();
}

/*virtual*/ int SpriteFramesModel::columnCount(const QModelIndex & /*parent*/) const
{
    return NUMCOLUMNS;
}

/*virtual*/ QVariant SpriteFramesModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    SpriteFrame *pFrame = m_FramesList[index.row()];

    if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Frame)
    {
        return Qt::AlignCenter;
    }

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_Frame:
            return pFrame->m_pFrame->GetName();
        case COLUMN_OffsetX:
            return QString::number(pFrame->m_vOffset.x());
        case COLUMN_OffsetY:
            return QString::number(pFrame->m_vOffset.y());
        case COLUMN_Duration:
            return QString::number(pFrame->m_fDuration, 'g', 3) % ((role == Qt::DisplayRole) ? "sec" : "");
        }
    }

    return QVariant();
}

/*virtual*/ QVariant SpriteFramesModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(iIndex)
            {
            case COLUMN_Frame:
                return QString("Frame");
            case COLUMN_OffsetX:
                return QString("X Offset");
            case COLUMN_OffsetY:
                return QString("Y Offset");
            case COLUMN_Duration:
                return QString("Duration");
            }
        }
        else
            return QString::number(iIndex);
    }

    return QVariant();
}

/*virtual*/ bool SpriteFramesModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    HyGuiLog("SpriteFramesModel::setData was invoked", LOGTYPE_Error);

    SpriteFrame *pFrame = m_FramesList[index.row()];

    if(role == Qt::EditRole)
    {
        switch(index.column())
        {
        case COLUMN_OffsetX:
            pFrame->m_vOffset.setX(value.toInt());
            break;
        case COLUMN_OffsetY:
            pFrame->m_vOffset.setY(value.toInt());
            break;
        case COLUMN_Duration:
            pFrame->m_fDuration = value.toFloat();
            break;
        }
    }

    QVector<int> vRolesChanged;
    vRolesChanged.append(role);
    dataChanged(index, index, vRolesChanged);

    return true;
}

/*virtual*/ Qt::ItemFlags SpriteFramesModel::flags(const QModelIndex &index) const
{
    if(index.column() == COLUMN_Frame)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteModel::SpriteModel(ProjectItem *pItem, QJsonArray stateArray) :   QAbstractListModel(pItem),
                                                                        m_pItem(pItem)
{
    // If item's init value is defined, parse and initalize with it, otherwise make default empty sprite
    if(stateArray.empty() == false)
    {
        for(int i = 0; i < stateArray.size(); ++i)
            AppendState(stateArray[i].toObject());
    }
    else
        AppendState(QJsonObject());
}

/*virtual*/ SpriteModel::~SpriteModel()
{
}

int SpriteModel::GetNumStates()
{
    return m_StateList.size();
}

SpriteStateData *SpriteModel::GetStateData(int iStateIndex)
{
    return m_StateList[iStateIndex];
}

QList<AtlasFrame *> SpriteModel::RequestFrames(int iStateIndex, QList<AtlasFrame *> requestList)
{
    QList<AtlasFrame *> returnedAtlasFramesList = m_pItem->GetProject().GetAtlasesData().RequestFrames(m_pItem, requestList);

    for(int i = 0; i < returnedAtlasFramesList.size(); ++i)
        m_StateList[iStateIndex]->pFramesModel->Add(returnedAtlasFramesList[i]);

    return returnedAtlasFramesList;
}

void SpriteModel::RelinquishFrames(int iStateIndex, QList<AtlasFrame *> relinquishList)
{
    for(int i = 0; i < relinquishList.size(); ++i)
        m_StateList[iStateIndex]->pFramesModel->Remove(relinquishList[i]);

    m_pItem->GetProject().GetAtlasesData().RelinquishFrames(m_pItem, relinquishList);
}

void SpriteModel::RefreshFrame(AtlasFrame *pFrame)
{
    for(int i = 0; i < m_StateList.size(); ++i)
        m_StateList[i]->pFramesModel->RefreshFrame(pFrame);
}

int SpriteModel::AppendState(QJsonObject stateObj)
{
    int iIndex = m_StateList.size();
    InsertState(iIndex, stateObj);

    return iIndex;
}

void SpriteModel::InsertState(int iStateIndex, QJsonObject stateObj)
{
    SpriteStateData *pNewState = new SpriteStateData();

    if(stateObj.empty() == false)
    {
        pNewState->sName = stateObj["name"].toString();

        pNewState->pLoopMapper = new CheckBoxMapper(this);
        pNewState->pLoopMapper->SetChecked(stateObj["loop"].toBool());

        pNewState->pReverseMapper = new CheckBoxMapper(this);
        pNewState->pReverseMapper->SetChecked(stateObj["reverse"].toBool());

        pNewState->pBounceMapper = new CheckBoxMapper(this);
        pNewState->pBounceMapper->SetChecked(stateObj["bounce"].toBool());

        QJsonArray spriteFrameArray = stateObj["frames"].toArray();
        QList<quint32> checksumRequestList;
        for(int i = 0; i < spriteFrameArray.size(); ++i)
        {
            QJsonObject spriteFrameObj = spriteFrameArray[i].toObject();
            checksumRequestList.append(JSONOBJ_TOINT(spriteFrameObj, "checksum"));
        }

        QList<AtlasFrame *> requestedAtlasFramesList;
        requestedAtlasFramesList = m_pItem->GetProject().GetAtlasesData().RequestFrames(m_pItem, checksumRequestList);

        if(spriteFrameArray.size() != requestedAtlasFramesList.size())
        {
            HyGuiLog("SpriteStatesModel::AppendState() failed to acquire all the stored frames", LOGTYPE_Error);
        }

        pNewState->pFramesModel = new SpriteFramesModel(this);
        for(int i = 0; i < requestedAtlasFramesList.size(); ++i)
        {
            QJsonObject spriteFrameObj = spriteFrameArray[i].toObject();
            QPoint vOffset(spriteFrameObj["offsetX"].toInt() - requestedAtlasFramesList[0]->GetCrop().left(),
                           spriteFrameObj["offsetY"].toInt() - (requestedAtlasFramesList[0]->GetSize().height() - requestedAtlasFramesList[0]->GetCrop().bottom()));

            int iFrameIndex = pNewState->pFramesModel->Add(requestedAtlasFramesList[i]);
            pNewState->pFramesModel->OffsetFrame(iFrameIndex, vOffset);
            pNewState->pFramesModel->DurationFrame(iFrameIndex, spriteFrameObj["duration"].toDouble());
        }
    }
    else
    {
        pNewState->sName = "Unnamed";

        pNewState->pLoopMapper = new CheckBoxMapper(this);
        pNewState->pLoopMapper->SetChecked(false);

        pNewState->pReverseMapper = new CheckBoxMapper(this);
        pNewState->pReverseMapper->SetChecked(false);

        pNewState->pBounceMapper = new CheckBoxMapper(this);
        pNewState->pBounceMapper->SetChecked(false);

        pNewState->pFramesModel = new SpriteFramesModel(this);
    }

    beginInsertRows(QModelIndex(), iStateIndex, iStateIndex);
    m_StateList.insert(iStateIndex, pNewState);
    endInsertRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(0, 0), createIndex(m_StateList.size() - 1, 0), roleList);
}

QJsonObject SpriteModel::PopStateAt(uint32 uiIndex)
{
    QJsonObject retStateObj;
    retStateObj.insert("name", m_StateList[uiIndex]->sName);
    retStateObj.insert("loop", m_StateList[uiIndex]->pLoopMapper->currentIndex());
    retStateObj.insert("reverse", m_StateList[uiIndex]->pReverseMapper->currentIndex());
    retStateObj.insert("bounce", m_StateList[uiIndex]->pBounceMapper->currentIndex());

    float fTotalDuration = 0.0f;
    QJsonArray framesArray = m_StateList[uiIndex]->pFramesModel->GetFramesInfo(fTotalDuration);
    retStateObj.insert("frames", framesArray);
    retStateObj.insert("duration", fTotalDuration);

    beginRemoveRows(QModelIndex(), uiIndex, uiIndex);
    m_StateList.removeAt(uiIndex);
    endRemoveRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(0, 0), createIndex(m_StateList.size() - 1, 0), roleList);

    return retStateObj;
}

QString SpriteModel::SetStateName(int iStateIndex, QString sNewName)
{
    QString sOldName = m_StateList[iStateIndex]->sName;
    m_StateList[iStateIndex]->sName = sNewName;

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);

    return sOldName;
}

void SpriteModel::MoveStateBack(int iStateIndex)
{
    if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex - 1) == false)
        return;

    m_StateList.swap(iStateIndex, iStateIndex - 1);
    endMoveRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

void SpriteModel::MoveStateForward(int iStateIndex)
{
    if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex + 2) == false)    // + 2 is here because Qt is retarded
        return;

    m_StateList.swap(iStateIndex, iStateIndex + 1);
    endMoveRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

QJsonArray SpriteModel::GetSaveInfo()
{
    QJsonArray retArray;
    for(int i = 0; i < m_StateList.size(); ++i)
    {
        QJsonObject spriteState;
        m_StateList[i]->GetStateInfo(spriteState);
        retArray.append(spriteState);
    }

    return retArray;
}

/*virtual*/ int SpriteModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    return m_StateList.size();
}

/*virtual*/ QVariant SpriteModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;

    if(role == Qt::DisplayRole || role == Qt::EditRole)
        return QString::number(index.row()) % " - " % m_StateList[index.row()]->sName;

    return QVariant();
}

/*virtual*/ bool SpriteModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
    return QAbstractItemModel::setData(index, value, role);
}

/*virtual*/ QVariant SpriteModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    return (iIndex == 0) ? QVariant(QString("State Names")) : QVariant();
}
