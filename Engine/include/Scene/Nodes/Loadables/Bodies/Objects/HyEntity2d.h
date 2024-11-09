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

	enum EntityAttributes
	{
		ENTITYATTRIB_IsRegisteredAssemble	= 1 << 0,				// Indicates this entity is registered for the "post-update" assemble step within HyScene
		ENTITYATTRIB_AssembleNeeded			= 1 << 1,				// Flag to indicate this entity requires "post-update" assemble step

		ENTITYATTRIB_MouseInputEnabled		= 1 << 2,
		ENTITYATTRIB_MouseInputHover		= 1 << 3,
		ENTITYATTRIB_MouseInputDown			= 1 << 4,
		ENTITYATTRIB_MouseInputInvalid		= 1 << 5,				// When mouse input was initially pressed outside of bounds

		ENTITYATTRIB_ReverseDisplayOrder	= 1 << 6,

		ENTITYATTRIB_NEXTFLAG				= 1 << 7,
	};
	uint32									m_uiAttribs;

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
	
	virtual void SetScissor(const HyRect &scissorRect) override;
	//virtual void SetScissor(HyStencilHandle hScissorHandle) override;
	//void SetScissor(HyStencilHandle hScissorHandle, bool bOverrideExplicitChildren);
	void SetScissor(const HyRect &scissorRect, bool bOverrideExplicitChildren);
	
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
	virtual void ResetDisplayOrder() override; // Will override all children's display order explicit flags, and assign them their 'sibling display order'

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) override;
	virtual const b2AABB &GetSceneAABB() override;
	virtual float GetWidth(float fPercent = 1.0f) override;		// Derived classes encouraged to override with faster implementation
	virtual float GetHeight(float fPercent = 1.0f) override;	// Derived classes encouraged to override with faster implementation
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CHILDREN NODES
	void ChildAppend(IHyNode2d &childRef);
	virtual void ChildPrepend(IHyNode2d &childRef);
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
	bool IsMouseInputEnabled() const;
	void SetMouseInput(bool bEnable);
	void EnableMouseInput();
	void DisableMouseInput();
	bool IsMouseHover();
	bool IsMouseDown() const;

	void ShapeAppend(HyShape2d &shapeRef);
	bool ShapeRemove(HyShape2d &shapeRef);
	uint32 ShapeCount() const;
	HyShape2d *ShapeGet(uint32 uiIndex);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DISPLAY ORDER
	bool IsReverseDisplayOrder() const;
	void ReverseDisplayOrder(bool bReverse);
	int32 SetChildrenDisplayOrder(bool bOverrideExplicitChildren);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// LOAD/UNLOAD ALL CHILDREN
	virtual void Load() override;
	virtual void Unload() override;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OPTIONAL ASSEMBLE (post-update) STEP
	bool IsRegisteredAssembleEntity() const;
	void RegisterAssembleEntity(bool bRegister = true);
	void SetAssembleNeeded();
	void Assemble();
	
protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;
	virtual void Update() override;
	
	virtual bool IsChildrenLoaded() const override final;
	virtual void SetNewChildAttributes(IHyNode2d &childRef);

	virtual void SetParentsVisible(bool bParentsVisible) override final;
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) override final;
	virtual void _SetScissorStencil(HyStencilHandle hHandle, bool bIsOverriding) override final;
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding) override final;
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) override final;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override final;

	bool CalcMouseInBounds();

	void SyncPhysicsFixtures();
	void SyncPhysicsBody();

	// Optional user overrides below
	virtual void OnUpdate() { }
	virtual void OnAssemble() { }
	virtual void OnMouseEnter() { }
	virtual void OnMouseLeave() { }
	virtual void OnMouseDown() { }
	virtual void OnMouseUp() { }			// If considered 'mouse down' and the mouse is released (regardless of where)
	virtual void OnMouseClicked() { }		// If considered 'mouse down' and the mouse is released inside of bounds of this (IsMouseHover() == true)

	friend void HyNodeCtorAppend(HyEntity2d *pEntity, IHyNode2d *pChildNode);

protected: // Hide inherited functionality that doesn't exist for entities, but leave it protected for derived classes to optionally use
	using IHyLoadable::GetState;
	using IHyLoadable::SetState;
	using IHyLoadable::GetNumStates;
private: // Hide inherited functionality that doesn't exist for entities
	using IHyLoadable::GetPath;
	using IHyLoadable::GetName;
	using IHyLoadable::GetPrefix;
	using IHyLoadable2d::Init;
	using IHyLoadable2d::Uninit;
};

#endif /* HyEntity2d_h__ */
