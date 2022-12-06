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

struct HyBox2dComponent
{
	b2Body *						m_pBody;
	b2Fixture *						m_pFixture;
	bool							m_bLockUpdate;

	HyBox2dComponent() :
		m_pBody(nullptr),
		m_pFixture(nullptr),
		m_bLockUpdate(false)
	{ }
};

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

	b2AABB							m_SceneAABB;			// Don't directly use, acquiring using GetSceneAABB()
															// Derived versions of this function will properly update 'm_SceneAABB' before returning

	HyBox2dComponent *				m_pBox2d;				// A pointer to the concrete value in HyScene::m_NodeMap_Collision, nullptr otherwise

public:
	HyAnimVec3						topColor;
	HyAnimVec3						botColor;
	HyAnimFloat						alpha;

	HyShape2d						shape;					// A shape representing *this for collision and physics
	HyPhysicsCtrl2d					physics;

public:
	IHyBody2d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity2d *pParent);
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

	bool IsMouseInBounds();

	virtual const b2AABB &GetSceneAABB() = 0;
	float GetSceneHeight();
	float GetSceneWidth();

	bool SetCollidable(HyBodyType eBodyType);
	bool IsSimulating() const;

protected:
	virtual void SetDirty(uint32 uiDirtyFlags) override;
	virtual void Update() override;

	void ShapeChanged();
	virtual void OnShapeChanged() { }; // Optional override to indicate whenever IHyBody2d::shape gets modified



	// Internal Entity propagation function overrides
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding);

private:
	void CalculateColor(float fExtrapolatePercent);

	virtual IHyNode &_VisableGetNodeRef() override final;
	virtual HyEntity2d *_VisableGetParent2dPtr() override final;
	virtual HyEntity3d *_VisableGetParent3dPtr() override final;
};

#endif /* IHyBody2d_h__ */
