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
