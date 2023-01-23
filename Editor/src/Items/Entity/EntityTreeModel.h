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
#include "Shared/TreeModel/ITreeModel.h"

#include <QObject>
#include <QJsonArray>

class EntityModel;

class EntityTreeItemData : public TreeModelItemData
{
	Q_OBJECT

	QUuid												m_Uuid;
	QUuid												m_ItemUuid;
	PropertiesTreeModel									m_PropertiesTreeModel;
	bool												m_bIsSelected;

public:
	EntityTreeItemData(ProjectItemData &entityItemDataRef, QString sCodeName, HyGuiItemType eItemType, QUuid uuidOfItem);
	EntityTreeItemData(ProjectItemData &entityItemDataRef, QJsonObject initObj);
	virtual ~EntityTreeItemData();

	QString GetCodeName() const;
	QUuid GetThisUuid() const;
	QUuid GetItemUuid() const;
	PropertiesTreeModel &GetPropertiesModel();

	void SetSelected(bool bIsSelected);

	void InsertJsonInfo(QJsonObject &childObjRef);

protected:
	void InitalizePropertiesTree();
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
	
	TreeModelItem *GetEntityTreeItem() const;
	EntityTreeItemData *GetEntityTreeItemData() const;
	void GetTreeItemData(QList<EntityTreeItemData *> &childListOut, QList<EntityTreeItemData *> &shapeListOut) const;
	EntityTreeItemData *FindTreeItemData(QUuid uuid) const;

	bool IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const;
	EntityTreeItemData *Cmd_InsertNewChild(ProjectItemData *pProjItem, QString sCodeNamePrefix, int iRow = -1);
	EntityTreeItemData *Cmd_InsertNewChild(QJsonObject initObj, int iRow = -1);
	bool Cmd_InsertChild(EntityTreeItemData *pItem, int iRow);
	int32 Cmd_PopChild(EntityTreeItemData *pItem);

	QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual void OnTreeModelItemRemoved(TreeModelItem *pTreeItem) override;

protected:
	QString GenerateCodeName(QString sDesiredName) const;
};

#endif // ENTITYTREEMODEL_H
