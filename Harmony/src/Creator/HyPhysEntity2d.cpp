/**************************************************************************
 *	HyPhysEntity2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/HyPhysEntity2d.h"

/*static*/ b2World *HyPhysEntity2d::sm_b2WorldRef = NULL;

HyPhysEntity2d::HyPhysEntity2d(b2BodyType eBodyType) :	m_keBodyType(eBodyType),
														m_b2Body(NULL),
														m_ptPrevPos(0.0f),
														m_fPrevRotation(0.0f),
														m_bEnabled(true)
{
	// Physics by default don't show as visible
	//m_bEnabled = false;
}


HyPhysEntity2d::~HyPhysEntity2d(void)
{
	if(m_b2Body)
		sm_b2WorldRef->DestroyBody(m_b2Body);
}


int32 HyPhysEntity2d::AddBoxFixture(float fHalfWidth, float fHalfHeight, float fDensity, vec2 vOffset /*= vec2(0.0f)*/, float fAngleOffset /*= 0.0f*/)
{
	HyAssert(m_b2Body, "HyPhysEntity2d::AddBoxFixture() was invoked with an uninitialized instance");

	if(m_eCoordType == HYCOORD_Pixel)
		vOffset /= HyCreator::PixelsPerMeter();

	b2PolygonShape poly;
	poly.SetAsBox(fHalfWidth, fHalfHeight, b2Vec2(vOffset.x, vOffset.y), fAngleOffset);

	tFixture newFixture(m_b2Body->CreateFixture(&poly, fDensity));
	
	// Static and kinematic bodies are always grounded
	if(m_b2Body->GetType() == b2_staticBody || m_b2Body->GetType() == b2_kinematicBody)
		newFixture.SetGrounded();

	m_Fixtures.push_back(newFixture);
	newFixture.pFixture->SetUserData(&m_Fixtures[m_Fixtures.size()-1]);

	return m_Fixtures.size()-1;
}	

int32 HyPhysEntity2d::AddCircleFixture(float fRadius, float fDensity, vec2 vOffset /*= vec2(0.0f)*/)
{
	HyAssert(m_b2Body, "HyPhysEntity2d::SetAsBox() was invoked with an uninitialized instance");

	if(m_eCoordType == HYCOORD_Pixel)
		vOffset /= HyCreator::PixelsPerMeter();

	b2CircleShape poly;
	poly.m_radius = fRadius;
	poly.m_p.x = vOffset.x;
	poly.m_p.y = vOffset.y;

	tFixture newFixture(m_b2Body->CreateFixture(&poly, fDensity));
	
	// Static and kinematic bodies are always grounded
	if(m_b2Body->GetType() == b2_staticBody || m_b2Body->GetType() == b2_kinematicBody)
		newFixture.SetGrounded();

	m_Fixtures.push_back(newFixture);
	newFixture.pFixture->SetUserData(&m_Fixtures[m_Fixtures.size()-1]);

	return m_Fixtures.size()-1;
}

int32 HyPhysEntity2d::AddEdgeChainFixture(vec2 *pVertices, uint32 uiNumVerts, bool bChainLoop, vec2 vOffset /*= vec2(0.0f)*/, float fAngleOffset /*= 0.0f*/)
{
	HyAssert(m_b2Body, "HyPhysEntity2d::SetAsBox() was invoked with an uninitialized instance");

	if(m_eCoordType == HYCOORD_Pixel)
	{
		vOffset /= HyCreator::PixelsPerMeter();
		for(uint32 i = 0; i < uiNumVerts; ++i)
			pVertices[i] /= HyCreator::PixelsPerMeter();
	}

	b2ChainShape chain;
	if(bChainLoop)
		chain.CreateChain(reinterpret_cast<b2Vec2 *>(pVertices), uiNumVerts);
	else
		chain.CreateLoop(reinterpret_cast<b2Vec2 *>(pVertices), uiNumVerts);
	b2Transform xf;
	xf.p.x = vOffset.x;
	xf.p.y = vOffset.y;
	xf.q.Set(fAngleOffset);
	b2Vec2 tmp;
	for(int32 i = 0; i < chain.m_count; ++i)
		chain.m_vertices[i] = b2Mul(xf, chain.m_vertices[i]);

	tFixture newFixture(m_b2Body->CreateFixture(&chain, 0.0f));

	// Static and kinematic bodies are always grounded
	if(m_b2Body->GetType() == b2_staticBody || m_b2Body->GetType() == b2_kinematicBody)
		newFixture.SetGrounded();

	m_Fixtures.push_back(newFixture);
	newFixture.pFixture->SetUserData(&m_Fixtures[m_Fixtures.size()-1]);
	return m_Fixtures.size()-1;
}

/*virtual*/ void HyPhysEntity2d::Update()
{
	b2Transform b2Trans = m_b2Body->GetTransform();

	if(m_ptPrevPos != m_ptPosition.Get())
	{
		// Grab angle from box2d if it hasn't been set manually
		if(m_fPrevRotation == m_vRotation.Get().z)
			m_vRotation.Z(b2Trans.q.GetAngle());

		if(m_eCoordType == HYCOORD_Pixel)
			m_b2Body->SetTransform(b2Vec2(m_ptPosition.X() / HyCreator::PixelsPerMeter(), m_ptPosition.Y() / HyCreator::PixelsPerMeter()), m_vRotation.Z());
		else
			m_b2Body->SetTransform(b2Vec2(m_ptPosition.X(), m_ptPosition.Y()), m_vRotation.Z());
	}
	else
	{
		if(m_fPrevRotation != m_vRotation.Z())
			m_b2Body->SetTransform(b2Trans.p, m_vRotation.Z());
		else
			m_vRotation.Z(b2Trans.q.GetAngle());

		// Grab position and convert it if necessary from box2d 
		if(m_eCoordType == HYCOORD_Pixel)
		{
			m_ptPosition.X(b2Trans.p.x * HyCreator::PixelsPerMeter());
			m_ptPosition.Y(b2Trans.p.y * HyCreator::PixelsPerMeter());
		}
		else
		{
			m_ptPosition.X(b2Trans.p.x);
			m_ptPosition.Y(b2Trans.p.y);
		}
	}

	m_ptPrevPos = m_ptPosition.Get();
	m_fPrevRotation = m_vRotation.Get().z;
}

// For internal engine use
///*virtual*/ void HyPhysEntity2d::OnSetData(IData *pData)
//{
//	if(m_b2Body)
//		sm_b2WorldRef->DestroyBody(m_b2Body);
//
//	b2BodyDef def;
//	def.type = m_keBodyType;
//	def.position.x = m_ptPosition.Get().x;
//	def.position.y = m_ptPosition.Get().y;
//
//	m_b2Body = sm_b2WorldRef->CreateBody(&def);
//	m_b2Body->SetUserData(this);
//}

