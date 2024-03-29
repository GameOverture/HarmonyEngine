/***************************************************************************************
 * HyEntityLeaf2d.h
 *
 * Copyright (c) 2017 Lightning Gaming, Inc.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software 
 * is hereby NOT granted. Please refer to the file "copyright.html"
 * for further important copyright and licensing information.
 *
 ***************************************************************************************/
#ifndef HyEntityLeaf2d_h__
#define HyEntityLeaf2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

template<typename LEAF>
class HyEntityLeaf2d : public HyEntity2d
{
protected:
	LEAF			m_Leaf;

public:
	HyEntityLeaf2d(HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent),
		m_Leaf(HyNodePath(), this)
	{
	}

	HyEntityLeaf2d(const HyNodePath &leafNodePath, HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent),
		m_Leaf(leafNodePath, this)
	{
	}

	HyEntityLeaf2d(const char *szLeafPrefix, const char *szLeafName, HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent)
	{
		m_Leaf.Init(szLeafPrefix, szLeafName, this);
	}

	virtual ~HyEntityLeaf2d()
	{
		// Manually remove the defining leaf here so it doesn't assert when 'm_Leaf' destructs and removes itself from this class
		HyEntity2d::ChildRemove(&m_Leaf);
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

	bool IsLeafValid() const
	{
		return m_Leaf.IsDataValid();
	}

	void InitLeaf(const HyNodePath &nodePath)
	{
		m_Leaf.Init(nodePath, this);
	}

	void InitLeaf(const char *szLeafPrefix, const char *szLeafName)
	{
		m_Leaf.Init(szLeafPrefix, szLeafName, this);
	}

	LEAF &GetLeaf()
	{
		return m_Leaf;
	}

	const LEAF &GetLeaf() const
	{
		return m_Leaf;
	}

	virtual bool IsLoadDataValid() override
	{
		return m_Leaf.IsLoadDataValid();
	}

	virtual bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childInst) override
	{
		HyAssert(&insertBefore != &m_Leaf, "HyEntityLeaf2d::ChildInsert cannot insert before its defining leaf");
		if(&insertBefore == &m_Leaf)
			return false;

		return HyEntity2d::ChildInsert(insertBefore, childInst);
	}

	virtual bool ChildRemove(IHyNode2d *pChild) override
	{
		HyAssert(pChild != &m_Leaf, "HyEntityLeaf2d::ChildRemove cannot remove its defining leaf");
		if(pChild == &m_Leaf)
			return false;

		return HyEntity2d::ChildRemove(pChild);
	}

	// Will not transfer its defining leaf, but will transfer all other children
	virtual void ChildrenTransfer(HyEntity2d &newParent) override
	{
		while(m_ChildList.size() > 1)
			newParent.ChildAppend(*m_ChildList[1]);
	}

	virtual uint32 ChildCount() override
	{
		return static_cast<uint32>(m_ChildList.size() - 1);	// -1 because the defining leaf doesn't count as it's permanently attached to this entity
	}

	virtual IHyNode2d *ChildGet(uint32 uiIndex) override
	{
		HyAssert(uiIndex < static_cast<uint32>(m_ChildList.size() - 1), "HyEntityLeaf2d::ChildGet passed an invalid index");
		return m_ChildList[uiIndex + 1];
	}

	LEAF &operator->()
	{
		return m_Leaf;
	}
};

#endif /* HyEntity2d_h__ */
