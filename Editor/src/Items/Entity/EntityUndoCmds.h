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
#include "PropertiesUndoCmd.h"
#include "EntityItemMimeData.h"

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

class EntityUndoCmd_AddWidget : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	ItemType						m_eWidgetType;
	EntityTreeItemData *			m_pWidgetTreeItemData;

public:
	EntityUndoCmd_AddWidget(ProjectItemData &entityItemRef, ItemType eWidgetType, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddWidget();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_AddAssets : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<IAssetItemData*>			m_AssetList;
	QList<EntityTreeItemData*>		m_NodeList;

public:
	EntityUndoCmd_AddAssets(ProjectItemData &entityItemRef, QList<IAssetItemData*> assetItemList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddAssets();

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

class EntityUndoCmd_PasteItems : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QJsonObject						m_PasteMimeObject;
	QList<EntityTreeItemData *>		m_PastedItemList;
	EntityTreeItemData *			m_pArrayFolder;

public:
	EntityUndoCmd_PasteItems(ProjectItemData &entityItemRef, QJsonObject pasteMimeObject, EntityTreeItemData *pArrayFolder, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PasteItems();

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
	ProjectItemData &m_EntityItemRef;
	int									m_iStateIndex;
	int									m_iFrameIndex;
	QList<EntityTreeItemData *>			m_AffectedItemDataList;
	QList<glm::mat4>					m_NewTransformList;
	QList<glm::mat4>					m_OldTransformList;
	QList<std::tuple<bool, bool, bool>>	m_CreatedKeyFrameList; // First: Translation, Second: Rotation, Third: Scale
	QList<QJsonArray>					m_OldShapeDataArrayList;

public:
	EntityUndoCmd_Transform(ProjectItemData &entityItemRef, int iStateIndex, int iFrameIndex, const QList<EntityTreeItemData *> &affectedItemDataList, const QList<glm::mat4> &newTransformList, const QList<glm::mat4> &oldTransformList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_Transform();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_AddPrimitive : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	int32							m_iIndex;

	EntityTreeItemData *			m_pPrimitiveTreeItemData;

public:
	EntityUndoCmd_AddPrimitive(ProjectItemData &entityItemRef, int32 iRowIndex = -1, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddPrimitive();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PrimitiveData : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	int								m_iStateIndex;
	int								m_iFrameIndex;
	EntityTreeItemData *			m_pPrimitiveItemData;
	
	QString							m_sNewType;
	QList<float>					m_NewData;

	QString							m_sOldType;
	QList<float>					m_OldData;

public:
	EntityUndoCmd_PrimitiveData(QString sText, ProjectItemData &entityItemRef, int iStateIndex, int iFrameIndex, EntityTreeItemData *pPrimitiveItemData, QString sNewType, const QList<float> &newData, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PrimitiveData();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_AddFixture : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	bool							m_bIsShape;
	int32							m_iIndex;

	EntityTreeItemData *			m_pShapeTreeItemData;

public:
	EntityUndoCmd_AddFixture(ProjectItemData &entityItemRef, bool bIsShape, int32 iRowIndex = -1, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddFixture();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_ShapeData : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	int								m_iStateIndex;
	int								m_iFrameIndex;
	EntityTreeItemData *			m_pShapeItemData;
	
	EditorShape						m_eNewType;
	QList<float>					m_NewData;

	EditorShape						m_eOldType;
	QList<float>					m_OldData;

public:
	EntityUndoCmd_ShapeData(QString sText, ProjectItemData &entityItemRef, int iStateIndex, int iFrameIndex, EntityTreeItemData *pShapeItemData, EditorShape eNewType, const QList<float> &newData, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_ShapeData();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_ChainData : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EntityTreeItemData *			m_pChainItemData;
	
	QList<float>					m_NewData;
	QList<float>					m_OldData;

public:
	EntityUndoCmd_ChainData(QString sText, ProjectItemData &entityItemRef, EntityTreeItemData *pChainItemData, const QList<float> &newData, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_ChainData();

	virtual void redo() override;
	virtual void undo() override;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//// NOTE: Converts to/from Primitive and Fixture Shapes, does not work with Fixture Chains
//class EntityUndoCmd_ConvertShape : public QUndoCommand
//{
//	ProjectItemData &				m_EntityItemRef;
//	EntityTreeItemData *			m_pNewShapeItemData;
//	EntityTreeItemData *			m_pPrevShapeItemData;
//	int								m_iPoppedIndex;
//
//public:
//	EntityUndoCmd_ConvertShape(ProjectItemData &entityItemRef, EntityTreeItemData *pShapeItemData, QUndoCommand *pParent = nullptr);
//	virtual ~EntityUndoCmd_ConvertShape();
//
//	virtual void redo() override;
//	virtual void undo() override;
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_RenameItem : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EntityTreeItemData *			m_pItemData;
	QString							m_sNewName;
	QString							m_sOldName;

public:
	EntityUndoCmd_RenameItem(ProjectItemData &entityItemRef, EntityTreeItemData *pItemData, QString sNewName, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_RenameItem();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_DuplicateToArray : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	EntityTreeItemData *			m_pItemData;
	int								m_iPoppedIndex;
	int								m_iArraySize;

	QList<EntityTreeItemData *>		m_ArrayItemList;

public:
	EntityUndoCmd_DuplicateToArray(ProjectItemData &entityItemRef, EntityTreeItemData *pItemData, int iArraySize, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_DuplicateToArray();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PackToArray : public QUndoCommand
{
	ProjectItemData &				m_EntityItemRef;
	QList<EntityTreeItemData *>		m_PackItemList;
	QString							m_sArrayName;
	int								m_iArrayFolderRow;
	QList<int>						m_PoppedIndexList;

	QList<EntityTreeItemData *>		m_ArrayItemList;

public:
	EntityUndoCmd_PackToArray(ProjectItemData &entityItemRef, QList<EntityTreeItemData *> packItemList, QString sArrayName, int iArrayFolderRow, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PackToArray();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_ReplaceItems : public QUndoCommand
{
	ProjectItemData &				m_ProjItemRef;
	QList<EntityTreeItemData *>		m_ReplaceItemList;

	QList<QUuid>					m_OriginalItemUuidList;

public:
	EntityUndoCmd_ReplaceItems(ProjectItemData &projItemRef, QList<EntityTreeItemData *> replaceItemList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_ReplaceItems();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PasteKeyFrames : public QUndoCommand
{
	EntityDopeSheetScene &							m_DopeSheetSceneRef;
	QList<QPair<EntityTreeItemData *, QJsonArray>>	m_PasteKeyFramesPairList;
	int												m_iStartFrameIndex;

	QList<QPair<EntityTreeItemData *, QJsonArray>>	m_PoppedKeyFramesPairList; // Keyframes that were overwritten

public:
	EntityUndoCmd_PasteKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, QList<QPair<EntityTreeItemData *, QJsonArray>> pasteKeyFramesPairList, int iStartFrameIndex, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PasteKeyFrames();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PopKeyFrames : public QUndoCommand
{
	EntityDopeSheetScene &					m_DopeSheetSceneRef;
	QJsonObject								m_KeyFramesObject;

public:
	EntityUndoCmd_PopKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, const QJsonObject &KeyFrameObj, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PopKeyFrames();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_NudgeSelectedKeyFrames : public QUndoCommand
{
	EntityDopeSheetScene &					m_DopeSheetSceneRef;
	int										m_iFrameOffset;

	// Store the original/old key frame data, before 'nudge' takes place
	QMap<KeyFrameKey, QJsonValue>			m_Prop_SelectedDataMap;
	QMap<KeyFrameKey, TweenJsonValues>		m_Tween_SelectedDataMap;
	QMap<KeyFrameKey, double>				m_TweenKnobs_SelectedDataMap;		// Only stored when a Tween's knob is selected, and not the tween keyframe itself

	// Old key frame data that will be overwritten by the 'nudge' operation
	QMap<KeyFrameKey, QJsonValue>			m_Prop_OverwrittenDataMap;
	QMap<KeyFrameKey, TweenJsonValues>		m_Tween_OverwrittenDataMap;

public:
	EntityUndoCmd_NudgeSelectedKeyFrames(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameOffset, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_NudgeSelectedKeyFrames();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_ConvertToTween : public QUndoCommand
{
	EntityDopeSheetScene &					m_DopeSheetSceneRef;
	QList<ContextTweenData>					m_ContextTweenDataList;

public:
	EntityUndoCmd_ConvertToTween(EntityDopeSheetScene &entityDopeSheetSceneRef, QList<ContextTweenData> contextTweenDataList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_ConvertToTween();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_BreakTween : public QUndoCommand
{
	EntityDopeSheetScene &				m_DopeSheetSceneRef;
	QList<ContextTweenData>				m_BreakTweenDataList;

	QList<QJsonValue>					m_TweenFuncValueList;

public:
	EntityUndoCmd_BreakTween(EntityDopeSheetScene &entityDopeSheetSceneRef, QList<ContextTweenData> breakTweenDataList, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_BreakTween();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_AddCallback : public QUndoCommand
{
	EntityDopeSheetScene &					m_DopeSheetSceneRef;
	int										m_iFrameIndex;
	QString									m_sCallback;

public:
	EntityUndoCmd_AddCallback(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameIndex, QString sCallback, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AddCallback();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_RemoveCallback : public QUndoCommand
{
	EntityDopeSheetScene &					m_DopeSheetSceneRef;
	int										m_iFrameIndex;
	QString									m_sCallback;

public:
	EntityUndoCmd_RemoveCallback(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameIndex, QString sCallback, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_RemoveCallback();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_RenameCallback : public QUndoCommand
{
	EntityDopeSheetScene &m_DopeSheetSceneRef;
	int										m_iFrameIndex;
	QString									m_sOldCallback;
	QString									m_sNewCallback;

public:
	EntityUndoCmd_RenameCallback(EntityDopeSheetScene &entityDopeSheetSceneRef, int iFrameIndex, QString sOldCallback, QString sNewCallback, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_RenameCallback();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_FramesPerSecond : public QUndoCommand
{
	EntityModel &							m_EntityModelRef;
	int										m_iNewFPS;
	int										m_iOldFPS;

public:
	EntityUndoCmd_FramesPerSecond(EntityModel &entityModelRef, int iNewFPS, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_FramesPerSecond();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_AutoInitialize : public QUndoCommand
{
	EntityModel &							m_EntityModelRef;
	bool									m_bNewValue;
	bool									m_bOldValue;

public:
	EntityUndoCmd_AutoInitialize(EntityModel &entityModelRef, bool bNewValue, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_AutoInitialize();

	virtual void redo() override;
	virtual void undo() override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EntityUndoCmd_PropertyModified : public PropertiesUndoCmd
{
	PropertiesType					m_ePropTypeModified;

	QList<EntityTreeItemData *>		m_EntityTreeItemDataList;

	int								m_iFrameIndex;

	// Only used if this UndoCmd is checking the toggle of this item - used to restore the old value(s) when undo() invoked, or override with extrapolated value
	QList<QJsonValue>				m_OverridePropertyValueList;
	QList<TweenJsonValues>			m_OverrideTweenDataList;

public:
	EntityUndoCmd_PropertyModified(PropertiesTreeModel *pModel, const QModelIndex &index, const QVariant &newData, QUndoCommand *pParent = nullptr);
	virtual ~EntityUndoCmd_PropertyModified();

protected:
	virtual void OnRedo() override;
	virtual void OnUndo() override;

	void UpdateEntityModel(bool bIsRedo);
};

#endif // ENTITYUNDOCMDS_H
