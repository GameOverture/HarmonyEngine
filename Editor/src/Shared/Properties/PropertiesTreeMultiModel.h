/**************************************************************************
*	PropertiesTreeMultiModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
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
	explicit PropertiesTreeMultiModel(ProjectItemData &ownerRef, int iStateIndex, QVariant subState, const QList<PropertiesTreeModel *> &multiModelListRef, QList<QJsonObject> multiPropsObjList, QObject *pParent = nullptr);
	virtual ~PropertiesTreeMultiModel();

private:
	using PropertiesTreeModel::AppendCategory;
	using PropertiesTreeModel::AppendProperty;

	using PropertiesTreeModel::SerializeJson;
	using PropertiesTreeModel::DeserializeJson;
};

#endif // PROPERTIESTREEMULTIMODEL_H
