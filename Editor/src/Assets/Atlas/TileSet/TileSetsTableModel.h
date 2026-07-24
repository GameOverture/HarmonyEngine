/**************************************************************************
 *	TileSetsTableModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILESETSTABLEMODEL_H
#define TILESETSTABLEMODEL_H

#include "AtlasTileSet.h"
#include "PropertiesTreeModel.h"

class TileSetsTableModel : public QAbstractTableModel
{
	Q_OBJECT

	QList<SpriteFrame *>						m_FramesList;
	QMap<QUuid, QPair<int, SpriteFrame *>>		m_RemovedFrameIdMap;  // Used to reinsert frames (via undo/redo) while keeping their attributes

public:
	enum eColumn
	{
		COLUMN_Name = 0,
		COLUMN_OffsetX,
		COLUMN_OffsetY,
		COLUMN_Duration,

		NUMCOLUMNS
	};

	SpriteFramesModel(QObject *pParent);

	int Add(AtlasFrame *pFrame);						// Returns the index the frame was inserted to
	void Remove(AtlasFrame *pFrame);
	void MoveRowUp(int iIndex);
	void MoveRowDown(int iIndex);
	void SetFrameOffset(int iIndex, QPoint vOffset);	// iIndex of -1 will apply to all
	void AddFrameOffset(int iIndex, QPoint vOffset);	// iIndex of -1 will apply to all
	void DurationFrame(int iIndex, float fDuration);	// iIndex of -1 will apply to all

	QJsonArray GetFramesInfo(float &fTotalDurationRef);
	SpriteFrame *GetFrameAt(int iIndex);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;
	virtual Qt::ItemFlags flags(const QModelIndex & index) const override;

Q_SIGNALS:
	void editCompleted(const QString &);
};

#endif // TILESETSTABLEMODEL_H
