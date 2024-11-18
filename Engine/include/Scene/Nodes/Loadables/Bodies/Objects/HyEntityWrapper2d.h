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
	NODE2D				m_FusedNode;			// The scene node2d item to be wrapped as an entity
	int					m_iFusedNodeChildIndex;	// The index of 'm_FusedNode' in *this's (HyEntity2d) children list

public:
	HyEntityWrapper2d(HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent),
		m_FusedNode(HyNodePath(), this),
		m_iFusedNodeChildIndex(0)
	{ }

	HyEntityWrapper2d(const HyNodePath &leafNodePath, HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent),
		m_FusedNode(leafNodePath, this),
		m_iFusedNodeChildIndex(0)
	{ }

	HyEntityWrapper2d(const char *szLeafPrefix, const char *szLeafName, HyEntity2d *pParent = nullptr) :
		HyEntity2d(pParent),
		m_iFusedNodeChildIndex(0)
	{
		m_FusedNode.Init(szLeafPrefix, szLeafName, this);
	}

	virtual ~HyEntityWrapper2d()
	{
		// Manually remove the defining fused item here so it doesn't assert when 'm_FusedNode' destructs and removes itself from this class
		HyEntity2d::ChildRemove(&m_FusedNode);
	}

	NODE2D &operator *()
	{
		return m_FusedNode;
	}

	NODE2D &operator->()
	{
		return m_FusedNode;
	}

	NODE2D &GetFusedNode()
	{
		return m_FusedNode;
	}

	const NODE2D &GetFusedNode() const
	{
		return m_FusedNode;
	}

	NODE2D &GetLeaf() // DEPRECATED: legacy function name
	{
		return m_FusedNode;
	}
	const NODE2D &GetLeaf() const // DEPRECATED: legacy function name
	{
		return m_FusedNode;
	}

	virtual uint32 GetState() const override
	{
		return m_FusedNode.GetState();
	}

	virtual bool SetState(uint32 uiStateIndex) override
	{
		uint32 uiPrevState = m_FusedNode.GetState();
		if(m_FusedNode.SetState(uiStateIndex) == false)
			return false;
		
		if(IHyLoadable::SetState(uiStateIndex) == false)
		{
			m_FusedNode.SetState(uiPrevState);
			return false;
		}

		return true;
	}

	void InitLeaf(const HyNodePath &nodePath) // DEPRECATED: legacy function name
	{
		m_FusedNode.Init(nodePath, this);
	}

	void InitLeaf(const char *szLeafPrefix, const char *szLeafName) // DEPRECATED: legacy function name
	{
		m_FusedNode.Init(szLeafPrefix, szLeafName, this);
	}

	virtual void Init(const HyNodePath &nodePath, HyEntity2d *pParent) override
	{
		m_FusedNode.Init(nodePath, this);

		if(m_pParent != pParent)
		{
			if(pParent != nullptr)
				pParent->ChildAppend(*this);
			else
				ParentDetach();
		}
	}
	virtual void Init(const char *szPrefix, const char *szName, HyEntity2d *pParent) override
	{
		m_FusedNode.Init(HyNodePath(szPrefix, szName), this);

		if(m_pParent != pParent)
		{
			if(pParent != nullptr)
				pParent->ChildAppend(*this);
			else
				ParentDetach();
		}
	}
	virtual void Uninit()
	{
		m_FusedNode.Uninit();
	}

	virtual bool IsLoadDataValid() override
	{
		return m_FusedNode.IsLoadDataValid();
	}

	virtual void ChildPrepend(IHyNode2d &childRef)
	{
		HyEntity2d::ChildPrepend(childRef);
		m_iFusedNodeChildIndex += 1;
	}

	virtual bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childInst) override
	{
		if(&insertBefore == &m_FusedNode)
			m_iFusedNodeChildIndex += 1;

		return HyEntity2d::ChildInsert(insertBefore, childInst);
	}

	virtual bool ChildRemove(IHyNode2d *pChild) override
	{
		HyAssert(pChild != &m_FusedNode, "HyEntityWrapper2d::ChildRemove cannot remove its defining fused item");
		if(pChild == &m_FusedNode)
			return false;

		// Determine what index this child is in the list
		int iChildIndex = 0;
		for(; iChildIndex < m_ChildList.size(); ++iChildIndex)
		{
			if(pChild == m_ChildList[iChildIndex])
				break;
		}
		if(iChildIndex < m_iFusedNodeChildIndex)
			m_iFusedNodeChildIndex -= 1;

		return HyEntity2d::ChildRemove(pChild);
	}

	// Will not transfer its defining fused item, but will transfer all other children
	virtual void ChildrenTransfer(HyEntity2d &newParent) override
	{
		while(m_ChildList.size() > 1)
		{
			if(&m_FusedNode == m_ChildList[0])
				newParent.ChildAppend(*m_ChildList[1]);
			else
				newParent.ChildPrepend(*m_ChildList[0]);
		}
		m_iFusedNodeChildIndex = 0;
	}

	virtual uint32 ChildCount() override
	{
		return static_cast<uint32>(m_ChildList.size() - 1);	// -1 because the defining fused item doesn't count as it's permanently attached to this entity
	}

	virtual IHyNode2d *ChildGet(uint32 uiIndex) override
	{
		HyAssert(uiIndex < static_cast<uint32>(m_ChildList.size() - 1), "HyEntityWrapper2d::ChildGet passed an invalid index");

		// Don't return the defining fused item, and offset uiIndex depending on what 'm_iFusedNodeChildIndex' is
		if(m_iFusedNodeChildIndex == 0)
			return m_ChildList[uiIndex + 1];
		else
		{
			if(uiIndex < m_iFusedNodeChildIndex)
				return m_ChildList[uiIndex];
			else
				return m_ChildList[uiIndex + 1];
		}
	}
};

#endif /* HyEntityWrapper2d_h__ */
