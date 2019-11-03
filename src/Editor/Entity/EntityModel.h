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
#include "EntityNodeTreeModel.h"
#include "GlobalWidgetMappers.h"
#include "ProjectItemMimeData.h"

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

	EntityNodeTreeModel			m_TreeModel;
	QList<ProjectItem *>		m_PrimitiveList;

public:
	EntityModel(ProjectItem &itemRef, QJsonArray stateArray);
	virtual ~EntityModel();

	EntityNodeTreeModel &GetChildrenModel();
	PropertiesTreeModel *GetPropertiesModel(int iStateIndex, ExplorerItem *pItem);

	void AddNewChildren(const ProjectItemMimeData *pMimeData);
	bool RemoveChild(ExplorerItem *pItem);

	const QList<ProjectItem *> &GetPrimitiveList();
	ProjectItem *CreateNewPrimitive();

	virtual QJsonObject GetStateJson(uint32 uiIndex) const override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
};

#endif // ENTITYMODEL_H
