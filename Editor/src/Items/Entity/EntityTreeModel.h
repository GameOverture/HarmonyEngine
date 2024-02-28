/**************************************************************************
*	EntityTreeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYTREEMODEL_H
#define ENTITYTREEMODEL_H

#include "ProjectItemData.h"
#include "PropertiesTreeModel.h"
#include "IAssetItemData.h"
#include "Shared/TreeModel/ITreeModel.h"

#include <QObject>
#include <QJsonArray>

class EntityModel;

class PropertiesUndoCmd;
class EntityPropertiesTreeModel : public PropertiesTreeModel
{
public:
	explicit EntityPropertiesTreeModel(ProjectItemData &ownerRef, int iStateIndex, QVariant subState, QObject *pParent = nullptr);
	virtual ~EntityPropertiesTreeModel();

	virtual PropertiesUndoCmd *AllocateUndoCmd(const QModelIndex &index, const QVariant &newData) override;
};

enum EntityItemType
{
	ENTTYPE_Root,
	ENTTYPE_BvFolder,
	ENTTYPE_Item,
	ENTTYPE_ArrayFolder,
	ENTTYPE_ArrayItem,
};
class EntityTreeItemData : public TreeModelItemData
{
	Q_OBJECT

	EntityModel &										m_EntityModelRef;
	EntityItemType										m_eEntType;

	EntityPropertiesTreeModel *							m_pPropertiesModel;

	QString												m_sPromotedEntityType;
	bool												m_bIsForwardDeclared;

	QUuid												m_ReferencedItemUuid;

	bool												m_bIsSelected;
	
	bool												m_bReallocateDrawItem; // This is set when the draw item needs any of its default values set that doesn't have a corresponding key frame

public:
	EntityTreeItemData(EntityModel &entityModelRef, bool bIsForwardDeclared, QString sCodeName, ItemType eItemType, EntityItemType eEntType, QUuid uuidOfReferencedItem, QUuid uuidOfThis);
	EntityTreeItemData(EntityModel &entityModelRef, bool bIsForwardDeclared, QJsonObject descObj, bool bIsArrayItem);
	virtual ~EntityTreeItemData();

	EntityItemType GetEntType() const;
	QString GetHyNodeTypeName(bool bIncludeNamespace) const;
	QString GetCodeName() const;

	const QUuid &GetThisUuid() const;
	const QUuid &GetReferencedItemUuid() const;

	bool IsPromotedEntity() const;
	bool IsForwardDeclared() const;

	EntityModel &GetEntityModel() const;
	EntityPropertiesTreeModel &GetPropertiesModel() const;

	bool IsSelected() const;
	void SetSelected(bool bIsSelected);

	bool IsReallocateDrawItem() const;
	void SetReallocateDrawItem(bool bReallocateDrawItem);

	int GetArrayIndex() const; // Returns -1 when not an array item

	void InsertJsonInfo_Desc(QJsonObject &childObjRef);

protected:
	void InitalizePropertyModel();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityTreeModel : public ITreeModel
{
	friend class EntityModel;

	Q_OBJECT

	EntityModel &										m_ModelRef;

	enum ColumnType
	{
		COLUMN_CodeName = 0,
		COLUMN_ItemPath,

		NUMCOLUMNS
	};

public:
	explicit EntityTreeModel(EntityModel &modelRef, QString sEntityCodeName, QJsonObject fileMetaObj, QObject *pParent = nullptr);
	virtual ~EntityTreeModel();
	
	TreeModelItem *GetRootTreeItem() const;
	EntityTreeItemData *GetRootTreeItemData() const;

	TreeModelItem *GetBvFolderTreeItem() const;
	EntityTreeItemData *GetBvFolderTreeItemData() const;

	TreeModelItem *GetArrayFolderTreeItem(EntityTreeItemData *pArrayItem) const;
	EntityTreeItemData *GetArrayFolderTreeItemData(EntityTreeItemData *pArrayItem) const;

	void GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &shapeListOut) const;
	EntityTreeItemData *FindTreeItemData(QUuid uuid) const;

	bool IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const;

private: // These functions should only be called by EntityModel's Cmd_ functions
	EntityTreeItemData *Cmd_AllocChildTreeItem(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_AllocAssetTreeItem(IAssetItemData *pAssetItem, QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_AllocExistingTreeItem(QJsonObject descObj, bool bIsArrayItem, int iRow = -1);
	EntityTreeItemData *Cmd_AllocShapeTreeItem(EditorShape eShape, QString sData, bool bIsPrimitive, QString sCodeNamePrefix, int iRow = -1);
	bool Cmd_ReaddChild(EntityTreeItemData *pItem, int iRow);
	int32 Cmd_PopChild(EntityTreeItemData *pItem);

public:
	QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual void OnTreeModelItemRemoved(TreeModelItem *pTreeItem) override;

	QString GenerateCodeName(QString sDesiredName) const;

protected:
	bool ShouldForwardDeclare(ProjectItemData *pProjItem);
	bool ShouldForwardDeclare(const QJsonObject &initObj);

	bool FindOrCreateArrayFolder(TreeModelItem *&pParentTreeItemOut, QString sCodeName, ItemType eItemType, int iRowToCreateAt); // 'pParentTreeItemOut' must point to either Root or BvFolder, it will be reassigned to the ArrayFolder that is either found (return true), or newly created (return false)
};

#endif // ENTITYTREEMODEL_H
