/**************************************************************************
 *	EntityUndoCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "EntityUndoCmds.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EntityUndoCmd_AddNewChild::EntityUndoCmd_AddNewChild(SpriteTableView *pSpriteTableView, int iFrameIndex, int iFrameIndexDestination, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                    m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                                    m_iFrameIndex(iFrameIndex),
                                                                                                                                                                    m_iFrameIndexDest(iFrameIndexDestination)
{
    setText("New Child");
}

/*virtual*/ EntityUndoCmd_AddNewChild::~EntityUndoCmd_AddNewChild()
{
}

void EntityUndoCmd_AddNewChild::redo()
{
    GetTreeModel()
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

    m_pSpriteTableView->selectRow(m_iFrameIndexDest);
}

void EntityUndoCmd_AddNewChild::undo()
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

    m_pSpriteTableView->selectRow(m_iFrameIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
