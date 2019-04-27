/**************************************************************************
*	PropertiesTreeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef PROPERTIESTREEMODEL_H
#define PROPERTIESTREEMODEL_H

#include "Global.h"
#include "PropertiesTreeView.h"
#include "PropertiesTreeItem.h"
#include "ProjectItem.h"
#include "Shared/TreeModel/ITreeModel.h"

class PropertiesTreeModel : public ITreeModel
{
	Q_OBJECT

	ProjectItem &								m_ItemRef;
	const int									m_iSTATE_INDEX;
	const QVariant								m_iSUBSTATE;

	QList<PropertiesTreeItem *>					m_CategoryList;

public:
	explicit PropertiesTreeModel(ProjectItem &itemRef, int iStateIndex, QVariant subState, QObject *parent = nullptr);
	virtual ~PropertiesTreeModel();

	ProjectItem &GetItem();
	QVariant GetValue(QString sUniquePropertyName) const;

	bool AppendCategory(QString sName, QColor color, QVariant commonDelegateBuilder = QVariant(), bool bCheckable = false, bool bStartChecked = false, QString sToolTip = "");
	bool AppendProperty(QString sCategoryName, QString sName, PropertiesDef defintion, QString sToolTip, bool bReadOnly = false);

	void RefreshProperties();

	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	//bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int iRole = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;

	// This creates an Undo command to be pushed on the UndoStack
	virtual bool setData(const QModelIndex &index, const QVariant &value, int iRole = Qt::EditRole) override;

private:
	PropertiesTreeItem *ValidateCategory(QString sCategoryName, QString sUniquePropertyName);
};

#endif // PROPERTIESTREEMODEL_H
