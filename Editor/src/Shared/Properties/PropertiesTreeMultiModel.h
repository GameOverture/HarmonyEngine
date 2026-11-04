/**************************************************************************
*	PropertiesTreeMultiModel.h
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2023 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
*************************************************************************/
#ifndef PROPERTIESTREEMULTIMODEL_H
#define PROPERTIESTREEMULTIMODEL_H

#include "Global.h"
#include "PropertiesTreeModel.h"

class PropertiesTreeMultiModel : public PropertiesTreeModel
{
	Q_OBJECT

	QList<PropertiesTreeModel *>		m_MultiModelList;

public:
	explicit PropertiesTreeMultiModel(ProjectItemData *pProjItemData, QUndoStack *pUndoStack, int iStateIndex, QVariant subState, const QList<PropertiesTreeModel *> &multiModelListRef, QObject *pParent = nullptr);
	virtual ~PropertiesTreeMultiModel();

	void Setup(const QList<PropertiesTreeModel *> &multiModelListRef);
	QList<PropertiesTreeModel *> GetMultiModelList();

private:
	using PropertiesTreeModel::InsertCategory;
	using PropertiesTreeModel::AppendProperty;

	using PropertiesTreeModel::SerializeJson;
	using PropertiesTreeModel::DeserializeJson;
};

#endif // PROPERTIESTREEMULTIMODEL_H
