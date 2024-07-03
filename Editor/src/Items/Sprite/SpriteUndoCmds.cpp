/**************************************************************************
 *	SpriteUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SpriteUndoCmds.h"
#include "SpriteTableView.h"
#include "SpriteModels.h"
#include "Project.h"
#include "AtlasModel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteUndoCmd_AddFrames::SpriteUndoCmd_AddFrames(ProjectItemData &spriteItemRef, int iStateIndex, QList<AtlasFrame *> frameList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_SpriteItemRef(spriteItemRef),
	m_iStateIndex(iStateIndex),
	m_FrameList(frameList)
{
	setText("Add " % QString::number(m_FrameList.size()) % " Frames to State " % QString::number(iStateIndex));
}

/*virtual*/ SpriteUndoCmd_AddFrames::~SpriteUndoCmd_AddFrames()
{
}

/*virtual*/ void SpriteUndoCmd_AddFrames::redo() /*override*/
{
	QVariant focusSubState = static_cast<SpriteModel *>(m_SpriteItemRef.GetModel())->Cmd_AddFrames(m_iStateIndex, m_FrameList);
	m_SpriteItemRef.FocusWidgetState(m_iStateIndex, focusSubState);
}

/*virtual*/ void SpriteUndoCmd_AddFrames::undo() /*override*/
{
	static_cast<SpriteModel *>(m_SpriteItemRef.GetModel())->Cmd_RemoveFrames(m_iStateIndex, m_FrameList);
	m_SpriteItemRef.FocusWidgetState(m_iStateIndex, -1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteUndoCmd_RemoveFrames::SpriteUndoCmd_RemoveFrames(ProjectItemData &spriteItemRef, int iStateIndex, QList<AtlasFrame *> frameList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_SpriteItemRef(spriteItemRef),
	m_iStateIndex(iStateIndex),
	m_FrameList(frameList)
{
	setText("Remove " % QString::number(m_FrameList.size()) % " Frames from State " % QString::number(iStateIndex));
}

/*virtual*/ SpriteUndoCmd_RemoveFrames::~SpriteUndoCmd_RemoveFrames()
{ }

/*virtual*/ void SpriteUndoCmd_RemoveFrames::redo() /*override*/
{
	static_cast<SpriteModel *>(m_SpriteItemRef.GetModel())->Cmd_RemoveFrames(m_iStateIndex, m_FrameList);
	m_SpriteItemRef.FocusWidgetState(m_iStateIndex, -1);
}

/*virtual*/ void SpriteUndoCmd_RemoveFrames::undo() /*override*/
{
	QVariant focusSubState = static_cast<SpriteModel *>(m_SpriteItemRef.GetModel())->Cmd_AddFrames(m_iStateIndex, m_FrameList);
	m_SpriteItemRef.FocusWidgetState(m_iStateIndex, focusSubState);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteUndoCmd_OrderFrame::SpriteUndoCmd_OrderFrame(SpriteTableView *pSpriteTableView, int iFrameIndex, int iFrameIndexDestination, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
	m_pSpriteTableView(pSpriteTableView),
	m_iFrameIndex(iFrameIndex),
	m_iFrameIndexDest(iFrameIndexDestination)
{
	if(m_iFrameIndex > m_iFrameIndexDest)
		setText("Order Frame Upwards");
	else
		setText("Order Frame Downwards");
}

/*virtual*/ SpriteUndoCmd_OrderFrame::~SpriteUndoCmd_OrderFrame()
{
}

void SpriteUndoCmd_OrderFrame::redo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());
	
	int iOffset = m_iFrameIndexDest - m_iFrameIndex;
	while(iOffset > 0)
	{
		pSpriteFramesModel->MoveRowDown(m_iFrameIndex);
		iOffset--;
	}
	
	while(iOffset < 0)
	{
		pSpriteFramesModel->MoveRowUp(m_iFrameIndex);
		iOffset++;
	}
	
	// TODO: replace this with ProjectItem::FocusWidgetState
	m_pSpriteTableView->selectRow(m_iFrameIndexDest);
}

void SpriteUndoCmd_OrderFrame::undo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());
	
	int iOffset = m_iFrameIndex - m_iFrameIndexDest;
	while(iOffset > 0)
	{
		pSpriteFramesModel->MoveRowDown(m_iFrameIndexDest);
		iOffset--;
	}
	
	while(iOffset < 0)
	{
		pSpriteFramesModel->MoveRowUp(m_iFrameIndexDest);
		iOffset++;
	}
	
	// TODO: replace this with ProjectItem::FocusWidgetState
	m_pSpriteTableView->selectRow(m_iFrameIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpriteUndoCmd_PositionFrame::SpriteUndoCmd_PositionFrame(SpriteTableView *pSpriteTableView, int iIndex, QPoint ptNewPos, bool bApplyAsOffset, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
	m_pSpriteTableView(pSpriteTableView),
	m_iFrameIndex(iIndex)
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		setText("Translate All Frames");

		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
		{
			m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_vOffset);
			m_vNewOffsetList.append(bApplyAsOffset ? pSpriteFramesModel->GetFrameAt(i)->m_vOffset + ptNewPos : ptNewPos);
		}
	}
	else
	{
		setText("Translate Frame");
		m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_vOffset);
		m_vNewOffsetList.append(bApplyAsOffset ? pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_vOffset + ptNewPos : ptNewPos);
	}
}

