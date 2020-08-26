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

	TreeModelItem *GetParent();
	int GetIndex() const;

	TreeModelItem *GetChild(int iIndex);
	QVector<TreeModelItem *>GetChildren();
	int GetNumChildren() const;
	int columnCount() const;

	QVariant data(int iColumn) const;
	bool SetData(int iColumn, const QVariant &valueRef);

	bool InsertChildren(int iPosition, int iCount, int iColumns);
	bool InsertColumns(int iPosition, int iColumns);
	
	bool IsRemoveValid(int iPosition, int iCount);
	bool DeleteChildren(int iPosition, int iCount);
	bool RemoveColumns(int iPosition, int iColumns);

	bool MoveChild(int iPosition, TreeModelItem *pNewParent, int iNewPostion);
};

#endif // TREEMODELITEM_H
