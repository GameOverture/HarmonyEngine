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
#include "EntityTreeModel.h"
#include "GlobalWidgetMappers.h"

#include <QObject>
#include <QJsonArray>

class EntityStateData : public IStateData
{
	QMap<EntityTreeItem *, PropertiesTreeModel *>    m_PropertiesMap;

public:
	EntityStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
	virtual ~EntityStateData();

	PropertiesTreeModel *GetPropertiesModel(EntityTreeItem *pTreeItem);
	void GetStateInfo(QJsonObject &stateObjOut);

	void Refresh();

	virtual int AddFrame(AtlasFrame *pFrame) override;
	virtual void RelinquishFrame(AtlasFrame *pFrame) override;

private:
	PropertiesTreeModel *AllocNewPropertiesModel(ProjectItem &entityItemRef, QVariant &subState, ProjectItem *pItemToAdd);
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityModel : public IModel
{
	Q_OBJECT

	EntityTreeModel         m_TreeModel;



public:
	EntityModel(ProjectItem &itemRef, QJsonArray stateArray);
	virtual ~EntityModel();

	//EntityTreeModel &GetTreeModel();
	PropertiesTreeModel *GetPropertiesModel(int iStateIndex, EntityTreeItem *pTreeItem);

	bool IsChildAddable(ProjectItem *pItem);
	bool AddNewChild(ProjectItem *pItem);
	bool InsertTreeItem(int iRow, EntityTreeItem *pItem, EntityTreeItem *pParentItem);
	bool RemoveTreeItems(int iRow, int iCount, EntityTreeItem *pParentItem);

	void SetWidget(QTreeView *pTreeView);

	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // ENTITYMODEL_H
