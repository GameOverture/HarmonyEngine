/**************************************************************************
 *	HyPhysEntity2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPhysEntity2d_h__
#define HyPhysEntity2d_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyPrimitive2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

class HyPhysEntity2d : public HyEntity2d
{
	friend class HyBox2dRuntime;
	friend class HyScene;
	static b2World *		sm_b2WorldRef;

	bool					m_bVisible;

	const b2BodyType		m_keBodyType;
	b2Body *				m_b2Body;

	struct tFixture
	{
		enum FixtureFlag
		{
			FIXTUREFLAG_IsGrounded = 1 << 0
		};
		uint32				m_uiFlags;

		b2Fixture *			pFixture;

		tFixture(b2Fixture *pfix) : pFixture(pfix),
			m_uiFlags(0)
		{ }

		void SetGrounded()		{ m_uiFlags |= FIXTUREFLAG_IsGrounded; }
		void UnsetGrounded()	{ m_uiFlags &= ~FIXTUREFLAG_IsGrounded; }
	};
	std::vector<tFixture>	m_Fixtures;

	glm::vec2				m_ptPrevPos;
	float					m_fPrevRotation;

public:
	HyPhysEntity2d(b2BodyType eBodyType);
	virtual ~HyPhysEntity2d(void);

	void GetVelocity(glm::vec2 &vVelOut)							{ memcpy(&vVelOut, &m_b2Body->GetLinearVelocity(), sizeof(glm::vec2)); }
	float GetVelocityX()											{ return m_b2Body->GetLinearVelocity().x; }
	float GetVelocityY()											{ return m_b2Body->GetLinearVelocity().y; }
	size_t GetNumFixtures()											{ return m_Fixtures.size(); }

	void SetLinearVelocity(glm::vec2 vVel)							{ b2Vec2 vb2Vel(vVel.x, vVel.y); m_b2Body->SetLinearVelocity(vb2Vel); }
	void SetLinearVelocity(float fX, float fY)						{ b2Vec2 vb2Vel(fX, fY); m_b2Body->SetLinearVelocity(vb2Vel); }
	void ApplyLinearImpulse(glm::vec2 vLinearImpulse)				{ m_b2Body->ApplyLinearImpulse(b2Vec2(vLinearImpulse.x, vLinearImpulse.y), b2Vec2(pos.Get().x, pos.Get().y), true); }
	void SetFixedRotation(bool bEnable)								{ m_b2Body->SetFixedRotation(bEnable); }

	bool IsGrounded(int32 iFixtureIndex)							{ return (m_Fixtures[iFixtureIndex].m_uiFlags & tFixture::FIXTUREFLAG_IsGrounded) != 0; }
	void SetFriction(int32 iFixtureIndex, float fFrictionAmt)		{ m_Fixtures[iFixtureIndex].pFixture->SetFriction(fFrictionAmt); }
	void SetRestitution(int32 iFixtureIndex, float fRestitutionAmt)	{ m_Fixtures[iFixtureIndex].pFixture->SetRestitution(fRestitutionAmt); }
	void SetDensity(int32 iFixtureIndex, float fDensityAmt)			{ m_Fixtures[iFixtureIndex].pFixture->SetDensity(fDensityAmt); m_b2Body->ResetMassData(); }
	void SetDensity(float fDensityAmt)								{ for(uint32 i = 0; i < m_Fixtures.size(); ++i) m_Fixtures[i].pFixture->SetDensity(fDensityAmt); m_b2Body->ResetMassData(); }
	
	void SetFilter(b2Filter & filter)								{ for(uint32 i = 0; i < m_Fixtures.size(); ++i) m_Fixtures[i].pFixture->SetFilterData(filter); }

	int32 AddBoxFixture(float fHalfWidth, float fHalfHeight, float fDensity, glm::vec2 vOffset = glm::vec2(0.0f), float fAngleOffset = 0.0f);

	int32 AddCircleFixture(float fRadius, float fDensity, glm::vec2 vOffset = glm::vec2(0.0f));

	int32 AddEdgeChainFixture(glm::vec2 *pVertices, uint32 uiNumVerts, bool bChainLoop, glm::vec2 vOffset = glm::vec2(0.0f), float fAngleOffset = 0.0f);

private:
	void UpdatePhysLOL();
};

#endif /* HyPhysEntity2d_h__ */
