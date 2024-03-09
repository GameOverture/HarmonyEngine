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
	explicit PropertiesTreeMultiModel(ProjectItemData &ownerRef, int iStateIndex, QVariant subState, const QList<PropertiesTreeModel *> &multiModelListRef, QObject *pParent = nullptr);
	virtual ~PropertiesTreeMultiModel();
	
	virtual void SetToggle(const QModelIndex &indexRef, bool bToggleOn) override;

	virtual void SetPropertyValue(QString sCategoryName, QString sPropertyName, const QVariant &valueRef, bool bIsProceduralObj) override;

	virtual bool setData(const QModelIndex &indexRef, const QVariant &valueRef, int iRole = Qt::EditRole) override;
	virtual QVariant data(const QModelIndex &indexRef, int iRole = Qt::DisplayRole) const override;
	
	virtual Qt::ItemFlags flags(const QModelIndex& indexRef) const override;

private:
	using PropertiesTreeModel::AppendCategory;
	using PropertiesTreeModel::AppendProperty;

	using PropertiesTreeModel::SerializeJson;
	using PropertiesTreeModel::DeserializeJson;
};

#endif // PROPERTIESTREEMULTIMODEL_H
