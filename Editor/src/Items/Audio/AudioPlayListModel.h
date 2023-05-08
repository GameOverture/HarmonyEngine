/**************************************************************************
 *	AudioPlayListModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOPLAYLISTMODEL_H
#define AUDIOPLAYLISTMODEL_H

#include "IModel.h"
#include "SoundClip.h"

#include <QObject>
#include <QJsonArray>

class AudioPlayListItem
{
	SoundClip *			m_pAudioAsset;
	uint					m_uiWeight;
	
public:
	AudioPlayListItem(SoundClip *pAsset, uint uiWeight = 10) :
		m_pAudioAsset(pAsset),
		m_uiWeight(uiWeight)
	{ }

	SoundClip *GetAudioAsset()		{ return m_pAudioAsset; }
	uint GetWeight() const			{ return m_uiWeight; }
	void SetWeight(uint uiWeight)	{ m_uiWeight = uiWeight; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////

class AudioPlayListModel : public QAbstractTableModel
{

	QList<AudioPlayListItem *>						m_PlayList;
	QMap<QUuid, QPair<int, AudioPlayListItem *>>	m_RemovedAssetIdMap;	// Used to reinsert frames (via undo/redo) while keeping their attributes

public:
	enum ColumnIndex
	{
		COLUMN_Audio = 0,
		COLUMN_Weight,
		COLUMN_Percent,

		NUMCOLUMNS
	};

	AudioPlayListModel(QObject *pParent);
	virtual ~AudioPlayListModel();

	int Add(SoundClip *pAsset);						// Returns the index the audio asset was inserted to
	void Remove(SoundClip *pAsset);
	void MoveRowUp(int iIndex);
	void MoveRowDown(int iIndex);
	void SetWeight(int iIndex, uint uiWeight);			// iIndex of -1 will apply to all

	QList<TreeModelItemData *> GetSoundClips() const;
	QJsonArray GenPlayListArray() const;
	AudioPlayListItem *GetPlayListItemAt(int iIndex);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const override;

Q_SIGNALS:
	void editCompleted(const QString &);
};

#endif // AUDIOPLAYLISTMODEL_H
