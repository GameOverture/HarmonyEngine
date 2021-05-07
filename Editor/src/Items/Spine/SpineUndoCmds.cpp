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

SpineUndoCmds::SpineUndoCmds(SpineCmd eCMD, ProjectItemData &spineItemRef, QList<QVariant> parameterList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_eCMD(eCMD),
	m_ParameterList(parameterList),
	m_SpineItemRef(spineItemRef),
	m_iStateIndex(-1)
{
	if(m_SpineItemRef.GetType() != ITEM_Spine)
		HyGuiLog("SpineUndoCmds recieved wrong type: " % QString::number(m_SpineItemRef.GetType()) , LOGTYPE_Error);

	switch(m_eCMD)
	{
	case SPINECMD_AddNewChildren:
		setText("Add New Child Node(s)");
		break;

	case SPINECMD_AddPrimitive:
		setText("Add Primitive");
		break;
	}

	if(spineItemRef.GetWidget())
		m_iStateIndex = spineItemRef.GetWidget()->GetCurStateIndex();
}

/*virtual*/ SpineUndoCmds::~SpineUndoCmds()
{
}

/*virtual*/ void SpineUndoCmds::redo() /*override*/
{
	switch(m_eCMD)
	{
	case SPINECMD_AddNewChildren: {
		//QList<TreeModelItemData *> itemList;
		//for(auto param : m_ParameterList)
		//{
		//	if(static_cast<SpineModel *>(m_SpineItemRef.GetModel())->GetNodeTreeModel().IsItemValid(param.value<TreeModelItemData *>(), true))
		//		itemList.push_back(param.value<TreeModelItemData *>());
		//}

		//static_cast<SpineModel *>(m_SpineItemRef.GetModel())->AddNewChildren(itemList);
		break; }

	case SPINECMD_AddPrimitive:
		break;
	}

	m_SpineItemRef.FocusWidgetState(m_iStateIndex, -1);
}

/*virtual*/ void SpineUndoCmds::undo() /*override*/
{
	switch(m_eCMD)
	{
	case SPINECMD_AddNewChildren: {
		//for(auto param : m_ParameterList)
		//{
		//	if(static_cast<SpineModel *>(m_SpineItemRef.GetModel())->GetNodeTreeModel().IsItemValid(param.value<TreeModelItemData *>(), true))
		//		static_cast<SpineModel *>(m_SpineItemRef.GetModel())->RemoveChild(param.value<TreeModelItemData *>());
		//}
		break; }

	case SPINECMD_AddPrimitive:
		break;
	}

	m_SpineItemRef.FocusWidgetState(m_iStateIndex, -1);
}
