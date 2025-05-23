/**************************************************************************
*	SpineSkinTreeModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef SPINESKINTREEMODEL_H
#define SPINESKINTREEMODEL_H

#include "Global.h"

class SpineStateData;

class SpineSkinTreeItem
{
	QString								m_data;
	QList<SpineSkinTreeItem *>			m_children;
	SpineSkinTreeItem *					m_parent;
	Qt::CheckState						m_checkState;

public:
	explicit SpineSkinTreeItem(const QString &data, SpineSkinTreeItem *parent = nullptr);
	~SpineSkinTreeItem();

	SpineSkinTreeItem *appendChild(const QString &data);

	SpineSkinTreeItem *child(int row) const;
	int childCount() const;
	int row() const;
	SpineSkinTreeItem *parent() const;
	QString data() const;

	Qt::CheckState checkState() const;
	void setCheckState(Qt::CheckState state);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SpineSkinTreeModel : public QAbstractItemModel
{
	Q_OBJECT

	SpineStateData &					m_StateDataRef;

	SpineSkinTreeItem *					m_pRoot;
	QStringList							m_sEnabledSkinList;

public:
	SpineSkinTreeModel(SpineStateData &stateDataRef, QStringList sSkinPathsList, QObject *parent = nullptr);
	virtual ~SpineSkinTreeModel();

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	virtual QModelIndex parent(const QModelIndex &index) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

	QStringList GetEnabledSkinList() const;
	void Cmd_SetEnabledSkins(const QStringList &sSkinPathsList);

protected:
	void UpdateChildrenCheckState(SpineSkinTreeItem *pItem, Qt::CheckState eState);
	void UpdateParentCheckState(SpineSkinTreeItem *pItem);

//Q_SIGNALS:
//	void CheckStatesUpdated();
};

#endif // SPINESKINTREEMODEL_H
