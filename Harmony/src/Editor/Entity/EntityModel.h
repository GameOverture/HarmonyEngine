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
	QMap<ExplorerItem *, PropertiesTreeModel *>    m_PropertiesMap;

public:
	EntityStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
	virtual ~EntityStateData();

	PropertiesTreeModel *GetPropertiesModel(ExplorerItem *pItem);
	void GetStateInfo(QJsonObject &stateObjOut);

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

	EntityTreeModel			m_TreeModel;
	QList<ProjectItem *>	m_PrimitiveList;

public:
	EntityModel(ProjectItem &itemRef, QJsonArray stateArray);
	virtual ~EntityModel();

	EntityTreeModel &GetChildrenModel();
	PropertiesTreeModel *GetPropertiesModel(int iStateIndex, ExplorerItem *pItem);

	bool AddNewChild(ExplorerItem *pItem);
	bool RemoveChild(ExplorerItem *pItem);

	const QList<ProjectItem *> &GetPrimitiveList();
	ProjectItem *CreateNewPrimitive();

	void SetWidget(QTreeView *pTreeView);

	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // ENTITYMODEL_H
