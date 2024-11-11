/**************************************************************************
 *	HyEntityWrapper2d.h
 *
 *	Harmony Engine
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyEntityWrapper2d_h__
#define HyEntityWrapper2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

template<typename NODE2D>
class HyEntityWrapper2d : public HyEntity2d
{
protected:
	NODE2D				m_Leaf;			// The scene node2d item to be wrapped
	int					m_iLeafIndex;	// The index of 'm_Leaf' in *this's (HyEntity2d) children list

public:
	HyEntityWrapper2d(HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent),
		m_Leaf(HyNodePath(), this),
		m_iLeafIndex(0)
	{ }

	HyEntityWrapper2d(const HyNodePath &leafNodePath, HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent),
		m_Leaf(leafNodePath, this),
		m_iLeafIndex(0)
	{ }

	HyEntityWrapper2d(const char *szLeafPrefix, const char *szLeafName, HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent),
		m_iLeafIndex(0)
	{
		m_Leaf.Init(szLeafPrefix, szLeafName, this);
	}

	virtual ~HyEntityWrapper2d()
	{
		// Manually remove the defining leaf here so it doesn't assert when 'm_Leaf' destructs and removes itself from this class
		HyEntity2d::ChildRemove(&m_Leaf);
	}

	NODE2D &operator *()
	{
		return m_Leaf;
	}

	NODE2D &operator->()
	{
		return m_Leaf;
	}

	NODE2D &GetLeaf()
	{
		return m_Leaf;
	}

	const NODE2D &GetLeaf() const
	{
		return m_Leaf;
	}

	virtual uint32 GetState() const override
	{
		return m_Leaf.GetState();
	}

	virtual bool SetState(uint32 uiStateIndex) override
	{
		uint32 uiPrevState = m_Leaf.GetState();
		if(m_Leaf.SetState(uiStateIndex) == false)
			return false;
		
		if(IHyLoadable::SetState(uiStateIndex) == false)
		{
			m_Leaf.SetState(uiPrevState);
			return false;
		}

		return true;
	}

	//bool IsLeafValid() const
	//{
	//	if((m_Leaf.GetInternalFlags() & IHyNode::NODETYPE_IsLoadable) != 0)
	//		return static_cast<IHyLoadable2d &>(m_Leaf).IsLoadDataValid();
	//	
	//	return true;
	//}

	void InitLeaf(const HyNodePath &nodePath)
	{
		m_Leaf.Init(nodePath, this);
	}

	void InitLeaf(const char *szLeafPrefix, const char *szLeafName)
	{
		m_Leaf.Init(szLeafPrefix, szLeafName, this);
	}

	virtual bool IsLoadDataValid() override
	{
		return m_Leaf.IsLoadDataValid();
	}

	virtual void ChildPrepend(IHyNode2d &childRef)
	{
		HyEntity2d::ChildPrepend(childRef);
		m_iLeafIndex += 1;
	}

	virtual bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childInst) override
	{
		if(&insertBefore == &m_Leaf)
			m_iLeafIndex += 1;

		return HyEntity2d::ChildInsert(insertBefore, childInst);
	}

	virtual bool ChildRemove(IHyNode2d *pChild) override
	{
		HyAssert(pChild != &m_Leaf, "HyEntityWrapper2d::ChildRemove cannot remove its defining leaf");
		if(pChild == &m_Leaf)
			return false;

		// Determine what index this child is in the list
		int iChildIndex = 0;
		for(; iChildIndex < m_ChildList.size(); ++iChildIndex)
		{
			if(pChild == m_ChildList[iChildIndex])
				break;
		}
		if(iChildIndex < m_iLeafIndex)
			m_iLeafIndex -= 1;

		return HyEntity2d::ChildRemove(pChild);
	}

	// Will not transfer its defining leaf, but will transfer all other children
	virtual void ChildrenTransfer(HyEntity2d &newParent) override
	{
		while(m_ChildList.size() > 1)
		{
			if(&m_Leaf == m_ChildList[0])
				newParent.ChildAppend(*m_ChildList[1]);
			else
				newParent.ChildPrepend(*m_ChildList[0]);
		}
		m_iLeafIndex = 0;
	}

	virtual uint32 ChildCount() override
	{
		return static_cast<uint32>(m_ChildList.size() - 1);	// -1 because the defining leaf doesn't count as it's permanently attached to this entity
	}

	virtual IHyNode2d *ChildGet(uint32 uiIndex) override
	{
		HyAssert(uiIndex < static_cast<uint32>(m_ChildList.size() - 1), "HyEntityWrapper2d::ChildGet passed an invalid index");

		// Don't return the defining leaf, and offset uiIndex depending on what 'm_iLeafIndex' is
		if(m_iLeafIndex == 0)
			return m_ChildList[uiIndex + 1];
		else
		{
			if(uiIndex < m_iLeafIndex)
				return m_ChildList[uiIndex];
			else
				return m_ChildList[uiIndex + 1];
		}
	}
};

#endif /* HyEntityWrapper2d_h__ */
