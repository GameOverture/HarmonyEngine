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
	m_ItemRef(*pModel->GetProjItem()),
	m_iStateIndex(pModel->GetStateIndex()),
	m_Substate(pModel->GetSubstate()),
	m_pModel(pModel),
	m_CatPropPair(pModel->GetCatPropPairName(index)),
	m_bIsColumnNameToggle(index.column() == PROPERTIESCOLUMN_Name),
	m_NewData(newData)
{
	QString sText = m_CatPropPair.second.isEmpty() ? m_CatPropPair.first : m_CatPropPair.second;
	
	if(m_bIsColumnNameToggle)
	{
		Qt::CheckState eOldCheckState;
		if(m_pModel->GetIndexDefinition(index).eAccessType == PROPERTIESACCESS_ToggleChecked)
		{
			sText += " Checked";
			eOldCheckState = Qt::Checked;
		}
		else if(m_pModel->GetIndexDefinition(index).eAccessType == PROPERTIESACCESS_ToggleUnchecked)
		{
			sText += " Unchecked";
			eOldCheckState = Qt::Unchecked;
		}
		else if(m_pModel->GetIndexDefinition(index).eAccessType == PROPERTIESACCESS_TogglePartial)
		{
			sText += " Partially Checked";
			eOldCheckState = Qt::PartiallyChecked;
		}
		else
		{
			HyGuiLog("PropertiesUndoCmd::PropertiesUndoCmd() - Invalid AccessType", LOGTYPE_Error);
			eOldCheckState = Qt::Checked;
		}

		m_OldData = eOldCheckState;
	}
	else
	{
		m_OldData = m_pModel->GetIndexValue(index);
		sText += " Value Changed";
	}

	setText(sText);
}

/*virtual*/ PropertiesUndoCmd::~PropertiesUndoCmd()
{ }

/*virtual*/ void PropertiesUndoCmd::redo() /*override*/
{
	// HACK: Avoid using m_pModel outside of OnRedo() because entities delete their multi-models
	OnRedo();
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_Substate);
}

/*virtual*/ void PropertiesUndoCmd::undo() /*override*/
{
	// HACK: Avoid using m_pModel outside of OnUndo() because entities delete their multi-models
	OnUndo();
	m_ItemRef.FocusWidgetState(m_iStateIndex, m_Substate);
}

/*virtual*/ void PropertiesUndoCmd::OnRedo()
{
	if(m_bIsColumnNameToggle)
		m_pModel->SetToggleState(m_CatPropPair.first, m_CatPropPair.second, m_NewData.value<Qt::CheckState>());
	else
		m_pModel->SetPropertyValue(m_CatPropPair.first, m_CatPropPair.second, m_NewData);
}

/*virtual*/ void PropertiesUndoCmd::OnUndo()
{
	if(m_bIsColumnNameToggle)
		m_pModel->SetToggleState(m_CatPropPair.first, m_CatPropPair.second, m_OldData.value<Qt::CheckState>());
	else
		m_pModel->SetPropertyValue(m_CatPropPair.first, m_CatPropPair.second, m_OldData);
}
