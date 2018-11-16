/**************************************************************************
*	IHyVisable2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyVisable2d_h__
#define IHyVisable2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Scene/Nodes/Loadables/Visables/IHyVisable.h"
#include "Scene/Tweens/HyTweenVec3.h"

class IHyVisable2d : public IHyLoadable2d, public IHyVisable
{
	friend class HyEntity2d;

protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

	int32							m_iDisplayOrder;		// Higher values are displayed front-most

public:
	HyTweenVec3						topColor;
	HyTweenVec3						botColor;
	HyTweenFloat					alpha;

public:
	IHyVisable2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent);
	IHyVisable2d(const IHyVisable2d &copyRef);
	virtual ~IHyVisable2d();

	const IHyVisable2d &operator=(const IHyVisable2d &rhs);

	void SetTint(float fR, float fG, float fB);
	void SetTint(uint32 uiColor);

	float CalculateAlpha();
	const glm::vec3 &CalculateTopTint();
	const glm::vec3 &CalculateBotTint();

	int32 GetDisplayOrder() const;
	virtual void SetDisplayOrder(int32 iOrderValue);

protected:
	virtual void Update() override;

	// Internal Entity propagation function overrides
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding);

private:
	void CalculateColor();

	virtual IHyNode &_VisableGetNodeRef() override final;
	virtual HyEntity2d *_VisableGetParent2dPtr() override final;
	virtual HyEntity3d *_VisableGetParent3dPtr() override final;

	friend void SetupNewChild(HyEntity2d &parentRef, IHyVisable2d &childRef);
};

#endif /* IHyVisable2d_h__ */
