/**************************************************************************
*	PropertiesUndoCmd.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "PropertiesUndoCmd.h"

PropertiesUndoCmd::PropertiesUndoCmd(PropertiesTreeModel &modelRef,
				  int iStateIndex,
				  const QVariant &subState,
				  PropertiesTreeItem &treeItemRef,
				  const QModelIndex &index,
				  const QVariant &newData,
				  int iRole,
				  QUndoCommand *pParent /*= 0*/) :	QUndoCommand(pParent),
													m_ModelRef(modelRef),
													m_iSTATE_INDEX(iStateIndex),
													m_iSUBSTATE(subState),
													m_TreeItemRef(treeItemRef),
													m_Index(index),
													m_NewData(newData),
													m_OldData(m_TreeItemRef.GetData()),
													m_iRole(iRole)
{
	setText(m_TreeItemRef.GetName());
}

/*virtual*/ PropertiesUndoCmd::~PropertiesUndoCmd()
{ }

/*virtual*/ void PropertiesUndoCmd::redo() /*override*/
{
	m_TreeItemRef.SetData(m_NewData);

	m_ModelRef.dataChanged(m_Index, m_Index, QVector<int>() << m_iRole);

	if(m_TreeItemRef.GetType() == PROPERTIESTYPE_CategoryChecked && m_TreeItemRef.GetNumChildren() != 0)
		m_ModelRef.RefreshProperties();

	m_ModelRef.GetItem().FocusWidgetState(m_iSTATE_INDEX, m_iSUBSTATE);
}

/*virtual*/ void PropertiesUndoCmd::undo() /*override*/
{
	m_TreeItemRef.SetData(m_OldData);

	m_ModelRef.dataChanged(m_Index, m_Index, QVector<int>() << m_iRole);

	if(m_TreeItemRef.GetType() == PROPERTIESTYPE_CategoryChecked && m_TreeItemRef.GetNumChildren() != 0)
		m_ModelRef.RefreshProperties();

	m_ModelRef.GetItem().FocusWidgetState(m_iSTATE_INDEX, m_iSUBSTATE);
}
