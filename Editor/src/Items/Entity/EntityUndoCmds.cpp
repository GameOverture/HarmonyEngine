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

EntityUndoCmd_AddChildren::EntityUndoCmd_AddChildren(ProjectItemData &entityItemRef, QList<ProjectItemData *> projItemList, QUndoCommand *pParent /*= nullptr*/) :
	QUndoCommand(pParent),
	m_EntityItemRef(entityItemRef),
	m_ChildrenList(projItemList)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()), LOGTYPE_Error);

	setText("Add New Child Node(s)");
}

/*virtual*/ EntityUndoCmd_AddChildren::~EntityUndoCmd_AddChildren()
{
}

/*virtual*/ void EntityUndoCmd_AddChildren::redo() /*override*/
{
	QList<ProjectItemData *> itemList;
	for(auto *pProjItem : m_ChildrenList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pProjItem, true))
			itemList.push_back(pProjItem);
	}

	m_NodeList = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_AddNewChildren(itemList, -1);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

/*virtual*/ void EntityUndoCmd_AddChildren::undo() /*override*/
{
	for(auto *pNodeItem : m_NodeList)
	{
		if(static_cast<EntityModel *>(m_EntityItemRef.GetModel())->GetTreeModel().IsItemValid(pNodeItem, true))
			static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(pNodeItem);
	}
	m_NodeList.clear();

	//m_EntityItemRef.FocusWidgetState(0, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_PopChild::EntityUndoCmd_PopChild(ProjectItemData &entityItemRef, EntityTreeItemData *pNodeItem, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_pNode(pNodeItem),
	m_uiIndex(0)
{
	if(m_EntityItemRef.GetType() != ITEM_Entity)
		HyGuiLog("EntityUndoCmd recieved wrong type: " % QString::number(m_EntityItemRef.GetType()), LOGTYPE_Error);

	setText("Remove Child Node");
}

/*virtual*/ EntityUndoCmd_PopChild::~EntityUndoCmd_PopChild()
{
}

/*virtual*/ void EntityUndoCmd_PopChild::redo() /*override*/
{
	m_uiIndex = static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_RemoveTreeItem(m_pNode);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

/*virtual*/ void EntityUndoCmd_PopChild::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_ReaddChild(m_pNode, m_uiIndex);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_SelectionChanged::EntityUndoCmd_SelectionChanged(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> selectedItemDataList, QList<EntityTreeItemData *> deselectedItemDataList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_SelectedItemDataList(selectedItemDataList),
	m_DeselectedItemDataList(deselectedItemDataList)
{
	setText("Selection Changed");
}

/*virtual*/ EntityUndoCmd_SelectionChanged::~EntityUndoCmd_SelectionChanged()
{
}

/*virtual*/ void EntityUndoCmd_SelectionChanged::redo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_SelectionChanged(m_SelectedItemDataList, m_DeselectedItemDataList);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

/*virtual*/ void EntityUndoCmd_SelectionChanged::undo() /*override*/
{
	static_cast<EntityModel *>(m_EntityItemRef.GetModel())->Cmd_SelectionChanged(m_DeselectedItemDataList, m_SelectedItemDataList);
	//m_EntityItemRef.FocusWidgetState(0, -1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityUndoCmd_Transform::EntityUndoCmd_Transform(ProjectItemData &entityItemRef, const QList<EntityTreeItemData *> &affectedItemDataList, const QList<glm::mat4> &newTransformList, const QList<glm::mat4> &oldTransformList, QUndoCommand *pParent /*= nullptr*/) :
	m_EntityItemRef(entityItemRef),
	m_AffectedItemDataList(affectedItemDataList),
	m_NewTransformList(newTransformList),
	m_OldTransformList(oldTransformList)
{
	if(m_AffectedItemDataList.size() != m_NewTransformList.size() || m_AffectedItemDataList.size() != m_OldTransformList.size())
		HyGuiLog("EntityUndoCmd_Transform recieved improper sized lists", LOGTYPE_Error);

	setText("Transform " % QString::number(m_AffectedItemDataList.size()) % " Items");
}

/*virtual*/ EntityUndoCmd_Transform::~EntityUndoCmd_Transform()
{
}

/*virtual*/ void EntityUndoCmd_Transform::redo() /*override*/
{
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		glm::decompose(m_NewTransformList[i], vScale, quatRot, ptTranslation, vSkew, vPerspective);

		double dRotation = glm::degrees(glm::atan(m_NewTransformList[i][0][1], m_NewTransformList[i][0][0]));
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Position", QPointF(ptTranslation.x, ptTranslation.y));
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Rotation", dRotation);
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Scale", QPointF(vScale.x, vScale.y));
	}
}

/*virtual*/ void EntityUndoCmd_Transform::undo() /*override*/
{
	glm::vec3 vScale(1.0f);
	glm::quat quatRot;
	glm::vec3 ptTranslation;
	glm::vec3 vSkew;
	glm::vec4 vPerspective;
	for(int i = 0; i < m_AffectedItemDataList.size(); ++i)
	{
		glm::decompose(m_OldTransformList[i], vScale, quatRot, ptTranslation, vSkew, vPerspective);

		double dRotation = glm::degrees(glm::atan(m_OldTransformList[i][0][1], m_OldTransformList[i][0][0]));
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Position", QPointF(ptTranslation.x, ptTranslation.y));
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Rotation", dRotation);
		m_AffectedItemDataList[i]->GetPropertiesModel().SetPropertyValue("Transformation", "Scale", QPointF(vScale.x, vScale.y));
	}
}
