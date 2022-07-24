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
#include "ProjectItemMimeData.h"
#include "PropertiesTreeModel.h"
#include "Shared/TreeModel/ITreeModel.h"

#include <QObject>
#include <QJsonArray>

class EntityModel;

class EntityNodeItemData : public TreeModelItemData
{
	Q_OBJECT

	ProjectItemData *	m_pProjItem;
	
public:
	EntityNodeItemData(ProjectItemData *pProjItem);
	virtual ~EntityNodeItemData();

	ProjectItemData *GetProjItem();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityNodeTreeModel : public ITreeModel
{
	Q_OBJECT

	EntityModel *										m_pEntityModel;
	QList<EntityNodeItemData *>							m_NodeList;

public:
	explicit EntityNodeTreeModel(EntityModel *pEntityModel, QObject *parent = nullptr);
	virtual ~EntityNodeTreeModel();
	
	EntityNodeItemData *FindEntityNodeItem(ProjectItemData *pItem);

	bool IsItemValid(TreeModelItemData *pItem, bool bShowDialogsOnFail) const;
	bool InsertNewChild(ProjectItemData *pProjItem, TreeModelItem *pParentTreeItem = nullptr, int iRow = -1);
	bool RemoveChild(EntityNodeItemData *pItem);

	QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual void OnTreeModelItemRemoved(TreeModelItem *pTreeItem) override;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityStateData : public IStateData
{
	QMap<ExplorerItemData *, PropertiesTreeModel *>			m_PropertiesMap;

public:
	EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~EntityStateData();

	PropertiesTreeModel *GetPropertiesModel(ExplorerItemData *pItem);

	virtual QVariant OnLinkAsset(AssetItemData *pAsset) override;
	virtual void OnUnlinkAsset(AssetItemData *pAsset) override;

private:
	PropertiesTreeModel *AllocNewPropertiesModel(ProjectItemData &entityItemRef, QVariant &subState, ExplorerItemData *pItemToAdd);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
	Q_OBJECT

	EntityNodeTreeModel										m_TreeModel;

public:
	EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~EntityModel();

	EntityNodeTreeModel &GetNodeTreeModel();
	PropertiesTreeModel *GetPropertiesModel(int iStateIndex, ExplorerItemData *pItem);

	void AddNewChildren(QList<TreeModelItemData *> itemList);
	bool RemoveChild(ProjectItemData *pItem);

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual void InsertStateSpecificData(uint32 uiIndex, FileDataPair &stateFileDataOut) const override;
	virtual QList<AssetItemData *> GetAssets(AssetType eAssetType) const override;
};

#endif // ENTITYMODEL_H
