/**************************************************************************
 *	HyEntity3d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyEntity3d_h__
#define HyEntity3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody3d.h"

class HyEntity3d : public IHyBody3d
{
	friend class HyScene;

protected:
	std::vector<IHyNode3d *>		m_ChildList;

public:
	HyEntity3d(HyEntity3d *pParent = nullptr);
	virtual ~HyEntity3d(void);

	void ChildAppend(IHyNode3d &childRef);
	virtual bool ChildInsert(IHyNode3d &insertBefore, IHyNode3d &childRef);
	bool ChildExists(IHyNode3d &childRef);
	virtual bool ChildRemove(IHyNode3d *pChild);
	virtual void ChildrenTransfer(HyEntity3d &newParent);
	virtual uint32 ChildCount();
	virtual IHyNode3d *ChildGet(uint32 uiIndex);
	void ForEachChild(std::function<void(IHyNode3d *)> func);

protected:
	virtual void Update() override final;
	virtual bool IsChildrenLoaded() const override final;

	void SetNewChildAttributes(IHyNode3d &childRef);

	virtual void SetDirty(uint32 uiDirtyFlags) override;

	// Optional user overrides below
	virtual void OnUpdate() { }

	friend void HyNodeCtorAppend(HyEntity3d *pEntity, IHyNode3d *pChildNode);
};

#endif /* HyEntity3d_h__ */
