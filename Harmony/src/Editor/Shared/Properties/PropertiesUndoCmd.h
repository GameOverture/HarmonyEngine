/**************************************************************************
*	PropertiesUndoCmd.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef PROPERTIESUNDOCMD_H
#define PROPERTIESUNDOCMD_H

#include <QUndoCommand>
#include "PropertiesTreeModel.h"

class PropertiesUndoCmd : public QUndoCommand
{
	PropertiesTreeModel &   m_ModelRef;
	const int               m_iSTATE_INDEX;
	const QVariant          m_iSUBSTATE;
	PropertiesTreeItem &    m_TreeItemRef;
	QModelIndex             m_Index;

	QVariant                m_NewData;
	QVariant                m_OldData;

	int                     m_iRole;

public:
	PropertiesUndoCmd(PropertiesTreeModel &modelRef,
					  int iStateIndex,
					  const QVariant &subState,
					  PropertiesTreeItem &treeItemRef,
					  const QModelIndex &index,
					  const QVariant &newData,
					  int iRole,
					  QUndoCommand *pParent = 0);

	virtual ~PropertiesUndoCmd();
	virtual void redo() override;
	virtual void undo() override;
};

#endif // PROPERTIESUNDOCMD_H
