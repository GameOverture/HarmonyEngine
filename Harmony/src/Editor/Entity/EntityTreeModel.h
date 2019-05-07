/**************************************************************************
*	EntityTreeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYTREEMODEL_H
#define ENTITYTREEMODEL_H

#include "ProjectItem.h"
#include "GlobalWidgetMappers.h"
#include "PropertiesTreeModel.h"
#include "Shared/TreeModel/ITreeModel.h"

#include <QAbstractItemModel>

class EntityModel;

class EntityTreeModel : public ITreeModel
{
	Q_OBJECT

	EntityModel *				m_pEntityModel;

public:
	explicit EntityTreeModel(EntityModel *pEntityModel, QObject *parent = nullptr);
	virtual ~EntityTreeModel();

	bool IsItemValid(ExplorerItem *pItem, bool bShowDialogsOnFail) const;

	bool AddChildItem(ExplorerItem *pItem);
	bool RemoveChild(ExplorerItem *pItem);

	QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // ENTITYTREEMODEL_H
