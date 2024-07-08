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
		Qt::CheckState eOldCheckState = Qt::Checked;
		if(pModel->GetPropertyDefinition(m_ModelIndex).eAccessType == PROPERTIESACCESS_ToggleChecked)
		{
			sText += " Checked";
			eOldCheckState = Qt::Checked;
		}
		else if(pModel->GetPropertyDefinition(m_ModelIndex).eAccessType == PROPERTIESACCESS_ToggleUnchecked)
		{
			sText += " Unchecked";
			eOldCheckState = Qt::Unchecked;
		}
		else if(pModel->GetPropertyDefinition(m_ModelIndex).eAccessType == PROPERTIESACCESS_TogglePartial)
		{
			sText += " Partially Checked";
			eOldCheckState = Qt::PartiallyChecked;
		}
		else
			HyGuiLog("PropertiesUndoCmd::PropertiesUndoCmd() - Invalid AccessType", LOGTYPE_Error);

		m_OldData = eOldCheckState;
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
		m_pModel->SetToggleState(m_ModelIndex, m_NewData.value<Qt::CheckState>());
	else
		m_pModel->setData(m_ModelIndex, m_NewData, Qt::UserRole);

	if(m_bDoFocusWidgetState)
		m_pModel->GetOwner().FocusWidgetState(m_pModel->GetStateIndex(), m_pModel->GetSubstate());
}

/*virtual*/ void PropertiesUndoCmd::undo() /*override*/
{
	if(m_ModelIndex.column() == PROPERTIESCOLUMN_Name)
		m_pModel->SetToggleState(m_ModelIndex, m_OldData.value<Qt::CheckState>());
	else
		m_pModel->setData(m_ModelIndex, m_OldData, Qt::UserRole);
	
	if(m_bDoFocusWidgetState)
		m_pModel->GetOwner().FocusWidgetState(m_pModel->GetStateIndex(), m_pModel->GetSubstate());
}
