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

class ITreeModelItem : public QObject
{
	Q_OBJECT

protected:
	ITreeModelItem *            m_pParentItem;
	QList<ITreeModelItem *>     m_ChildList;

public:
	explicit ITreeModelItem();
	virtual ~ITreeModelItem();

	ITreeModelItem *GetParent();
	ITreeModelItem *GetChild(int iRow) const;
	QList<ITreeModelItem *> GetChildren(bool bRecusively);

	void AppendChild(ITreeModelItem *pChild);
	void InsertChild(int iIndex, ITreeModelItem *pChild);
	void RemoveChild(int iIndex);

	int GetNumChildren() const;
	int GetRow() const;

	virtual QString GetToolTip() const = 0;
};

#endif // IMODELTREEITEM_H