/*virtual*/ SpriteUndoCmd_PositionFrame::~SpriteUndoCmd_PositionFrame()
{
}

void SpriteUndoCmd_PositionFrame::redo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			pSpriteFramesModel->SetFrameOffset(i, m_vNewOffsetList[i]);
	}
	else
	{
		pSpriteFramesModel->SetFrameOffset(m_iFrameIndex, m_vNewOffsetList[0]);
		m_pSpriteTableView->selectRow(m_iFrameIndex);
	}

	// TODO: replace this with ProjectItem::FocusWidgetState
}

void SpriteUndoCmd_PositionFrame::undo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			pSpriteFramesModel->SetFrameOffset(i, m_OriginalOffsetList[i]);
	}
	else
	{
		pSpriteFramesModel->SetFrameOffset(m_iFrameIndex, m_OriginalOffsetList[0]);
		m_pSpriteTableView->selectRow(m_iFrameIndex);
	}

	// TODO: replace this with ProjectItem::FocusWidgetState
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpriteUndoCmd_SetXFrame::SpriteUndoCmd_SetXFrame(SpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
	m_pSpriteTableView(pSpriteTableView),
	m_iFrameIndex(iIndex),
	m_NewOffsetList(newOffsetList)
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		setText("Translate Every Frame X Offset");

		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_vOffset);
	}
	else
	{
		setText("Translate Frame X Offset");
		m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_vOffset);
	}
}

/*virtual*/ SpriteUndoCmd_SetXFrame::~SpriteUndoCmd_SetXFrame()
{
}

void SpriteUndoCmd_SetXFrame::redo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
		{
			QPoint vOffset(m_NewOffsetList[i], m_OriginalOffsetList[i].y());
			pSpriteFramesModel->SetFrameOffset(i, vOffset);
		}
	}
	else
	{
		QPoint vOffset(m_NewOffsetList[0], m_OriginalOffsetList[0].y());
		pSpriteFramesModel->SetFrameOffset(m_iFrameIndex, vOffset);
		m_pSpriteTableView->selectRow(m_iFrameIndex);
	}
}

void SpriteUndoCmd_SetXFrame::undo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			pSpriteFramesModel->SetFrameOffset(i, m_OriginalOffsetList[i]);
	}
	else
	{
		pSpriteFramesModel->SetFrameOffset(m_iFrameIndex, m_OriginalOffsetList[0]);
		m_pSpriteTableView->selectRow(m_iFrameIndex);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpriteUndoCmd_SetYFrame::SpriteUndoCmd_SetYFrame(SpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
	m_pSpriteTableView(pSpriteTableView),
	m_iFrameIndex(iIndex),
	m_NewOffsetList(newOffsetList)
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		setText("Translate Every Frame Y Offset");

		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_vOffset);
	}
	else
	{
		setText("Translate Frame Y Offset");
		m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_vOffset);
	}
}

/*virtual*/ SpriteUndoCmd_SetYFrame::~SpriteUndoCmd_SetYFrame()
{
}

void SpriteUndoCmd_SetYFrame::redo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
		{
			QPoint vOffset(m_OriginalOffsetList[i].x(), m_NewOffsetList[i]);
			pSpriteFramesModel->SetFrameOffset(i, vOffset);
		}
	}
	else
	{
		QPoint vOffset(m_OriginalOffsetList[0].x(), m_NewOffsetList[0]);
		pSpriteFramesModel->SetFrameOffset(m_iFrameIndex, vOffset);
		m_pSpriteTableView->selectRow(m_iFrameIndex);
	}
}

void SpriteUndoCmd_SetYFrame::undo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			pSpriteFramesModel->SetFrameOffset(i, m_OriginalOffsetList[i]);
	}
	else
	{
		pSpriteFramesModel->SetFrameOffset(m_iFrameIndex, m_OriginalOffsetList[0]);
		m_pSpriteTableView->selectRow(m_iFrameIndex);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SpriteUndoCmd_DurationFrame::SpriteUndoCmd_DurationFrame(SpriteTableView *pSpriteTableView, int iIndex, float fDuration, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
	m_pSpriteTableView(pSpriteTableView),
	m_iFrameIndex(iIndex),
	m_fNewDuration(fDuration)
{
	setText("Modify Frame Duration");
	
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			m_OriginalDurationList.append(pSpriteFramesModel->GetFrameAt(i)->m_fDuration);
	}
	else
		m_OriginalDurationList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_fDuration);
}

/*virtual*/ SpriteUndoCmd_DurationFrame::~SpriteUndoCmd_DurationFrame()
{
}

void SpriteUndoCmd_DurationFrame::redo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			pSpriteFramesModel->DurationFrame(i, m_fNewDuration);
	}
	else
	{
		pSpriteFramesModel->DurationFrame(m_iFrameIndex, m_fNewDuration);
		m_pSpriteTableView->selectRow(m_iFrameIndex);
	}
}

void SpriteUndoCmd_DurationFrame::undo()
{
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
			pSpriteFramesModel->DurationFrame(i, m_OriginalDurationList[i]);
	}
	else
	{
		pSpriteFramesModel->DurationFrame(m_iFrameIndex, m_OriginalDurationList[0]);
		m_pSpriteTableView->selectRow(m_iFrameIndex);
	}
}
