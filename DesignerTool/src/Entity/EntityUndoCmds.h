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

class ProjectItem;
class EntityWidget;
class EntityModel;
class EntityTreeItem;
class EntityTreeModel;

enum EntityCmd
{
    ENTITYCMD_AddNewChild = 0,
    ENTITYCMD_AddPrimitive,
};

class EntityUndoCmd : public QUndoCommand
{
    // NOTE: Member order matters!
    const EntityCmd     m_eCMD;
    ProjectItem &       m_ItemRef;
    void *              m_pParameter;

    EntityWidget *      m_pWidget;
    EntityModel *       m_pModel;
    EntityTreeItem *    m_pParentTreeItem;

    int                 m_iRow;

public:
    EntityUndoCmd(EntityCmd eCMD, ProjectItem &itemRef, void *pParameter, QUndoCommand *pParent = 0);
    virtual ~EntityUndoCmd();

    void redo() override;
    void undo() override;
};

#endif // ENTITYUNDOCMDS_H
