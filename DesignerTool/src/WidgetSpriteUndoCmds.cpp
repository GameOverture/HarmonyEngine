/**************************************************************************
 *	WidgetSpriteUndoCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "WidgetSpriteUndoCmds.h"

#include "WidgetSpriteModelView.h"
#include "ItemSprite.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WidgetSpriteUndoCmd_OrderFrame::WidgetSpriteUndoCmd_OrderFrame(WidgetSpriteTableView *pSpriteTableView, int iFrameIndex, int iFrameIndexDestination, QUndoCommand *pParent /*= 0*/) :   QUndoCommand(pParent),
                                                                                                                                                                                        m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                                                        m_iFrameIndex(iFrameIndex),
                                                                                                                                                                                        m_iFrameIndexDest(iFrameIndexDestination)
{
    if(m_iFrameIndex > m_iFrameIndexDest)
        setText("Order Frame Upwards");
    else
        setText("Order Frame Downwards");
}

/*virtual*/ WidgetSpriteUndoCmd_OrderFrame::~WidgetSpriteUndoCmd_OrderFrame()
{
}

void WidgetSpriteUndoCmd_OrderFrame::redo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());
    
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

void WidgetSpriteUndoCmd_OrderFrame::undo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());
    
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
WidgetSpriteUndoCmd_OffsetFrame::WidgetSpriteUndoCmd_OffsetFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, QPoint vOffset, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                                        m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                                        m_iFrameIndex(iIndex),
                                                                                                                                                                        m_vNewOffset(vOffset)
{

    
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        setText("Translate Every Frame Offset");

        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(i)->m_vOffset);
    }
    else
    {
        setText("Translate Frame Offset");
        m_OriginalOffsetList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_vOffset);
    }
}

/*virtual*/ WidgetSpriteUndoCmd_OffsetFrame::~WidgetSpriteUndoCmd_OffsetFrame()
{
}

void WidgetSpriteUndoCmd_OffsetFrame::redo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            pSpriteFramesModel->OffsetFrame(i, m_vNewOffset);
    }
    else
    {
        pSpriteFramesModel->OffsetFrame(m_iFrameIndex, m_vNewOffset);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

void WidgetSpriteUndoCmd_OffsetFrame::undo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            pSpriteFramesModel->OffsetFrame(i, m_OriginalOffsetList[i]);
    }
    else
    {
        pSpriteFramesModel->OffsetFrame(m_iFrameIndex, m_OriginalOffsetList[0]);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WidgetSpriteUndoCmd_OffsetXFrame::WidgetSpriteUndoCmd_OffsetXFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                                                    m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                                                    m_iFrameIndex(iIndex),
                                                                                                                                                                                    m_NewOffsetList(newOffsetList)
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

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

/*virtual*/ WidgetSpriteUndoCmd_OffsetXFrame::~WidgetSpriteUndoCmd_OffsetXFrame()
{
}

void WidgetSpriteUndoCmd_OffsetXFrame::redo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
        {
            QPoint vOffset(m_NewOffsetList[i], m_OriginalOffsetList[i].y());
            pSpriteFramesModel->OffsetFrame(i, vOffset);
        }
    }
    else
    {
        QPoint vOffset(m_NewOffsetList[0], m_OriginalOffsetList[0].y());
        pSpriteFramesModel->OffsetFrame(m_iFrameIndex, vOffset);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

void WidgetSpriteUndoCmd_OffsetXFrame::undo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            pSpriteFramesModel->OffsetFrame(i, m_OriginalOffsetList[i]);
    }
    else
    {
        pSpriteFramesModel->OffsetFrame(m_iFrameIndex, m_OriginalOffsetList[0]);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WidgetSpriteUndoCmd_OffsetYFrame::WidgetSpriteUndoCmd_OffsetYFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent /*= 0*/) :  QUndoCommand(pParent),
                                                                                                                                                                                    m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                                                    m_iFrameIndex(iIndex),
                                                                                                                                                                                    m_NewOffsetList(newOffsetList)
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

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

/*virtual*/ WidgetSpriteUndoCmd_OffsetYFrame::~WidgetSpriteUndoCmd_OffsetYFrame()
{
}

void WidgetSpriteUndoCmd_OffsetYFrame::redo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
        {
            QPoint vOffset(m_OriginalOffsetList[i].x(), m_NewOffsetList[i]);
            pSpriteFramesModel->OffsetFrame(i, vOffset);
        }
    }
    else
    {
        QPoint vOffset(m_OriginalOffsetList[0].x(), m_NewOffsetList[0]);
        pSpriteFramesModel->OffsetFrame(m_iFrameIndex, vOffset);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

void WidgetSpriteUndoCmd_OffsetYFrame::undo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            pSpriteFramesModel->OffsetFrame(i, m_OriginalOffsetList[i]);
    }
    else
    {
        pSpriteFramesModel->OffsetFrame(m_iFrameIndex, m_OriginalOffsetList[0]);
        m_pSpriteTableView->selectRow(m_iFrameIndex);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
WidgetSpriteUndoCmd_DurationFrame::WidgetSpriteUndoCmd_DurationFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, float fDuration, QUndoCommand *pParent /*= 0*/) : QUndoCommand(pParent),
                                                                                                                                                                            m_pSpriteTableView(pSpriteTableView),
                                                                                                                                                                            m_iFrameIndex(iIndex),
                                                                                                                                                                            m_fNewDuration(fDuration)
{
    setText("Modify Frame Duration");
    
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

    if(m_iFrameIndex == -1)
    {
        for(int i = 0; i < pSpriteFramesModel->rowCount(); ++i)
            m_OriginalDurationList.append(pSpriteFramesModel->GetFrameAt(i)->m_fDuration);
    }
    else
        m_OriginalDurationList.append(pSpriteFramesModel->GetFrameAt(m_iFrameIndex)->m_fDuration);
}

/*virtual*/ WidgetSpriteUndoCmd_DurationFrame::~WidgetSpriteUndoCmd_DurationFrame()
{
}

void WidgetSpriteUndoCmd_DurationFrame::redo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

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

void WidgetSpriteUndoCmd_DurationFrame::undo()
{
    WidgetSpriteModel *pSpriteFramesModel = static_cast<WidgetSpriteModel *>(m_pSpriteTableView->model());

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
