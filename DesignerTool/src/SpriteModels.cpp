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
#include "SpriteItem.h"

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

QJsonArray SpriteFramesModel::GetFrames(float &fTotalDurationRef)
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

SpriteStatesModel::SpriteStatesModel(SpriteItem *pSpriteItem) : QAbstractListModel(pSpriteItem),
                                                                m_pSpriteItem(pSpriteItem)
{
}

int SpriteStatesModel::AppendState(QJsonObject stateObj)
{
    int iIndex = m_StateList.size();
    InsertState(iIndex, stateObj);

    return iIndex;
}

void SpriteStatesModel::InsertState(int iStateIndex, QJsonObject stateObj)
{
    AnimState *pNewState = new AnimState();

    if(stateObj.empty() == false)
    {
        pNewState->sName = stateObj["name"].toString();
        pNewState->bLoop = stateObj["loop"].toBool();
        pNewState->bReverse = stateObj["reverse"].toBool();
        pNewState->bBounce = stateObj["bounce"].toBool();

        QJsonArray spriteFrameArray = stateObj["frames"].toArray();
        QList<quint32> frameRequestList;
        for(int i = 0; i < spriteFrameArray.size(); ++i)
        {
            QJsonObject spriteFrameObj = spriteFrameArray[i].toObject();
            frameRequestList.append(JSONOBJ_TOINT(spriteFrameObj, "checksum"));
        }

        QList<AtlasFrame *> requestedAtlasFramesList;
        requestedAtlasFramesList = m_pSpriteItem->GetProject()->GetAtlasesData().RequestFrames(m_pSpriteItem, requestList);

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
        pNewState->bLoop = false;
        pNewState->bReverse = false;
        pNewState->bBounce = false;
        pNewState->pFramesModel = new SpriteFramesModel(this);
    }

    beginInsertRows(QModelIndex(), iStateIndex, iStateIndex);
    m_StateList.insert(iStateIndex, pNewState);
    endInsertRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(0, 0), createIndex(m_StateList.size() - 1, 0), roleList);
}

QJsonObject SpriteStatesModel::PopStateAt(uint32 uiIndex)
{
    QJsonObject retStateObj;
    retStateObj.insert("name", m_StateList[uiIndex]->sName);
    retStateObj.insert("loop", m_StateList[uiIndex]->bLoop);
    retStateObj.insert("reverse", m_StateList[uiIndex]->bReverse);
    retStateObj.insert("bounce", m_StateList[uiIndex]->bBounce);

    float fTotalDuration = 0.0f;
    QJsonArray framesArray = m_StateList[uiIndex]->pFramesModel->GetFrames(fTotalDuration);
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

QString SpriteStatesModel::SetStateName(int iStateIndex, QString sNewName)
{
    QString sOldName = m_StateList[iStateIndex]->sName;
    m_StateList[iStateIndex]->sName = sNewName;

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);

    return sOldName;
}

void SpriteStatesModel::MoveStateBack(int iStateIndex)
{
    if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex - 1) == false)
        return;

    m_StateList.swap(iStateIndex, iStateIndex - 1);
    endMoveRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

void SpriteStatesModel::MoveStateForward(int iStateIndex)
{
    if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex + 2) == false)    // + 2 is here because Qt is retarded
        return;

    m_StateList.swap(iStateIndex, iStateIndex + 1);
    endMoveRows();

    QVector<int> roleList;
    roleList.append(Qt::DisplayRole);
    dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

/*virtual*/ int SpriteStatesModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
}

/*virtual*/ QVariant SpriteStatesModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    SpriteFrame *pFrame = m_FramesList[index.row()];

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;

    if(role == Qt::DisplayRole || role == Qt::EditRole)
        return QString::number(index.row()) % " - " % m_StateList[index.row()]->sName;

    return QVariant();
}

/*virtual*/ QVariant SpriteStatesModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
}
