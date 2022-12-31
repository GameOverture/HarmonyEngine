/**************************************************************************
*	PropertiesUndoCmd.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "PropertiesUndoCmd.h"

PropertiesUndoCmd::PropertiesUndoCmd(PropertiesTreeModel *pModel, const QModelIndex &index, const QVariant &newData, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_pModel(pModel),
	m_ModelIndex(index),
	m_NewData(newData),
	m_OldData(pModel->GetPropertyValue(index))
{
	setText(pModel->GetPropertyName(index));
}

/*virtual*/ PropertiesUndoCmd::~PropertiesUndoCmd()
{ }

/*virtual*/ void PropertiesUndoCmd::redo() /*override*/
{
	if(m_pModel->GetPropertyDefinition(m_ModelIndex).eType == PROPERTIESTYPE_CategoryChecked)
	{
		m_pModel->setData(m_pModel->index(m_ModelIndex.row(), 1/*COLUMN_Value*/, m_ModelIndex.parent()), m_NewData, Qt::UserRole);
		m_pModel->RefreshCategory(m_ModelIndex);
	}
	else
		m_pModel->setData(m_ModelIndex, m_NewData, Qt::UserRole);

	if(m_pModel->GetPropertyDefinition(m_ModelIndex).IsCategory() == false)
	{
		QString sCategory = m_pModel->GetPropertyName(m_ModelIndex.parent());
		QString sProperty = m_pModel->GetPropertyName(m_ModelIndex);
		m_pModel->GetOwner().PropertyModified(*m_pModel, sCategory, sProperty);
	}
	m_pModel->GetOwner().FocusWidgetState(m_pModel->GetStateIndex(), m_pModel->GetSubstate());
}

/*virtual*/ void PropertiesUndoCmd::undo() /*override*/
{
	if(m_pModel->GetPropertyDefinition(m_ModelIndex).eType == PROPERTIESTYPE_CategoryChecked)
	{
		m_pModel->setData(m_pModel->index(m_ModelIndex.row(), 1/*COLUMN_Value*/, m_ModelIndex.parent()), m_OldData, Qt::UserRole);
		m_pModel->RefreshCategory(m_ModelIndex);
	}
	else
		m_pModel->setData(m_ModelIndex, m_OldData, Qt::UserRole);

	if(m_pModel->GetPropertyDefinition(m_ModelIndex).IsCategory() == false)
	{
		QString sCategory = m_pModel->GetPropertyName(m_ModelIndex.parent());
		QString sProperty = m_pModel->GetPropertyName(m_ModelIndex);
		m_pModel->GetOwner().PropertyModified(*m_pModel, sCategory, sProperty);
	}
	m_pModel->GetOwner().FocusWidgetState(m_pModel->GetStateIndex(), m_pModel->GetSubstate());
}
