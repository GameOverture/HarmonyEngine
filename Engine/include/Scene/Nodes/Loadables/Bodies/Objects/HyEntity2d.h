/**************************************************************************
 *	HyEntity2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyEntity2d_h__
#define HyEntity2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"

class HyEntity2d : public IHyBody2d
{
	friend class HyScene;

protected:
	std::vector<IHyNode2d *>				m_ChildList;
	std::vector<HyShape2d *>				m_ShapeList;

	enum Entity2dAttributes
	{
		ENT2DATTRIB_MouseInputEnabled		= 1 << 1,
		ENT2DATTRIB_MouseInputHover			= 1 << 2,
		ENT2DATTRIB_MouseInputDown			= 1 << 3,
		ENT2DATTRIB_MouseInputInvalid		= 1 << 4,				// When mouse input was initially pressed outside of bounds

		ENT2DATTRIB_ReverseDisplayOrder		= 1 << 5,
	};
	uint32									m_uiEntAttribs;

public:
	HyPhysicsCtrl2d							physics;				// Optional physics component

public:
	HyEntity2d(HyEntity2d *pParent = nullptr);
	HyEntity2d(const HyEntity2d &) = delete;
	HyEntity2d(HyEntity2d &&donor) noexcept;
	virtual ~HyEntity2d(void);

	HyEntity2d &operator=(HyEntity2d &&donor) noexcept;

	// When children are established in the member-initializer list, it doesn't call SetNewChildAttributes().
	// InitChildren() will call SetNewChildAttributes() on all the current children
	void InitChildren();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OVERRIDES + OVERLOADS
	virtual void SetVisible(bool bEnabled) override;

	virtual void SetPauseUpdate(bool bUpdateWhenPaused) override;
	void SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren);
	
	virtual void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight) override;
	void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight, bool bOverrideExplicitChildren);
	
	virtual void ClearScissor(bool bUseParentScissor) override;
	void ClearScissor(bool bUseParentScissor, bool bOverrideExplicitChildren);

	virtual void SetStencil(HyStencil *pStencil) override;
	void SetStencil(HyStencil *pStencil, bool bOverrideExplicitChildren);
	
	virtual void ClearStencil(bool bUseParentStencil) override;
	void ClearStencil(bool bUseParentStencil, bool bOverrideExplicitChildren);

	virtual void UseCameraCoordinates() override;
	void UseCameraCoordinates(bool bOverrideExplicitChildren);

	virtual void UseWindowCoordinates(int32 iWindowIndex = 0) override;
	void UseWindowCoordinates(int32 iWindowIndex, bool bOverrideExplicitChildren);
	
	virtual void SetDisplayOrder(int32 iOrderValue) override;
	void SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren);
	virtual void ResetDisplayOrder() override;

	virtual const b2AABB &GetSceneAABB() override;
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CHILDREN NODES
	void ChildAppend(IHyNode2d &childRef);
	virtual bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childRef);
	bool ChildExists(IHyNode2d &childRef);
	virtual bool ChildRemove(IHyNode2d *pChild);
	virtual void ChildrenTransfer(HyEntity2d &newParent);
	virtual uint32 ChildCount();
	virtual IHyNode2d *ChildGet(uint32 uiIndex);
	void ForEachChild(std::function<void(IHyNode2d *)> func);
	std::vector<IHyNode2d *> FindChildren(std::function<bool(IHyNode2d *)> func);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MOUSE INPUT & SHAPES (hitboxes and/or collision)
	void EnableMouseInput();
	void DisableMouseInput();
	bool IsMouseInBounds();
	void ShapeAppend(HyShape2d &shapeRef);
	bool ShapeRemove(HyShape2d &shapeRef);
	uint32 ShapeCount() const;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DISPLAY ORDER
	bool IsReverseDisplayOrder() const;
	void ReverseDisplayOrder(bool bReverse);
	int32 SetChildrenDisplayOrder(bool bOverrideExplicitChildren);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// LOAD/UNLOAD ALL CHILDREN
	virtual void Load() override;
	virtual void Unload() override;
	
protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;
	virtual void Update() override;
	virtual bool IsChildrenLoaded() const override final;

	virtual void SetNewChildAttributes(IHyNode2d &childRef);

	virtual void SetParentsVisible(bool bParentsVisible) override final;
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) override final;
	virtual void _SetScissor(const ScissorRect *pParentScissor, bool bIsOverriding) override final;
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding) override final;
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) override final;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override final;

	void SyncPhysicsFixtures();
	void SyncPhysicsBody();

	// Optional user overrides below
	virtual void OnUpdate() { }
	virtual void OnMouseEnter() { }
	virtual void OnMouseLeave() { }
	virtual void OnMouseDown() { }
	virtual void OnMouseClicked() { }

	friend void HyNodeCtorAppend(HyEntity2d *pEntity, IHyNode2d *pChildNode);

private: // Hide inherited functionality that doesn't exist for entities
	using IHyLoadable::GetState;
	using IHyLoadable::SetState;
	using IHyLoadable::GetName;
	using IHyLoadable::GetPrefix;
	using IHyLoadable2d::Init;
	using IHyLoadable2d::Uninit;
};

#endif /* HyEntity2d_h__ */
