/**************************************************************************
 *	SpriteUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SPRITEUNDOCMDS_H
#define SPRITEUNDOCMDS_H

#include <QUndoCommand>
#include <QTableWidget>

class AtlasFrame;
class SpriteTableView;
class ProjectItemData;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_AddFrames : public QUndoCommand
{
	ProjectItemData &				m_SpriteItemRef;
	int								m_iStateIndex;

	QList<AtlasFrame *>				m_FrameList;

public:
	SpriteUndoCmd_AddFrames(ProjectItemData &spriteItemRef, int iStateIndex, QList<AtlasFrame *> frameList, QUndoCommand *pParent = nullptr);
	virtual ~SpriteUndoCmd_AddFrames();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_RemoveFrames : public QUndoCommand
{
	ProjectItemData &			m_SpriteItemRef;
	int							m_iStateIndex;

	QList<AtlasFrame *>			m_FrameList;

public:
	SpriteUndoCmd_RemoveFrames(ProjectItemData &spriteItemRef, int iStateIndex, QList<AtlasFrame *> frameList, QUndoCommand *pParent = nullptr);
	virtual ~SpriteUndoCmd_RemoveFrames();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_OrderFrame : public QUndoCommand
{
	SpriteTableView *     m_pSpriteTableView;
	int                         m_iFrameIndex;
	int                         m_iFrameIndexDest;
	
public:
	SpriteUndoCmd_OrderFrame(SpriteTableView *pSpriteTableView, int iFrameIndex, int iFrameIndexDestination, QUndoCommand *pParent = 0);
	virtual ~SpriteUndoCmd_OrderFrame();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_PositionFrame : public QUndoCommand
{
	SpriteTableView *           m_pSpriteTableView;
	int                         m_iFrameIndex;
	QList<QPoint>               m_OriginalOffsetList;
	QList<QPoint>               m_vNewOffsetList;
	
public:
	SpriteUndoCmd_PositionFrame(SpriteTableView *pSpriteTableView, int iIndex, QPoint ptNewPos, bool bApplyAsOffset, QUndoCommand *pParent = 0);
	virtual ~SpriteUndoCmd_PositionFrame();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_SetXFrame : public QUndoCommand
{
	SpriteTableView *     m_pSpriteTableView;
	int                         m_iFrameIndex;
	QList<QPoint>               m_OriginalOffsetList;
	QList<int>                  m_NewOffsetList;

public:
	SpriteUndoCmd_SetXFrame(SpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent = 0);
	virtual ~SpriteUndoCmd_SetXFrame();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_SetYFrame : public QUndoCommand
{
	SpriteTableView *     m_pSpriteTableView;
	int                         m_iFrameIndex;
	QList<QPoint>               m_OriginalOffsetList;
	QList<int>                  m_NewOffsetList;

public:
	SpriteUndoCmd_SetYFrame(SpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent = 0);
	virtual ~SpriteUndoCmd_SetYFrame();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_DurationFrame : public QUndoCommand
{
	SpriteTableView *    m_pSpriteTableView;
	int                        m_iFrameIndex;
	QList<float>               m_OriginalDurationList;
	float                      m_fNewDuration;
	
public:
	SpriteUndoCmd_DurationFrame(SpriteTableView *pSpriteTableView, int iIndex, float fDuration, QUndoCommand *pParent = 0);
	virtual ~SpriteUndoCmd_DurationFrame();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SpriteUndoCmd_GenerateStates : public QUndoCommand
{
	ProjectItemData &					m_SpriteItemRef;
	QMap<QString, QList<AtlasFrame *>>	m_ImportMap;

public:
	SpriteUndoCmd_GenerateStates(ProjectItemData &spriteItemRef, QMap<QString, QList<AtlasFrame *>> importMap, QUndoCommand *pParent = 0);
	virtual ~SpriteUndoCmd_GenerateStates();

	virtual void redo() override;
	virtual void undo() override;
};

#endif // SPRITEUNDOCMDS_H
