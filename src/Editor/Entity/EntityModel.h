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
#include "ProjectItem.h"
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

	EntityModel *									m_pEntityModel;

	TreeModelItem *									m_pSelfNode;

public:
	explicit EntityNodeTreeModel(EntityModel *pEntityModel, QObject *parent = nullptr);
	virtual ~EntityNodeTreeModel();

	bool IsItemValid(ExplorerItem *pItem, bool bShowDialogsOnFail) const;

	bool AddChildItem(ExplorerItem *pItem);
	bool RemoveChild(ExplorerItem *pItem);

	QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	virtual void OnTreeModelItemRemoved(TreeModelItem *pTreeItem) override;
};

class EntityStateData : public IStateData
{
	QMap<ExplorerItem *, PropertiesTreeModel *>		m_PropertiesMap;

public:
	EntityStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData);
	virtual ~EntityStateData();

	PropertiesTreeModel *GetPropertiesModel(ExplorerItem *pItem);

	void Refresh();

	virtual int AddFrame(AtlasFrame *pFrame) override;
	virtual void RelinquishFrame(AtlasFrame *pFrame) override;

private:
	PropertiesTreeModel *AllocNewPropertiesModel(ProjectItem &entityItemRef, QVariant &subState, ExplorerItem *pItemToAdd);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
	Q_OBJECT

	EntityNodeTreeModel			m_TreeModel;
	QList<ProjectItem *>		m_PrimitiveList;

	class DependencyLookup
	{
		QMap<QString, ProjectItem *>		m_ItemMap;

	public:
		void AddDependency(ProjectItem *pItem);
		bool RemoveDependency(ProjectItem *pItem);  // Returns true if no remaining duplicates exist
		ProjectItem *FindByGuid(QUuid guid);
	};
	DependencyLookup								m_Dependencies;

public:
	EntityModel(ProjectItem &itemRef, const FileDataPair &itemFileDataRef);
	virtual ~EntityModel();

	EntityNodeTreeModel &GetChildrenModel();
	PropertiesTreeModel *GetPropertiesModel(int iStateIndex, ExplorerItem *pItem);

	void AddNewChildren(const ProjectItemMimeData *pMimeData);
	bool RemoveChild(ExplorerItem *pItem);

	const QList<ProjectItem *> &GetPrimitiveList();
	ProjectItem *CreateNewPrimitive();

	virtual bool OnSave() override { return true; }
	virtual void InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) override;
	virtual FileDataPair GetStateFileData(uint32 uiIndex) const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
};

#endif // ENTITYMODEL_H
