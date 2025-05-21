/**************************************************************************
 *	SpineUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SpineUndoCmds.h"
#include "SpineModel.h"
#include "SpineWidget.h"

SpineUndoCmd_AddNewCrossFade::SpineUndoCmd_AddNewCrossFade(ProjectItemData &spineItemRef, QString sAnimOne, QString sAnimTwo, float fMixValue, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_SpineItemRef(spineItemRef),
	m_sAnimOne(sAnimOne),
	m_sAnimTwo(sAnimTwo),
	m_fMixValue(fMixValue)
{
	setText("Add New Mix");

	if(m_SpineItemRef.GetType() != ITEM_Spine)
		HyGuiLog("SpineUndoCmd_AddNewCrossFade recieved wrong item type: " % QString::number(m_SpineItemRef.GetType()) , LOGTYPE_Error);
}

/*virtual*/ SpineUndoCmd_AddNewCrossFade::~SpineUndoCmd_AddNewCrossFade()
{
}

/*virtual*/ void SpineUndoCmd_AddNewCrossFade::redo() /*override*/
{
	static_cast<SpineModel *>(m_SpineItemRef.GetModel())->Cmd_AppendMix(m_sAnimOne, m_sAnimTwo, m_fMixValue);
}

/*virtual*/ void SpineUndoCmd_AddNewCrossFade::undo() /*override*/
{
	static_cast<SpineModel *>(m_SpineItemRef.GetModel())->Cmd_RemoveMix(m_sAnimOne, m_sAnimTwo);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpineUndoCmd_ModifyCrossFade::SpineUndoCmd_ModifyCrossFade(ProjectItemData &spineItemRef, QTableView *pTableView, QModelIndex index, QVariant newData, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_SpineItemRef(spineItemRef),
	m_pTableView(pTableView),
	m_Index(index),
	m_NewData(newData)
{
	m_OldData = m_pTableView->model()->data(m_Index, Qt::UserRole);

	switch(index.column())
	{
	case SpineCrossFadeModel::COLUMN_AnimOne:
		setText("Modify anim crossfade (Anim One)");
		break;
	case SpineCrossFadeModel::COLUMN_Mix:
		setText("Modify anim crossfade (mix)");
		break;
	case SpineCrossFadeModel::COLUMN_AnimTwo:
		setText("Modify anim crossfade (Anim Two)");
		break;
	}
}

/*virtual*/ SpineUndoCmd_ModifyCrossFade::~SpineUndoCmd_ModifyCrossFade()
{
}

/*virtual*/ void SpineUndoCmd_ModifyCrossFade::redo() /*override*/
{
	SpineCrossFadeModel *pCrossFadeModel = static_cast<SpineCrossFadeModel *>(m_pTableView->model());
	switch(m_Index.column())
	{
	case SpineCrossFadeModel::COLUMN_AnimOne:
		pCrossFadeModel->SetAnimOne(m_Index.row(), m_NewData.toString());
		break;
	case SpineCrossFadeModel::COLUMN_Mix:
		pCrossFadeModel->SetMix(m_Index.row(), m_NewData.toFloat());
		break;
	case SpineCrossFadeModel::COLUMN_AnimTwo:
		pCrossFadeModel->SetAnimTwo(m_Index.row(), m_NewData.toString());
		break;
	}

	m_pTableView->selectRow(m_Index.row());
}

/*virtual*/ void SpineUndoCmd_ModifyCrossFade::undo() /*override*/
{
	SpineCrossFadeModel *pCrossFadeModel = static_cast<SpineCrossFadeModel *>(m_pTableView->model());
	switch(m_Index.column())
	{
	case SpineCrossFadeModel::COLUMN_AnimOne:
		pCrossFadeModel->SetAnimOne(m_Index.row(), m_OldData.toString());
		break;
	case SpineCrossFadeModel::COLUMN_Mix:
		pCrossFadeModel->SetMix(m_Index.row(), m_OldData.toFloat());
		break;
	case SpineCrossFadeModel::COLUMN_AnimTwo:
		pCrossFadeModel->SetAnimTwo(m_Index.row(), m_OldData.toString());
		break;
	}

	m_pTableView->selectRow(m_Index.row());
}
