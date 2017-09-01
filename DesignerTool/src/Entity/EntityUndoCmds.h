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

class EntityTreeItem;
class ProjectItem;
class EntityTreeModel;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityUndoCmd_AddNewChild : public QUndoCommand
{
    EntityTreeItem *    m_pParentTreeItem;
    EntityTreeModel *   m_pTreeModel;
    ProjectItem *       m_pItem;
    EntityTreeItem *    m_pNewTreeItem;

    int                 m_iRow;

public:
    EntityUndoCmd_AddNewChild(EntityTreeItem *pParentTreeItem, EntityTreeModel *pTreeModel, ProjectItem *pItem, QUndoCommand *pParent = 0);
    virtual ~EntityUndoCmd_AddNewChild();

    void redo() override;
    void undo() override;
};

#endif // ENTITYUNDOCMDS_H
