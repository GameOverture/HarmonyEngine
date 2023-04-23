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
	bool												m_bIsForwardDeclared;
	QUuid												m_ItemUuid;

	bool												m_bIsSelected;

public:
	EntityTreeItemData(EntityModel &entityModelRef, bool bIsForwardDeclared, QString sCodeName, ItemType eItemType, EntityItemType eEntType, QUuid uuidOfItem, QUuid uuidOfThis);
	EntityTreeItemData(EntityModel &entityModelRef, bool bIsForwardDeclared, QJsonObject descObj, QJsonArray propArray, bool bIsArrayItem);
	virtual ~EntityTreeItemData();

	EntityItemType GetEntType() const;
	QString GetCodeName() const;
	const QUuid &GetThisUuid() const;
	const QUuid &GetItemUuid() const;
	bool IsForwardDeclared() const;

	PropertiesTreeModel &GetPropertiesModel(int iStateIndex);

	bool IsSelected() const;
	void SetSelected(bool bIsSelected);

	void InsertJsonInfo_Desc(QJsonObject &childObjRef);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityTreeModel : public ITreeModel
{
	Q_OBJECT

	EntityModel &										m_ModelRef;

	enum ColumnType
	{
		COLUMN_CodeName = 0,
		COLUMN_ItemPath,

		NUMCOLUMNS
	};

public:
	explicit EntityTreeModel(EntityModel &modelRef, QString sEntityCodeName, QUuid uuidOfEntity, QObject *pParent = nullptr);
	virtual ~EntityTreeModel();
	
	TreeModelItem *GetRootTreeItem() const;
	EntityTreeItemData *GetRootTreeItemData() const;

	TreeModelItem *GetBvFolderTreeItem() const;
	EntityTreeItemData *GetBvFolderTreeItemData() const;

	EntityTreeItemData *GetArrayFolderTreeItemData(EntityTreeItemData *pArrayItem) const;

	void GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &shapeListOut) const;
	EntityTreeItemData *FindTreeItemData(QUuid uuid) const;

	bool IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const;
	EntityTreeItemData *Cmd_InsertNewChild(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_InsertNewAsset(IAssetItemData *pAssetItem, QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_InsertNewItem(QJsonObject descObj, QJsonArray propArray, bool bIsArrayItem, int iRow = -1);
	EntityTreeItemData *Cmd_InsertNewShape(EditorShape eShape, QString sData, bool bIsPrimitive, QString sCodeNamePrefix, int iRow = -1);
	bool Cmd_ReaddChild(EntityTreeItemData *pItem, int iRow);
	int32 Cmd_PopChild(EntityTreeItemData *pItem);

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
