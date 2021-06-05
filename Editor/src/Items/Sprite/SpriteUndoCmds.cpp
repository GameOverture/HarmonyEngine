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
SpriteUndoCmd_OffsetFrame::SpriteUndoCmd_OffsetFrame(SpriteTableView *pSpriteTableView, int iIndex, QPoint vOffset, bool bAddOffset, QUndoCommand *pParent /*= 0*/) :
	QUndoCommand(pParent),
	m_pSpriteTableView(pSpriteTableView),
	m_iFrameIndex(iIndex)
{

	
	SpriteFramesModel *pSpriteFramesModel = static_cast<SpriteFramesModel *>(m_pSpriteTableView->model());

	if(m_iFrameIndex == -1)
	{
		setText("Translate Every Frame Offset");

		for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
		{
			m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_vOffset);
			m_vNewOffsetList.append(bAddOffset ? pSpriteFramesModel->GetFrameAt(i)->m_vOffset + vOffset : vOffset);
		}
	}
	else
	{
		setText("Translate Frame Offset");
		m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_vOffset);
		m_vNewOffsetList.append(bAddOffset ? pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_vOffset + vOffset : vOffset);
	}
}

/*virtual*/ SpriteUndoCmd_OffsetFrame::~SpriteUndoCmd_OffsetFrame()
{
}

void SpriteUndoCmd_OffsetFrame::redo()
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

void SpriteUndoCmd_OffsetFrame::undo()
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
SpriteUndoCmd_OffsetXFrame::SpriteUndoCmd_OffsetXFrame(SpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent /*= 0*/) :
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

/*virtual*/ SpriteUndoCmd_OffsetXFrame::~SpriteUndoCmd_OffsetXFrame()
{
}

void SpriteUndoCmd_OffsetXFrame::redo()
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

void SpriteUndoCmd_OffsetXFrame::undo()
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
SpriteUndoCmd_OffsetYFrame::SpriteUndoCmd_OffsetYFrame(SpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent /*= 0*/) :
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

/*virtual*/ SpriteUndoCmd_OffsetYFrame::~SpriteUndoCmd_OffsetYFrame()
{
}

void SpriteUndoCmd_OffsetYFrame::redo()
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

void SpriteUndoCmd_OffsetYFrame::undo()
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