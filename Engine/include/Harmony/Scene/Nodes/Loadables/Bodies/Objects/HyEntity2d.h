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
#include "Scene/Physics/HyPhysicsGrid2d.h"
#include "Scene/Physics/HyPhysics2d.h"

class HyEntity2d : public IHyBody2d
{
	friend class HyScene;

protected:
	std::vector<IHyNode2d *>				m_ChildList;

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
	HyShape2d								shape;					// A shape representing this entity for collision and physics
	HyPhysics2d								physics;

public:
	HyEntity2d(HyEntity2d *pParent = nullptr);
	HyEntity2d(const HyEntity2d &) = delete;
	HyEntity2d(HyEntity2d &&donor) noexcept;
	virtual ~HyEntity2d(void);

	HyEntity2d &operator=(HyEntity2d &&donor) noexcept;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OVERRIDES + OVERLOADS
	virtual void SetVisible(bool bEnabled) override;
	void SetVisible(bool bEnabled, bool bOverrideExplicitChildren);

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
	// MOUSE BUTTON INPUT
	void EnableMouseInput();
	void DisableMouseInput();

	bool IsMouseInBounds();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DISPLAY ORDER
	bool IsReverseDisplayOrder() const;
	void ReverseDisplayOrder(bool bReverse);
	int32 SetChildrenDisplayOrder(bool bOverrideExplicitChildren);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CHILDREN LOAD/UNLOAD
	virtual void Load() override;
	virtual void Unload() override;
	
protected:
	virtual void Update() override final;
	virtual bool IsChildrenLoaded() const override final;
	virtual void OnDataAcquired() override;

	void SetNewChildAttributes(IHyNode2d &childRef);

	virtual void SetDirty(uint32 uiDirtyFlags) override;

	virtual void _SetVisible(bool bEnabled, bool bIsOverriding) override final;
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) override final;
	virtual void _SetScissor(const ScissorRect *pParentScissor, bool bIsOverriding) override final;
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding) override final;
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) override final;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override final;

	// Optional user overrides below
	virtual void OnUpdate() { }
	virtual void OnMouseEnter() { }
	virtual void OnMouseLeave() { }
	virtual void OnMouseDown() { }
	virtual void OnMouseClicked() { }

	friend void HyNodeCtorAppend(HyEntity2d *pEntity, IHyNode2d *pChildNode);
};

#endif /* HyEntity2d_h__ */
