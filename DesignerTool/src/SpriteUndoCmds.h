/**************************************************************************
 *	WidgetSpriteUndoCmds.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETSPRITEUNDOCMDS_H
#define WIDGETSPRITEUNDOCMDS_H

#include <QUndoCommand>
#include <QTableWidget>

class WidgetSpriteTableView;
class ItemWidget;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteUndoCmd_OrderFrame : public QUndoCommand
{
    WidgetSpriteTableView *     m_pSpriteTableView;
    int                         m_iFrameIndex;
    int                         m_iFrameIndexDest;
    
public:
    WidgetSpriteUndoCmd_OrderFrame(WidgetSpriteTableView *pSpriteTableView, int iFrameIndex, int iFrameIndexDestination, QUndoCommand *pParent = 0);
    virtual ~WidgetSpriteUndoCmd_OrderFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteUndoCmd_OffsetFrame : public QUndoCommand
{
    WidgetSpriteTableView *     m_pSpriteTableView;
    int                         m_iFrameIndex;
    QList<QPoint>               m_OriginalOffsetList;
    QPoint                      m_vNewOffset;
    
public:
    WidgetSpriteUndoCmd_OffsetFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, QPoint vOffset, QUndoCommand *pParent = 0);
    virtual ~WidgetSpriteUndoCmd_OffsetFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteUndoCmd_OffsetXFrame : public QUndoCommand
{
    WidgetSpriteTableView *     m_pSpriteTableView;
    int                         m_iFrameIndex;
    QList<QPoint>               m_OriginalOffsetList;
    QList<int>                  m_NewOffsetList;

public:
    WidgetSpriteUndoCmd_OffsetXFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent = 0);
    virtual ~WidgetSpriteUndoCmd_OffsetXFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteUndoCmd_OffsetYFrame : public QUndoCommand
{
    WidgetSpriteTableView *     m_pSpriteTableView;
    int                         m_iFrameIndex;
    QList<QPoint>               m_OriginalOffsetList;
    QList<int>                  m_NewOffsetList;

public:
    WidgetSpriteUndoCmd_OffsetYFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, QList<int> newOffsetList, QUndoCommand *pParent = 0);
    virtual ~WidgetSpriteUndoCmd_OffsetYFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class WidgetSpriteUndoCmd_DurationFrame : public QUndoCommand
{
    WidgetSpriteTableView *    m_pSpriteTableView;
    int                        m_iFrameIndex;
    QList<float>               m_OriginalDurationList;
    float                      m_fNewDuration;
    
public:
    WidgetSpriteUndoCmd_DurationFrame(WidgetSpriteTableView *pSpriteTableView, int iIndex, float fDuration, QUndoCommand *pParent = 0);
    virtual ~WidgetSpriteUndoCmd_DurationFrame();

    void redo() Q_DECL_OVERRIDE;
    void undo() Q_DECL_OVERRIDE;
};

#endif // WIDGETSPRITEUNDOCMDS_H
