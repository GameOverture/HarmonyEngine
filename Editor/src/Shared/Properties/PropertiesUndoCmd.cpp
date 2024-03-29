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
	m_bDoFocusWidgetState(true)
{
	QString sText = pModel->GetPropertyName(m_ModelIndex);
	if(m_ModelIndex.column() == PROPERTIESCOLUMN_Name)
	{
		m_OldData = static_cast<bool>(pModel->GetPropertyDefinition(m_ModelIndex).eAccessType == PROPERTIESACCESS_ToggleOn);
		sText += " Checked";
	}
	else
	{
		m_OldData = pModel->GetPropertyValue(m_ModelIndex);
		sText += " Value Changed";
	}

	setText(sText);
}

/*virtual*/ PropertiesUndoCmd::~PropertiesUndoCmd()
{ }

/*virtual*/ void PropertiesUndoCmd::redo() /*override*/
{
	if(m_ModelIndex.column() == PROPERTIESCOLUMN_Name)
		m_pModel->SetToggle(m_ModelIndex, m_NewData.toBool());
	else
		m_pModel->setData(m_ModelIndex, m_NewData, Qt::UserRole);

	//if(m_pModel->GetPropertyDefinition(m_ModelIndex).IsCategory() == false)
	//	m_pModel->GetOwner().PropertyModified(*m_pModel, m_ModelIndex);

	if(m_bDoFocusWidgetState)
		m_pModel->GetOwner().FocusWidgetState(m_pModel->GetStateIndex(), m_pModel->GetSubstate());
}

/*virtual*/ void PropertiesUndoCmd::undo() /*override*/
{
	if(m_ModelIndex.column() == PROPERTIESCOLUMN_Name)
		m_pModel->SetToggle(m_ModelIndex, m_OldData.toBool());
	else
		m_pModel->setData(m_ModelIndex, m_OldData, Qt::UserRole);

	//if(m_pModel->GetPropertyDefinition(m_ModelIndex).IsCategory() == false)
	//	m_pModel->GetOwner().PropertyModified(*m_pModel, m_ModelIndex);
	
	if(m_bDoFocusWidgetState)
		m_pModel->GetOwner().FocusWidgetState(m_pModel->GetStateIndex(), m_pModel->GetSubstate());
}
