/**************************************************************************
*	EntityModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYMODEL_H
#define ENTITYMODEL_H

#include "IModel.h"
#include "ProjectItemData.h"
#include "GlobalWidgetMappers.h"
#include "EntityTreeModel.h"
#include "ProjectItemMimeData.h"

#include <QObject>
#include <QJsonArray>

#define ENTCOLOR_TransformBv HyColor::Blue.Lighten()
#define ENTCOLOR_Primitive HyColor::DarkMagenta
#define ENTCOLOR_Shape HyColor::Cyan
#define ENTCOLOR_Marquee HyColor::Blue.Lighten()
#define ENTCOLOR_Clear HyColor::White

#define ENTCOLORPOINT_Transform HyColor::White, HyColor::Blue.Lighten()
#define ENTCOLORPOINT_TransformRotate HyColor::Blue.Lighten(), HyColor::White
#define ENTCOLORPOINT_Vem HyColor::White, HyColor::Red
#define ENTCOLORPOINT_VemSelected HyColor::White, HyColor::Blue.Lighten()

class EntityStateData : public IStateData
{
	QMap<EntityTreeItemData *, PropertiesTreeModel *>		m_PropertiesMap;

public:
	EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~EntityStateData();

	PropertiesTreeModel *GetPropertiesTreeModel(EntityTreeItemData *pItemData);

	void Cmd_AddItemDataProperties(EntityTreeItemData *pItemData, QJsonObject propObj);
	void Cmd_RemoveItemDataProperties(EntityTreeItemData *pItemData);

	void InitalizePropertyModel(EntityTreeItemData *pItemData, PropertiesTreeModel &propertiesTreeModelRef);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
	Q_OBJECT

	EntityTreeModel										m_TreeModel;

	bool												m_bVertexEditMode;

public:
	EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~EntityModel();

	EntityTreeModel &GetTreeModel();

	// Command Modifiers (Cmd_) - These mutate the internal state and should only be called from this constructor and from UndoCmd's
	QList<EntityTreeItemData *> Cmd_AddNewChildren(QList<ProjectItemData *> projItemList, int iRow);
	QList<EntityTreeItemData *> Cmd_AddNewAssets(QList<IAssetItemData *> assetItemList, int iRow);
	EntityTreeItemData *Cmd_AddNewItem(QJsonObject descObj, QJsonArray propsArray, bool bIsArrayItem, int iRow); // If a newly created ArrayFolder is needed, it'll be placed at 'iRow'. If ArrayFolder already exists, 'iRow' is the row within the ArrayFolder
	EntityTreeItemData *Cmd_AddNewShape(EditorShape eShape, QString sData, bool bIsPrimitive, int iRow);
	QList<EntityTreeItemData *> Cmd_AddNewPasteItems(QJsonObject mimeObject, EntityTreeItemData *pArrayFolder);
	QList<EntityTreeItemData *> Cmd_CreateNewArray(QList<EntityTreeItemData *> itemDataList, QString sArrayName, int iArrayFolderRow); // It is assumed that the items within 'itemDataList' have been removed/popped prior
	void Cmd_SelectionChanged(QList<EntityTreeItemData *> selectedList, QList<EntityTreeItemData *> deselectedList);
	int32 Cmd_RemoveTreeItem(EntityTreeItemData *pItem);
	bool Cmd_ReaddChild(EntityTreeItemData *pNodeItem, int iRow);

	void Cmd_RenameItem(EntityTreeItemData *pItemData, QString sNewName);

	void SetShapeEditDrag(EditorShape eShapeType, bool bAsPrimitive);
	void SetShapeEditVemMode(bool bEnable);
	void ClearShapeEdit();

	QString GenerateCodeName(QString sDesiredName) const;

	virtual void OnPropertyModified(PropertiesTreeModel &propertiesModelRef, QString sCategory, QString sProperty) override;

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;

	void InsertChildAndShapeList(int iStateIndex, FileDataPair &fileDataPairOut) const; // Pass -1 when saving the 'global' common data
};

#endif // ENTITYMODEL_H
