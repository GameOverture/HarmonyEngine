/**************************************************************************
*	IModelTreeItem.h
*
*	Harmony Engine - Designer Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Designer Tool License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IMODELTREEITEM_H
#define IMODELTREEITEM_H

#include <QAbstractItemModel>

class IModelTreeItem
{
protected:
	IModelTreeItem *            m_pParentItem;
	QList<IModelTreeItem *>     m_ChildList;

public:
	explicit IModelTreeItem();
	virtual ~IModelTreeItem();

	IModelTreeItem *GetParent();
	IModelTreeItem *GetChild(int iRow);

	void AppendChild(IModelTreeItem *pChild);
	void InsertChild(int iIndex, IModelTreeItem *pChild);
	void RemoveChild(int iIndex);

	int GetNumChildren() const;
	int GetRow() const;

	virtual QString GetToolTip() const = 0;
};

#endif // IMODELTREEITEM_H
