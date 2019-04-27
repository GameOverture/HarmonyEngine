/**************************************************************************
*	IModelTreeItem.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IMODELTREEITEM_H
#define IMODELTREEITEM_H

#include <QAbstractItemModel>

class IModelTreeItem : public QObject
{
	Q_OBJECT

protected:
	IModelTreeItem *            m_pParentItem;
	QList<IModelTreeItem *>     m_ChildList;

public:
	explicit IModelTreeItem();
	virtual ~IModelTreeItem();

	IModelTreeItem *GetParent();
	IModelTreeItem *GetChild(int iRow) const;

	void AppendChild(IModelTreeItem *pChild);
	void InsertChild(int iIndex, IModelTreeItem *pChild);
	void RemoveChild(int iIndex);

	int GetNumChildren() const;
	int GetRow() const;

	virtual QString GetToolTip() const = 0;
};

#endif // IMODELTREEITEM_H
