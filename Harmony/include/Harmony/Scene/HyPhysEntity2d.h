/**************************************************************************
 *	HyPhysEntity2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyPhysEntity2d_h__
#define __HyPhysEntity2d_h__

#include "Afx/HyStdAfx.h"
#include "Box2D/Box2D.h"

#include "Scene/Instances/HyPrimitive2d.h"
#include "Scene/HyEntity2d.h"

class HyPhysEntity2d : public HyEntity2d
{
	friend class HyBox2dRuntime;
	friend class HyScene;
	static b2World *	sm_b2WorldRef;

	bool				m_bEnabled;

	const b2BodyType	m_keBodyType;
	b2Body *			m_b2Body;

	struct tFixture
	{
		enum eFixtureFlag
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
	vector<tFixture>	m_Fixtures;

	vec2				m_ptPrevPos;
	float				m_fPrevRotation;

public:
	HyPhysEntity2d(b2BodyType eBodyType);
	virtual ~HyPhysEntity2d(void);

	void GetVelocity(vec2 &vVelOut)									{ memcpy(&vVelOut, &m_b2Body->GetLinearVelocity(), sizeof(vec2)); }
	float GetVelocityX()											{ return m_b2Body->GetLinearVelocity().x; }
	float GetVelocityY()											{ return m_b2Body->GetLinearVelocity().y; }
	size_t GetNumFixtures()											{ return m_Fixtures.size(); }

	void SetLinearVelocity(vec2 vVel)								{ b2Vec2 vb2Vel(vVel.x, vVel.y); m_b2Body->SetLinearVelocity(vb2Vel); }
	void SetLinearVelocity(float fX, float fY)						{ b2Vec2 vb2Vel(fX, fY); m_b2Body->SetLinearVelocity(vb2Vel); }
	void ApplyLinearImpulse(vec2 vLinearImpulse)					{ m_b2Body->ApplyLinearImpulse(b2Vec2(vLinearImpulse.x, vLinearImpulse.y), b2Vec2(m_ptPosition.Get().x, m_ptPosition.Get().y), true); }
	void SetFixedRotation(bool bEnable)								{ m_b2Body->SetFixedRotation(bEnable); }

	bool IsGrounded(int32 iFixtureIndex)							{ return (m_Fixtures[iFixtureIndex].m_uiFlags & tFixture::FIXTUREFLAG_IsGrounded) != 0; }
	void SetFriction(int32 iFixtureIndex, float fFrictionAmt)		{ m_Fixtures[iFixtureIndex].pFixture->SetFriction(fFrictionAmt); }
	void SetRestitution(int32 iFixtureIndex, float fRestitutionAmt)	{ m_Fixtures[iFixtureIndex].pFixture->SetRestitution(fRestitutionAmt); }
	void SetDensity(int32 iFixtureIndex, float fDensityAmt)			{ m_Fixtures[iFixtureIndex].pFixture->SetDensity(fDensityAmt); m_b2Body->ResetMassData(); }
	void SetDensity(float fDensityAmt)								{ for(uint32 i = 0; i < m_Fixtures.size(); ++i) m_Fixtures[i].pFixture->SetDensity(fDensityAmt); m_b2Body->ResetMassData(); }

	int32 AddBoxFixture(float fHalfWidth, float fHalfHeight, float fDensity, vec2 vOffset = vec2(0.0f), float fAngleOffset = 0.0f);

	int32 AddCircleFixture(float fRadius, float fDensity, vec2 vOffset = vec2(0.0f));

	int32 AddEdgeChainFixture(vec2 *pVertices, uint32 uiNumVerts, bool bChainLoop, vec2 vOffset = vec2(0.0f), float fAngleOffset = 0.0f);

	//virtual void SetCoordinateType(eCoordinateType eCoordType, bool bDoConversion);

private:
	virtual void Update();
};

#endif /* __HyPhysEntity2d_h__ */
