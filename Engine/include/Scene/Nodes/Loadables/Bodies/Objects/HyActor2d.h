/**************************************************************************
 *	HyActor2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyActor2d_h__
#define HyActor2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Utilities/HyLocomotion.h"

class HyActor2d : public HyEntity2d
{
protected:
	enum ActorAttributes
	{
		ACTORATTRIB_IsAirborne				= 1 << 7,

		ACTORATTRIB_NEXTFLAG				= 1 << 8,
	};
	static_assert((int)ACTORATTRIB_IsAirborne == (int)ENTITYATTRIB_NEXTFLAG, "HyActor2d is not matching with base classes attrib flags");

	HyLocomotion2d							m_Locomotion;
	HyShape2d								m_PhysicsMover;

public:
	HyActor2d(HyEntity2d *pParent = nullptr);
	HyActor2d(const HyEntity2d &) = delete;
	HyActor2d(HyActor2d &&donor) noexcept;
	virtual ~HyActor2d(void);

	HyActor2d &operator=(HyActor2d &&donor) noexcept;

	bool IsOnGround() const;

	void SetThrottle(glm::vec2 vThrottle);
	void Jump();
	
protected:
	virtual void Update() override;
};
typedef HyActor2d HyActor;

#endif /* HyActor2d */
