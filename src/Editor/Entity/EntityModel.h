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

class EntityNodeTreeModel : public ITreeModel
{
	Q_OBJECT

	EntityModel *										m_pEntityModel;

public:
	explicit EntityNodeTreeModel(EntityModel *pEntityModel, QObject *parent = nullptr);
	virtual ~EntityNodeTreeModel();

	bool IsItemValid(ExplorerItemData *pItem, bool bShowDialogsOnFail) const;
	bool IsItemValid(ProjectItemData *pItem, bool bShowDialogsOnFail) const;

	bool InsertNewChild(ProjectItemData *pNewItem, TreeModelItem *pParentTreeItem = nullptr, int iRow = -1);
	bool RemoveChild(ProjectItemData *pItem);

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

	void Refresh();

	virtual int AddFrame(AtlasFrame *pFrame) override;
	virtual void RelinquishFrame(AtlasFrame *pFrame) override;

private:
	PropertiesTreeModel *AllocNewPropertiesModel(ProjectItemData &entityItemRef, QVariant &subState, ExplorerItemData *pItemToAdd);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
	Q_OBJECT

	EntityNodeTreeModel									m_TreeModel;
	QList<ProjectItemData *>								m_PrimitiveList;

	class DependencyLookup
	{
		QMap<QString, ProjectItemData *>					m_ItemMap;

	public:
		void AddDependency(ProjectItemData *pItem)			{ m_ItemMap[pItem->GetModel()->GetUuid().toString(QUuid::WithoutBraces)] = pItem; }
		void RemoveDependency(ProjectItemData *pItem)		{ m_ItemMap.remove(pItem->GetModel()->GetUuid().toString(QUuid::WithoutBraces)); }
		ProjectItemData *FindByGuid(QUuid uuid)				{ return m_ItemMap.contains(uuid.toString(QUuid::WithoutBraces)) ? m_ItemMap[uuid.toString(QUuid::WithoutBraces)] : nullptr; }
	};
	DependencyLookup									m_Dependencies;

public:
	EntityModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~EntityModel();

	EntityNodeTreeModel &GetChildrenModel();
	PropertiesTreeModel *GetPropertiesModel(int iStateIndex, ExplorerItemData *pItem);

	void AddNewChildren(QList<ProjectItemData *> itemList);
	bool RemoveChild(ProjectItemData *pItem);

	const QList<ProjectItemData *> &GetPrimitiveList();
	ProjectItemData *CreateNewPrimitive();

	virtual bool OnPrepSave() override;
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual FileDataPair GetStateFileData(uint32 uiIndex) const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
};

#endif // ENTITYMODEL_H
