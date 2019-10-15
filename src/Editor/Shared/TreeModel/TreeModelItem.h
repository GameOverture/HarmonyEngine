/**************************************************************************
*	TreeModelItem.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TREEMODELITEM_H
#define TREEMODELITEM_H

#include "Global.h"

class TreeModelItem
{
	TreeModelItem *				m_pParentItem;
	QList<TreeModelItem *>		m_ChildList;

	QVector<QVariant>			m_DataVec;

public:
	explicit TreeModelItem(const QVector<QVariant> &data, TreeModelItem *pParent = nullptr);
	~TreeModelItem();

	TreeModelItem *parent();
	int childNumber() const;

	TreeModelItem *child(int iIndex);
	QVector<TreeModelItem *>GetChildren();
	int childCount() const;
	int columnCount() const;

	QVariant data(int iColumn) const;
	bool setData(int iColumn, const QVariant &valueRef);

	bool insertChildren(int iPosition, int iCount, int iColumns);
	bool insertColumns(int iPosition, int iColumns);
	
	bool isRemoveValid(int iPosition, int iCount);
	bool removeChildren(int iPosition, int iCount);
	bool removeColumns(int iPosition, int iColumns);
};

#endif // TREEMODELITEM_H
