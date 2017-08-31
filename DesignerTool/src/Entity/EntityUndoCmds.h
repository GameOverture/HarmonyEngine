/**************************************************************************
 *	EntityUndoCmds.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ENTITYUNDOCMDS_H
#define ENTITYUNDOCMDS_H

#include <QUndoCommand>
//#include <QTableWidget>

class EntityTreeItem;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityUndoCmd_AddNewChild : public QUndoCommand
{
    ProjectItem *               m_pSpriteTableView;
    int                         m_iFrameIndex;
    int                         m_iFrameIndexDest;

public:
    EntityUndoCmd_AddNewChild(EntityTreeItem *pParentTreeItem, int iFrameIndex, int iFrameIndexDestination, QUndoCommand *pParent = 0);
    virtual ~EntityUndoCmd_AddNewChild();

    void redo() override;
    void undo() override;
};

#endif // ENTITYUNDOCMDS_H
