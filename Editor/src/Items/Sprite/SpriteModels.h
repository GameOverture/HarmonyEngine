/**************************************************************************
 *	SpriteModels.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEMODELS_H
#define SPRITEMODELS_H

#include "IModel.h"
#include "AtlasFrame.h"
#include "GlobalWidgetMappers.h"

#include <QObject>
#include <QJsonArray>

class SpriteItem;

class SpriteFrame
{
public:
	AtlasFrame *			m_pFrame;
	QPoint					m_vOffset;
	float					m_fDuration;

	SpriteFrame(AtlasFrame *pFrame) :
		m_pFrame(pFrame),
		m_vOffset(0, 0),
		m_fDuration(1.0f / 30.0f)
	{ }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteFramesModel : public QAbstractTableModel
{
	Q_OBJECT

	QList<SpriteFrame *>						m_FramesList;
	QMap<QUuid, QPair<int, SpriteFrame *>>		m_RemovedFrameIdMap;  // Used to reinsert frames (via undo/redo) while keeping their attributes

public:
	enum eColumn
	{
		COLUMN_Frame = 0,
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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteStateData : public IStateData
{
	CheckBoxMapper *    m_pChkMapper_Loop;
	CheckBoxMapper *    m_pChkMapper_Reverse;
	CheckBoxMapper *    m_pChkMapper_Bounce;
	SpriteFramesModel * m_pFramesModel;

public:
	SpriteStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~SpriteStateData();
	
	CheckBoxMapper *GetLoopMapper();
	CheckBoxMapper *GetReverseMapper();
	CheckBoxMapper *GetBounceMapper();
	SpriteFramesModel *GetFramesModel();

	//void GetStateFileData(FileDataPair &stateFileDataOut);

	QList<IAssetItemData *> GetAtlasFrames() const;
	
	int Cmd_AddFrame(AtlasFrame *pFrame);
	void Cmd_RemoveFrame(AtlasFrame *pFrame);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteModel : public IModel
{
	Q_OBJECT

public:
	SpriteModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~SpriteModel();

	// Command Modifiers (Cmd_) - These mutate the internal state and should only be called from this constructor and from UndoCmd's
	int Cmd_AddFrames(int iStateIndex, QList<AtlasFrame *> frameList);
	void Cmd_RemoveFrames(int iStateIndex, QList<AtlasFrame *> frameList);
	
	virtual bool OnPrepSave() override { return true; }
	virtual void InsertItemSpecificData(FileDataPair &itemFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SPRITEMODELS_H
