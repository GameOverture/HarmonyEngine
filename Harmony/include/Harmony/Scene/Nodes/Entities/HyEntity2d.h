/**************************************************************************
 *	HyEntity2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEntity2d_h__
#define __HyEntity2d_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Nodes/IHyNodeDraw2d.h"

class HyEntity2d : public IHyNodeDraw2d
{
protected:
	std::vector<IHyNode2d *>		m_ChildList;

	enum eAttributes
	{
		ATTRIBFLAG_Scissor					= 1 << 0,
		ATTRIBFLAG_MouseInput				= 1 << 1,
		ATTRIBFLAG_HasBoundingVolume		= 1 << 2,
		ATTRIBFLAG_BoundingVolumeDirty		= 1 << 3,
		ATTRIBFLAG_Physics					= 1 << 4
	};
	uint32							m_uiAttributes;

	enum eMouseInputState
	{
		MOUSEINPUT_None = 0,
		MOUSEINPUT_Hover,
		MOUSEINPUT_Down
	};
	eMouseInputState				m_eMouseInputState;
	void *							m_pMouseInputUserParam;

public:
	HyEntity2d(HyEntity2d *pParent = nullptr);
	virtual ~HyEntity2d(void);

	bool IsLoaded() const;

	void SetEnabled(bool bEnabled, bool bOverrideExplicitChildren);
	void SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren);
	//void SetScissor(
	int32 SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren);

	void ChildAppend(IHyNode2d &childRef);
	bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childRef);
	bool ChildExists(IHyNode2d &childRef);
	bool ChildRemove(IHyNode2d *pChild);
	void ChildrenTransfer(HyEntity2d &newParent);
	uint32 ChildCount();
	IHyNode2d *ChildGet(uint32 uiIndex);
	void ForEachChild(std::function<void(IHyNode2d *)> func);

	void EnableMouseInput(bool bEnable, void *pUserParam = nullptr);
	void EnableCollider(bool bEnable);
	void EnablePhysics(bool bEnable);
	
protected:
	virtual void NodeUpdate() override final;

	// Optional user overrides below
	virtual void OnUpdate() { };
	virtual void OnCalcBoundingVolume() { }				// Should calculate the local bounding volume in 'm_BoundingVolume'
	virtual void OnMouseEnter(void *pUserParam) { }
	virtual void OnMouseLeave(void *pUserParam) { }
	virtual void OnMouseDown(void *pUserParam) { }
	virtual void OnMouseUp(void *pUserParam) { }
	virtual void OnMouseClicked(void *pUserParam) { }

	virtual void SetDirty(HyNodeDirtyType eDirtyType) override;

	virtual void _SetEnabled(bool bEnabled, bool bIsOverriding) override;
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) override;
	virtual void _SetScissor(HyScissor &scissorRef, bool bIsOverriding) override;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override;

private:
	virtual void SetNewChildAttributes(IHyNode2d &childInst);
};

#endif /* __HyEntity2d_h__ */
