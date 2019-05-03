/**************************************************************************
 *	EntityUndoCmds.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "EntityUndoCmds.h"
#include "EntityModel.h"
#include "EntityWidget.h"

EntityUndoCmd::EntityUndoCmd(EntityCmd eCMD, ProjectItem &entityItemRef, ExplorerItem *pParameter, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_eCMD(eCMD),
	m_pParameter(pParameter),
	m_EntityItemRef(entityItemRef)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()) , LOGTYPE_Error);

	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChild:
		setText("Add New Child");
		break;

	case ENTITYCMD_AddPrimitive:
		setText("Add Primitive");
		break;
	}
}

/*virtual*/ EntityUndoCmd::~EntityUndoCmd()
{
}

/*virtual*/ void EntityUndoCmd::redo() /*override*/
{
	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChild:
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->AddNewChild(m_pParameter);
		break;
	case ENTITYCMD_AddPrimitive:
		break;
	}
}

/*virtual*/ void EntityUndoCmd::undo() /*override*/
{
	switch(m_eCMD)
	{
	case ENTITYCMD_AddNewChild:
		static_cast<EntityModel *>(m_EntityItemRef.GetModel())->RemoveChild(m_pParameter);
		break;
	case ENTITYCMD_AddPrimitive:
		break;
	}
}
