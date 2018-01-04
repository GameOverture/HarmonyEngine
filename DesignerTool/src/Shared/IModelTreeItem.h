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
	explicit IModelTreeItem() : m_pParentItem(nullptr)
	{ }

	virtual ~IModelTreeItem()
	{ }

	IModelTreeItem *GetParent()
	{
		return m_pParentItem;
	}

	IModelTreeItem *GetChild(int iRow)
	{
		if(iRow >= m_ChildList.size() || iRow < 0)
			return nullptr;

		return m_ChildList[iRow];
	}

	void AppendChild(IModelTreeItem *pChild)
	{
		InsertChild(m_ChildList.size(), pChild);
	}

	void InsertChild(int iIndex, IModelTreeItem *pChild)
	{
		if(pChild->m_pParentItem == this)
		{
			m_ChildList.move(m_ChildList.indexOf(pChild), iIndex);
			return;
		}
		else if(pChild->m_pParentItem)
			pChild->m_pParentItem->RemoveChild(pChild->GetRow());

		pChild->m_pParentItem = this;
		m_ChildList.insert(iIndex, pChild);
	}

	void RemoveChild(int iIndex)
	{
		m_ChildList[iIndex]->m_pParentItem = nullptr;
		m_ChildList.removeAt(iIndex);
	}

	int GetNumChildren() const
	{
		return m_ChildList.size();
	}

	int GetRow() const
	{
		if(m_pParentItem)
			return m_pParentItem->m_ChildList.indexOf(const_cast<IModelTreeItem *>(this));

		return 0;
	}

	virtual QString GetToolTip() const = 0;
};

#endif // IMODELTREEITEM_H
