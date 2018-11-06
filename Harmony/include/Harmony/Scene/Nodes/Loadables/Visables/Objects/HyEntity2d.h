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
#include "Scene/Nodes/Loadables/Visables/IHyVisable2d.h"

class HyEntity2d : public IHyVisable2d
{
	friend class HyScene;

protected:
	std::vector<IHyNode2d *>		m_ChildList;

	// TODO: Change these attributes to be "Updatable components" that can attach to any specific entity.
	enum Attributes
	{
		ATTRIBFLAG_MouseInput				= 1 << 1,
		ATTRIBFLAG_ReverseDisplayOrder		= 1 << 2
	};
	uint32							m_uiAttributes;

	enum MouseInputState
	{
		MOUSEINPUT_None = 0,
		MOUSEINPUT_Hover,
		MOUSEINPUT_Down
	};
	MouseInputState					m_eMouseInputState;
	void *							m_pMouseInputUserParam;

public:
	HyEntity2d(const char *szPrefix, const char *szName, HyEntity2d *pParent = nullptr);
	HyEntity2d(HyEntity2d *pParent = nullptr);
	virtual ~HyEntity2d(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void SetEnabled(bool bEnabled) override;
	void SetEnabled(bool bEnabled, bool bOverrideExplicitChildren);

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
	void UseCameraCoordinates(bool bOverrideExplicitChildren = false);

	virtual void UseWindowCoordinates(int32 iWindowIndex = 0) override;
	void UseWindowCoordinates(int32 iWindowIndex, bool bOverrideExplicitChildren);
	
	virtual void SetDisplayOrder(int32 iOrderValue) override;
	void SetDisplayOrder(int32 iOrderValue, bool bOverrideExplicitChildren);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void ChildAppend(IHyNode2d &childRef);
	virtual bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childRef);
	bool ChildExists(IHyNode2d &childRef);
	virtual bool ChildRemove(IHyNode2d *pChild);
	virtual void ChildrenTransfer(HyEntity2d &newParent);
	virtual uint32 ChildCount();
	virtual IHyNode2d *ChildGet(uint32 uiIndex);
	void ForEachChild(std::function<void(IHyNode2d *)> func);

	bool EnableMouseInput(void *pUserParam = nullptr);
	void DisableMouseInput();

	void ReverseDisplayOrder(bool bReverse);

	virtual const b2AABB &GetWorldAABB() override;

	virtual void Load() override;
	virtual void Unload() override;
	
protected:
	virtual void Update() override final;
	virtual bool IsChildrenLoaded() const override final;
	virtual void OnDataAcquired() override;

	void SetNewChildAttributes(IHyNode2d &childRef);

	virtual void SetDirty(uint32 uiDirtyFlags) override;

	virtual void _SetEnabled(bool bEnabled, bool bIsOverriding) override final;
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
};

#endif /* HyEntity2d_h__ */
