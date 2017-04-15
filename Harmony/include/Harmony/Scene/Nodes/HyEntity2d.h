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

#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenVec3.h"

class HyEntity2d : public IHyNode2d
{
protected:
	std::vector<IHyNode2d *>		m_ChildList;

	float							m_fAlpha;

public:
	HyTweenVec3						tint;
	HyTweenFloat					alpha;

public:
	HyEntity2d(HyEntity2d *pParent = nullptr);
	virtual ~HyEntity2d(void);

	void SetEnabled(bool bEnabled, bool bOverrideExplicitChildren);
	void SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren);

	float CalcAlpha();
	void CalcTopTint(glm::vec3 &tintOut);

	void ChildAppend(IHyNode2d &childRef);
	bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childRef);
	bool ChildExists(IHyNode2d &childRef);
	bool ChildRemove(IHyNode2d *pChild);
	void ChildrenTransfer(HyEntity2d &newParent);
	uint32 ChildCount();
	IHyNode2d *ChildGet(uint32 uiIndex);

	void ForEachChild(std::function<void(IHyNode2d *)> func);

	// Optional user overrides below
	virtual void OnUpdate() { };
	virtual void OnCalcBoundingVolume() { }				// Should calculate the local bounding volume in 'm_BoundingVolume'
	virtual void OnMouseEnter(void *pUserParam) { }
	virtual void OnMouseLeave(void *pUserParam) { }
	virtual void OnMouseDown(void *pUserParam) { }
	virtual void OnMouseUp(void *pUserParam) { }
	virtual void OnMouseClicked(void *pUserParam) { }

private:
	virtual void SetNewChildAttributes(IHyNode2d &childInst);

	virtual void SetDirty() override;

	virtual void _SetEnabled(bool bEnabled, bool bOverrideExplicitChildren);
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren);
};

#endif /* __HyEntity2d_h__ */
