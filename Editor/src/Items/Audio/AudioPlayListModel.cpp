/**************************************************************************
 *	AudioPlayListModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AudioPlayListModel.h"

#include <QJsonArray>

AudioPlayListModel::AudioPlayListModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

/*virtual*/ AudioPlayListModel::~AudioPlayListModel()
{
}

// Returns the index the audio asset was inserted to
int AudioPlayListModel::Add(SoundClip *pAsset)
{
	AudioPlayListItem *pFrameToInsert = nullptr;

	// See if this frame has been recently removed, and re-add if possible. Otherwise, create a new Frame
	QMap<QUuid, QPair<int, AudioPlayListItem *>>::iterator iter = m_RemovedAssetIdMap.find(pAsset->GetUuid());
	int iInsertIndex;
	if(iter == m_RemovedAssetIdMap.end())
	{
		iInsertIndex = m_PlayList.count();
		pFrameToInsert = new AudioPlayListItem(pAsset);
	}
	else
	{
		iInsertIndex = iter.value().first;
		pFrameToInsert = iter.value().second;

		m_RemovedAssetIdMap.remove(pAsset->GetUuid());
	}

	beginInsertRows(QModelIndex(), iInsertIndex, iInsertIndex);
	m_PlayList.insert(iInsertIndex, pFrameToInsert);
	endInsertRows();

	return iInsertIndex;
}

void AudioPlayListModel::Remove(SoundClip *pAsset)
{
	for(int i = 0; i < m_PlayList.count(); ++i)
	{
		// NOTE: Don't delete this AudioPlayListItem as the remove may be 'undone'
		if(m_PlayList[i]->GetAudioAsset() == pAsset)
		{
			m_RemovedAssetIdMap[pAsset->GetUuid()] = QPair<int, AudioPlayListItem *>(i, m_PlayList[i]);

			beginRemoveRows(QModelIndex(), i, i);
			m_PlayList.removeAt(i);
			endRemoveRows();
			break;
		}
	}
}

void AudioPlayListModel::MoveRowUp(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
		return;

	m_PlayList.swapItemsAt(iIndex, iIndex - 1);
	endMoveRows();
}

void AudioPlayListModel::MoveRowDown(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is stupid
		return;

	m_PlayList.swapItemsAt(iIndex, iIndex + 1);
	endMoveRows();
}

// iIndex of -1 will apply to all
void AudioPlayListModel::SetWeight(int iIndex, uint uiWeight)
{
	if(iIndex == -1)
	{
		for(int i = 0; i < m_PlayList.count(); ++i)
			m_PlayList[i]->SetWeight(uiWeight);

		dataChanged(createIndex(0, COLUMN_Weight), createIndex(m_PlayList.count() - 1, COLUMN_Percent));
	}
	else
	{
		m_PlayList[iIndex]->SetWeight(uiWeight);
		dataChanged(createIndex(iIndex, COLUMN_Weight), createIndex(iIndex, COLUMN_Percent));
	}
}

QList<TreeModelItemData *> AudioPlayListModel::GetSoundClips() const
{
	QList<TreeModelItemData *> soundClipList;
	for(int i = 0; i < m_PlayList.count(); ++i)
		soundClipList.append(m_PlayList[i]->GetAudioAsset());
	return soundClipList;
}

QJsonArray AudioPlayListModel::GenPlayListArray() const
{
	QJsonArray playListArray;

	for(int i = 0; i < m_PlayList.count(); ++i)
	{
		QJsonObject frameObj;
		frameObj.insert("checksum", QJsonValue(static_cast<qint64>(m_PlayList[i]->GetAudioAsset()->GetChecksum())));
		frameObj.insert("bankId", QJsonValue(static_cast<qint64>(m_PlayList[i]->GetAudioAsset()->GetBankId())));
		frameObj.insert("weight", static_cast<int>(m_PlayList[i]->GetWeight()));
		playListArray.append(frameObj);
	}

	return playListArray;
}

AudioPlayListItem *AudioPlayListModel::GetPlayListItemAt(int iIndex)
{
	if(iIndex < 0)
		return nullptr;

	return m_PlayList[iIndex];
}

/*virtual*/ int AudioPlayListModel::rowCount(const QModelIndex & /*parent*/) const
{
	return m_PlayList.count();
}

/*virtual*/ int AudioPlayListModel::columnCount(const QModelIndex & /*parent*/) const
{
	return NUMCOLUMNS;
}

/*virtual*/ QVariant AudioPlayListModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	AudioPlayListItem *pPlayListItem = m_PlayList[index.row()];

	if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Audio)
	{
		return Qt::AlignCenter;
	}

	if(role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch(index.column())
		{
		case COLUMN_Audio:
			return pPlayListItem->GetAudioAsset()->GetName();
		case COLUMN_Weight:
			return QString::number(pPlayListItem->GetWeight());
		case COLUMN_Percent: {
			int dTotal = 0.0;
			for(auto item : m_PlayList)
				dTotal += static_cast<double>(item->GetWeight());
			
			return QVariant(QString::number(100.0 * (static_cast<double>(pPlayListItem->GetWeight()) / dTotal), 'g', 3) % "%");
		}
		}
	}

	return QVariant();
}

/*virtual*/ QVariant AudioPlayListModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			switch(iIndex)
			{
			case COLUMN_Audio:
				return QString("Frame");
			case COLUMN_Weight:
				return QString("Weight");
			case COLUMN_Percent:
				return QString("Percent");
			}
		}
		else
			return QString::number(iIndex);
	}

	return QVariant();
}

/*virtual*/ bool AudioPlayListModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	AudioPlayListItem *pPlayListItem = m_PlayList[index.row()];

	if(role == Qt::EditRole)
	{
		switch(index.column())
		{
		case COLUMN_Weight:
			pPlayListItem->SetWeight(value.toInt());
			break;
		}
	}

	QVector<int> vRolesChanged;
	vRolesChanged.append(role);
	dataChanged(index, index, vRolesChanged);

	return true;
}

/*virtual*/ Qt::ItemFlags AudioPlayListModel::flags(const QModelIndex &index) const
{
	if(index.column() == COLUMN_Weight)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
	else
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
