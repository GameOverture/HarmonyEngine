/**************************************************************************
*	EntityModel.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
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

	EntityTreeModel &GetTreeModel();

	PropertiesTreeModel *GetPropertiesModel(int iStateIndex, EntityTreeItem *pTreeItem);

	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // ENTITYMODEL_H
