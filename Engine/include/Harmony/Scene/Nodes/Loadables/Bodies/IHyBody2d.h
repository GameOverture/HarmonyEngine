/**************************************************************************
*	IHyBody2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyBody2d_h__
#define IHyBody2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody.h"
#include "Scene/AnimFloats/HyAnimVec3.h"

class IHyBody2d : public IHyLoadable2d, public IHyBody
{
	friend class HyEntity2d;

protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

	int32							m_iDisplayOrder;		// Higher values are displayed front-most

public:
	HyAnimVec3						topColor;
	HyAnimVec3						botColor;
	HyAnimFloat						alpha;

public:
	IHyBody2d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity2d *pParent);
	IHyBody2d(const IHyBody2d &copyRef);
	IHyBody2d(IHyBody2d &&donor) noexcept;
	virtual ~IHyBody2d();

	IHyBody2d &operator=(const IHyBody2d &rhs);
	IHyBody2d &operator=(IHyBody2d &&donor) noexcept;

	void SetTint(float fR, float fG, float fB);
	void SetTint(HyColor color);
	void SetTint(uint32 uiColor);

	float CalculateAlpha();
	const glm::vec3 &CalculateTopTint();
	const glm::vec3 &CalculateBotTint();

	int32 GetDisplayOrder() const;
	virtual void SetDisplayOrder(int32 iOrderValue);
	virtual void ResetDisplayOrder();

protected:
	virtual void Update() override;

	// Internal Entity propagation function overrides
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding);

private:
	void CalculateColor();

	virtual IHyNode &_VisableGetNodeRef() override final;
	virtual HyEntity2d *_VisableGetParent2dPtr() override final;
	virtual HyEntity3d *_VisableGetParent3dPtr() override final;
};

#endif /* IHyBody2d_h__ */
