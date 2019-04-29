/**************************************************************************
*	ITreeModelItem.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IMODELTREEITEM_H
#define IMODELTREEITEM_H

#include "Global.h"

class TreeModelItem : public QObject
{
	Q_OBJECT

protected:
	TreeModelItem *            m_pParentItem;
	QList<TreeModelItem *>     m_ChildList;

public:
	explicit TreeModelItem();
	virtual ~TreeModelItem();

	TreeModelItem *GetParent();
	TreeModelItem *GetChild(int iRow) const;
	QList<TreeModelItem *> GetChildren(bool bRecusively);

	void AppendChild(TreeModelItem *pChild);
	void InsertChild(int iIndex, TreeModelItem *pChild);
	void RemoveChild(int iIndex);

	int GetNumChildren() const;
	int GetRow() const;

	virtual QString GetToolTip() const = 0;
};

#endif // IMODELTREEITEM_H
