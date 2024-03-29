/**************************************************************************
*	IHyDrawable2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyDrawable2d_h__
#define IHyDrawable2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable.h"

class IHyDrawable2d : public IHyBody2d, public IHyDrawable
{
	friend class HyScene;

public:
	IHyDrawable2d(HyType eInstType, const HyNodePath &nodePath, HyEntity2d *pParent);
	IHyDrawable2d(const IHyDrawable2d &copyRef);
	IHyDrawable2d(IHyDrawable2d &&donor) noexcept;
	virtual ~IHyDrawable2d();

	IHyDrawable2d &operator=(const IHyDrawable2d &rhs);
	IHyDrawable2d &operator=(IHyDrawable2d &&donor) noexcept;

	virtual const b2AABB &GetSceneAABB() override;

	virtual bool IsValidToRender() override final;

protected:
	virtual void Update() override final;

	virtual void OnLoaded() override;
	virtual void OnUnloaded() override;

	virtual bool OnIsValidToRender() = 0;

#ifdef HY_PLATFORM_GUI
public:
	template<typename HYDATATYPE>
	void GuiOverrideData(HyJsonObj itemDataObj, bool bUseGuiOverrideName = true);
#endif

private:
	virtual IHyNode &_DrawableGetNodeRef() override final;
};

#endif /* IHyDrawable2d_h__ */
