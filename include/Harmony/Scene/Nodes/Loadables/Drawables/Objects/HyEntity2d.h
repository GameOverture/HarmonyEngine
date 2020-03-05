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
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable2d.h"
#include "Scene/Physics/HyPhysicsGrid.h"
#include "Scene/Physics/HyPhysicsCollider.h"

class HyEntity2d : public IHyDrawable2d
{
	friend class HyScene;

protected:
	std::vector<IHyNode2d *>				m_ChildList;

	enum Attributes
	{
		ATTRIBFLAG_MouseInput				= 1 << 1,
		ATTRIBFLAG_ReverseDisplayOrder		= 1 << 2,
	};
	uint32									m_uiAttributes;

	enum MouseInputState
	{
		MOUSEINPUT_None = 0,
		MOUSEINPUT_Hover,
		MOUSEINPUT_Down
	};
	MouseInputState							m_eMouseInputState;
	void *									m_pMouseInputUserParam;

	b2Body *								m_pPhysicsBody;
	std::vector<b2Fixture *>				m_pPhysicsColliders;

public:
	HyEntity2d(HyEntity2d *pParent = nullptr);
	HyEntity2d(std::string sPrefix, std::string sName, HyEntity2d *pParent);
	HyEntity2d(const HyEntity2d &) = delete;
	HyEntity2d(HyEntity2d &&donor);
	virtual ~HyEntity2d(void);

	HyEntity2d &operator=(HyEntity2d &&donor);

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

	virtual const b2AABB &GetWorldAABB() override;
	
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

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MOUSE BUTTON INPUT
	void EnableMouseInput(void *pUserParam = nullptr);
	void DisableMouseInput();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// PHYSICS
	void PhysInit(HyPhysicsGrid &physGridRef,
				  HyPhysicsType eType,
				  bool bIsEnabled = true,
				  bool bIsFixedRotation = false,
				  bool bIsCcd = false,
				  bool bIsAwake = true,
				  bool bAllowSleep = true,
				  float fGravityScale = 1.0f);

	HyPhysicsType PhysGetType() const;
	void PhysSetType(HyPhysicsType eType);
	bool PhysIsEnabled() const;
	void PhysSetEnabled(bool bEnable);
	bool PhysIsFixedRotation() const;
	void PhysSetFixedRotation(bool bFixedRot);
	bool PhysIsCcd() const;
	void PhysSetCcd(bool bContinuousCollisionDetection);
	bool PhysIsAwake() const;
	void PhysSetAwake(bool bAwake);
	bool PhysIsSleepingAllowed() const;
	void PhysSetSleepingAllowed(bool bAllowSleep);
	float PhysGetGravityScale() const;
	void PhysSetGravityScale(float fGravityScale);

	glm::vec2 PhysWorldCenterMass() const;
	glm::vec2 PhysLocalCenterMass() const;
	glm::vec2 PhysGetLinearVelocity() const;
	void PhysSetLinearVelocity(glm::vec2 vVelocity);
	float PhysGetAngularVelocity() const;
	void PhysSetAngularVelocity(float fOmega);
	void PhysApplyForce(const glm::vec2 &vForce, const glm::vec2 &ptPoint, bool bWake);
	void PhysApplyForceToCenter(const glm::vec2 &vForce, bool bWake);
	void PhysApplyTorque(float fTorque, bool bWake);
	void PhysApplyLinearImpulse(const glm::vec2 &vImpulse, const glm::vec2 &ptPoint, bool bWake);
	void PhysApplyLinearImpulseToCenter(const glm::vec2 &vImpulse, bool bWake);
	void PhysApplyAngularImpulse(float fImpulse, bool bWake);
	float PhysGetMass() const;
	float PhysGetInertia() const;

	// fFriction : The friction coefficient, usually in the range [0,1].
	// fRestitution : (elasticity) usually in the range [0,1].
	// fDensity : usually in kg/m^2.
	// bIsSensor : Is a sensor shape collects contact information but never generates a collision response.
	std::unique_ptr<HyPhysicsCollider> PhysAddCollider(const HyShape2d &shapeRef, float fDensity, float fFriction, float fRestitution, bool bIsSensor);
	std::unique_ptr<HyPhysicsCollider> PhysAddCircleCollider(float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor);
	std::unique_ptr<HyPhysicsCollider> PhysAddCircleCollider(const glm::vec2 &ptCenter, float fRadius, float fDensity, float fFriction, float fRestitution, bool bIsSensor);
	void PhysDestroyCollider(std::unique_ptr<HyPhysicsCollider> pCollider);

	void PhysRelease();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// DISPLAY ORDER
	bool IsReverseDisplayOrder() const;
	void ReverseDisplayOrder(bool bReverse);
	int32 SetChildrenDisplayOrder(bool bOverrideExplicitChildren);

	virtual void Load() override;
	virtual void Unload() override;
	
protected:
	virtual void Update() override final;
	virtual bool IsChildrenLoaded() const override final;
	virtual void OnDataAcquired() override;

	void SetNewChildAttributes(IHyNode2d &childRef);

	virtual void SetDirty(uint32 uiDirtyFlags) override;
	void ApplyDirty(uint32 uiDirtyFlags);

	virtual void _SetVisible(bool bEnabled, bool bIsOverriding) override final;
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding) override final;
	virtual void _SetScissor(const ScissorRect *pParentScissor, bool bIsOverriding) override final;
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding) override final;
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) override final;
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) override final;

	// Optional user overrides below
	virtual void OnUpdate() { }
	virtual void OnMouseEnter(void *pUserParam) { }
	virtual void OnMouseLeave(void *pUserParam) { }
	virtual void OnMouseDown(void *pUserParam) { }
	virtual void OnMouseUp(void *pUserParam) { }
	virtual void OnMouseClicked(void *pUserParam) { }

	friend void _CtorChildAppend(HyEntity2d &entityRef, IHyNode2d &childRef);
};

#endif /* HyEntity2d_h__ */
