/**************************************************************************
 *	EntityUndoCmds.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ENTITYUNDOCMDS_H
#define ENTITYUNDOCMDS_H

#include "EntityModel.h"

#include <QUndoCommand>

class EntityUndoCmd_AddChildren : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<ProjectItemData *>		m_ChildrenList;
	QList<EntityTreeItemData *>		m_NodeList;

public:
	EntityUndoCmd_AddChildren(ProjectItemData &entityItemRef, QList<ProjectItemData *> projItemList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddChildren();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PopItems : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_PoppedItemList;
	QList<int>						m_PoppedIndexList;

public:
	EntityUndoCmd_PopItems(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> poppedItemList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PopItems();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_AddNewShape : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EditorShape						m_eShape;
	QString							m_sData;
	bool							m_bIsPrimitive;
	int32							m_iIndex;
	
	EntityTreeItemData *			m_pShapeTreeItemData;

public:
	EntityUndoCmd_AddNewShape(ProjectItemData &entityItemRef, EditorShape eShape, QString sData, bool bIsPrimitive, int32 iRowIndex = -1, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddNewShape();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_SelectionChanged : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_SelectedItemDataList;
	QList<EntityTreeItemData *>		m_DeselectedItemDataList;

public:
	EntityUndoCmd_SelectionChanged(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> selectedItemDataList, QList<EntityTreeItemData *> deselectedItemDataList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_SelectionChanged();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_OrderChildren : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_SelectedItemDataList;
	QList<int>						m_PrevItemIndexList;
	QList<int>						m_NewItemIndexList;
	bool							m_bOrderUpwards;

public:
	EntityUndoCmd_OrderChildren(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> selectedItemDataList, QList<int> prevItemIndexList, QList<int> newItemIndexList, bool bOrderUpwards, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_OrderChildren();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_Transform : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_AffectedItemDataList;
	QList<glm::mat4>				m_NewTransformList;
	QList<glm::mat4>				m_OldTransformList;

public:
	EntityUndoCmd_Transform(ProjectItemData &entityItemRef, const QList<EntityTreeItemData *> &affectedItemDataList, const QList<glm::mat4> &newTransformList, const QList<glm::mat4> &oldTransformList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_Transform();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_ShapeData : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EntityTreeItemData *			m_pShapeItemData;
	QString							m_sNewData;
	QString							m_sPrevData;

public:
	EntityUndoCmd_ShapeData(ProjectItemData &entityItemRef, EntityTreeItemData *pShapeItemData, QString sNewData, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_ShapeData();

	virtual void redo() override;
	virtual void undo() override;
};

#endif // ENTITYUNDOCMDS_H
