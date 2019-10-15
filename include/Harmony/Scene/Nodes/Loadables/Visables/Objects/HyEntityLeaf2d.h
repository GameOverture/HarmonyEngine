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
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"

template<typename LEAF>
class HyEntityLeaf2d : public HyEntity2d
{
protected:
	LEAF			m_Leaf;

public:
	HyEntityLeaf2d(const char *szPrefix, const char *szName, HyEntity2d *pParent) :
		HyEntity2d(pParent),
		m_Leaf(szPrefix, szName, this)
	{
	}

	virtual ~HyEntityLeaf2d()
	{
		// Manually remove the defining leaf here so it doesn't assert when 'm_Leaf' destructs and removes itself from this class
		HyEntity2d::ChildRemove(&m_Leaf);
	}

	LEAF &GetLeaf()
	{
		return m_Leaf;
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
};

#endif /* HyEntity2d_h__ */
