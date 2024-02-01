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
#include "Scene/Physics/HyPhysicsCtrl2d.h"

class IHyBody2d : public IHyLoadable2d, public IHyBody
{
	friend class HyScene;
	friend class HyEntity2d;
	friend class HyShape2d;
	friend class HyPhysicsCtrl2d;

protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

	int32							m_iDisplayOrder;		// Higher values are displayed front-most

	//HyShape2d						m_LocalBoundingVolume;	// A conforming shape around *this that assumes an identity matrix for its transform.

	b2AABB							m_SceneAABB;			// Don't directly use, acquiring using GetSceneAABB()
															// Derived versions of this function will properly update 'm_SceneAABB' before returning

public:
	HyAnimVec3						topColor;
	HyAnimVec3						botColor;
	HyAnimFloat						alpha;

public:
	IHyBody2d(HyType eNodeType, const HyNodePath &nodePath, HyEntity2d *pParent);
	IHyBody2d(const IHyBody2d &copyRef);
	IHyBody2d(IHyBody2d &&donor) noexcept;
	virtual ~IHyBody2d();

	IHyBody2d &operator=(const IHyBody2d &rhs);
	IHyBody2d &operator=(IHyBody2d &&donor) noexcept;

	void SetTint(HyColor color);
	void SetTint(HyColor topColor, HyColor botColor);

	float CalculateAlpha(float fExtrapolatePercent);
	const glm::vec3 &CalculateTopTint(float fExtrapolatePercent);
	const glm::vec3 &CalculateBotTint(float fExtrapolatePercent);

	int32 GetDisplayOrder() const;
	virtual void SetDisplayOrder(int32 iOrderValue);
	virtual void ResetDisplayOrder();

	virtual void CalcLocalBoundingShape(HyShape2d &shapeOut) = 0;
	virtual const b2AABB &GetSceneAABB() = 0;
	float GetSceneHeight();
	float GetSceneWidth();

	virtual float GetWidth(float fPercent = 1.0f) = 0;
	virtual float GetHeight(float fPercent = 1.0f) = 0;

protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;
	virtual void Update() override;

	// Internal Entity propagation function overrides
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding);

private:
	void CalculateColor(float fExtrapolatePercent);

	virtual IHyNode &_VisableGetNodeRef() override final;
	virtual HyEntity2d *_VisableGetParent2dPtr() override final;
	virtual HyEntity3d *_VisableGetParent3dPtr() override final;
};

#endif /* IHyBody2d_h__ */
