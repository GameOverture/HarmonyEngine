/**************************************************************************
*	HyLocomotionPhys.cpp
*
*	Harmony Engine
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyLocomotionPhys.h"
#include "HyEngine.h"

struct CastResult
{
	b2Vec2 point;
	b2BodyId bodyId;
	float fraction;
	bool hit;
};
static float CastCallback(b2ShapeId shapeId, b2Vec2 point, b2Vec2 normal, float fraction, void *context)
{
	CastResult *result = (CastResult *)context;
	result->point = point;
	result->bodyId = b2Shape_GetBody(shapeId);
	result->fraction = fraction;
	result->hit = true;
	return fraction;
}

HyLocomotionPhys2d::HyLocomotionPhys2d() :
	HyLocomotionPhys2d(glm::vec2(0.0f, 0.0f), HyLocomotionParams())
{ }

HyLocomotionPhys2d::HyLocomotionPhys2d(glm::vec2 vSize, float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer) :
	HyLocomotionPhys2d(vSize, HyLocomotionParams(fMinSpeed, fMaxSpeed, fAcceleration, fDeceleration, fJumpSpeed, fMoverGravity, fFriction, fAirSteer))
{ }

HyLocomotionPhys2d::HyLocomotionPhys2d(glm::vec2 vSize, const HyLocomotionParams &initRef) :
	HyLocomotion2d(initRef),
	m_fPogoVelocity(0.0f)
{
	Setup(vSize, initRef);
}

/*virtual*/ HyLocomotionPhys2d::~HyLocomotionPhys2d()
{
}

void HyLocomotionPhys2d::Setup(glm::vec2 vSize, const HyLocomotionParams &initRef)
{
	HyLocomotion2d::Setup(initRef);
	if(m_fMinSpeed <= 0.0f) // Min speed cannot be zero
		m_fMinSpeed = 0.001f;

	m_fJumpSpeed = initRef.m_fJumpSpeed;
	m_fMoverGravity = initRef.m_fMoverGravity;
	m_fFriction = initRef.m_fFriction;
	m_fAirSteer = initRef.m_fAirSteer;

	SetSize(vSize);
}

void HyLocomotionPhys2d::SetSize(glm::vec2 vSize)
{
	if(vSize.x <= 0)
		vSize.x = HyEngine::GetPixelsPerMeter() * 0.3f;
	if(vSize.y <= 0)
		vSize.y = HyEngine::GetPixelsPerMeter() * 2.0f;

	// NOTE: explicitly set 'm_Mover' in meters instead of pixels
	vSize.x *= HyEngine::GetPpmInverse();
	vSize.y *= HyEngine::GetPpmInverse();

	float fRadius = (vSize.x * 0.5f);

	m_Mover.center1 = { 0.0f, fRadius };
	m_Mover.center2 = { 0.0f, vSize.y - (fRadius * 2.0f) };
	m_Mover.radius = vSize.x * 0.5f;
}

void HyLocomotionPhys2d::Jump()
{
	m_vVelocity.y = m_fJumpSpeed;
}

// https://github.com/id-Software/Quake/blob/master/QW/client/pmove.c#L390
// https://github.com/erincatto/box2d/blob/main/samples/sample_character.cpp#L230
// pogoFilter = { MoverBit, StaticBit | DynamicBit };
// collideFilter = { MoverBit, StaticBit | DynamicBit | MoverBit }; // Mover overlap filter
// castFilter = { MoverBit, StaticBit | DynamicBit }; // Movers don't sweep against other movers, allows for soft collision
void HyLocomotionPhys2d::Update(b2WorldId hWorld, glm::vec2 &ptPosInOut, bool &bOnGroundInOut, b2QueryFilter pogoFilter, b2QueryFilter collideFilter, b2QueryFilter castFilter)
{
	b2Transform transform = { { ptPosInOut.x, ptPosInOut.y }, b2Rot_identity };

	// Friction
	float fSpeed = b2Length({ m_vVelocity.x, m_vVelocity.y });
	if(fSpeed < m_fMinSpeed)
	{
		m_vVelocity.x = 0.0f;
		m_vVelocity.y = 0.0f;
	}
	else if(bOnGroundInOut)
	{
		// Linear damping above 'm_fDecel' and fixed reduction below 'm_fDecel'
		float fControl = fSpeed < m_fDecel ? m_fDecel : fSpeed;

		// friction has units of 1/time
		float fDrop = fControl * m_fFriction * HyEngine::DeltaTime();
		float fNewSpeed = b2MaxFloat(0.0f, fSpeed - fDrop);
		m_vVelocity *= fNewSpeed / fSpeed;
	}

	b2Vec2 vDesiredVelocity = { m_fMaxSpeed * m_vThrottle.x, m_fMaxSpeed * m_vThrottle.y };
	float fDesiredSpeed;
	b2Vec2 vDesiredDirection = b2GetLengthAndNormalize(&fDesiredSpeed, vDesiredVelocity);

	if(fDesiredSpeed > m_fMaxSpeed)
		fDesiredSpeed = m_fMaxSpeed;

	if(bOnGroundInOut)
		m_vVelocity.y = 0.0f;

	// Accelerate
	float fCurrentSpeed = b2Dot({ m_vVelocity.x, m_vVelocity.y }, vDesiredDirection);
	float fAddSpeed = fDesiredSpeed - fCurrentSpeed;
	if(fAddSpeed > 0.0f)
	{
		float fSteer = bOnGroundInOut ? 1.0f : m_fAirSteer;
		float fAccelSpeed = fSteer * m_fAccel * m_fMaxSpeed * HyEngine::DeltaTime();
		if(fAccelSpeed > fAddSpeed)
			fAccelSpeed = fAddSpeed;

		b2Vec2 v = fAccelSpeed * vDesiredDirection;
		m_vVelocity.x += v.x;
		m_vVelocity.y += v.y;
	}

	// TODO: Apply mover gravity in the same direction b2World_GetGravity() is pointing
	m_vVelocity.y -= m_fMoverGravity * HyEngine::DeltaTime();

	b2Vec2 ptOrigin = b2TransformPoint(transform, m_Mover.center1);
	b2Circle circle = { ptOrigin, 0.5f * m_Mover.radius };
	b2Vec2 vSegmentOffset = { 0.75f * m_Mover.radius, 0.0f };
	b2Segment segment;
	segment.point1 = ptOrigin - vSegmentOffset;
	segment.point2 = ptOrigin + vSegmentOffset;

	b2ShapeProxy proxy = {};
	b2Vec2 translation;

	float fPogoRestLength = 3.0f * m_Mover.radius;
	float fRayLength = fPogoRestLength + m_Mover.radius;

	//if(m_pogoShape == PogoPoint)
	//{
	//	proxy = b2MakeProxy(&ptOrigin, 1, 0.0f);
	//	translation = { 0.0f, -fRayLength };
	//}
	//else if(m_pogoShape == PogoCircle)
	//{
		proxy = b2MakeProxy(&ptOrigin, 1, circle.radius);
		translation = { 0.0f, -fRayLength + circle.radius };
	//}
	//else
	//{
	//	proxy = b2MakeProxy(&segment.point1, 2, 0.0f);
	//	translation = { 0.0f, -fRayLength };
	//}
	
	CastResult castResult = {};
	b2World_CastShape(hWorld, &proxy, translation, pogoFilter, CastCallback, &castResult);

	// Avoid snapping to ground if still going up
	if(bOnGroundInOut == false)
		bOnGroundInOut = castResult.hit && m_vVelocity.y <= 0.01f; // TODO: Need to account for direction of gravity b2World_GetGravity() is pointing
	else
		bOnGroundInOut = castResult.hit;

	if(castResult.hit == false)
	{
		m_fPogoVelocity = 0.0f;

		//b2Vec2 delta = translation;
		//g_draw.DrawSegment(ptOrigin, ptOrigin + delta, b2_colorGray);

		////if(m_pogoShape == PogoPoint)
		////{
		////	g_draw.DrawPoint(ptOrigin + delta, 10.0f, b2_colorGray);
		////}
		////else if(m_pogoShape == PogoCircle)
		////{
		//	g_draw.DrawCircle(ptOrigin + delta, circle.radius, b2_colorGray);
		////}
		////else
		////{
		////	g_draw.DrawSegment(segment.point1 + delta, segment.point2 + delta, b2_colorGray);
		////}
	}
	else
	{
		float fPogoCurrentLength = castResult.fraction * fRayLength;

		const float fPOGO_HERTZ = 5.0f;
		const float fPOGO_DAMPING_RATIO = 0.8f;

		float zeta = fPOGO_DAMPING_RATIO;
		float hertz = fPOGO_HERTZ;
		float omega = 2.0f * B2_PI * hertz;
		float omegaH = omega * HyEngine::DeltaTime();

		m_fPogoVelocity = (m_fPogoVelocity - omega * omegaH * (fPogoCurrentLength - fPogoRestLength)) /
			(1.0f + 2.0f * zeta * omegaH + omegaH * omegaH);

		//b2Vec2 delta = castResult.fraction * translation;
		//g_draw.DrawSegment(ptOrigin, ptOrigin + delta, b2_colorGray);

		////if(m_pogoShape == PogoPoint)
		////{
		////	g_draw.DrawPoint(ptOrigin + delta, 10.0f, b2_colorPlum);
		////}
		////else if(m_pogoShape == PogoCircle)
		////{
		//	g_draw.DrawCircle(ptOrigin + delta, circle.radius, b2_colorPlum);
		////}
		////else
		////{
		////	g_draw.DrawSegment(segment.point1 + delta, segment.point2 + delta, b2_colorPlum);
		////}

		b2Body_ApplyForce(castResult.bodyId, { 0.0f, -50.0f }, castResult.point, true);
	}

	b2Vec2 vVel = { m_vVelocity.x, m_vVelocity.y };
	b2Vec2 target = transform.p + HyEngine::DeltaTime() * vVel + HyEngine::DeltaTime() * m_fPogoVelocity * b2Vec2{ 0.0f, 1.0f };

	int itotalIterations = 0;
	float tolerance = 0.01f;
	for(int iteration = 0; iteration < 5; ++iteration)
	{
		m_iPlaneCount = 0;

		b2Capsule mover;
		mover.center1 = b2TransformPoint(transform, m_Mover.center1);
		mover.center2 = b2TransformPoint(transform, m_Mover.center2);
		mover.radius = m_Mover.radius;

		b2World_CollideMover(hWorld, &mover, collideFilter, PlaneResultFcn, this);
		b2PlaneSolverResult result = b2SolvePlanes(target, m_planes, m_iPlaneCount);

		itotalIterations += result.iterationCount;

		b2Vec2 moverTranslation = result.position - transform.p;

		float fraction = b2World_CastMover(hWorld, &m_Mover, moverTranslation, castFilter);

		b2Vec2 delta = fraction * moverTranslation;
		transform.p += delta;

		if(b2LengthSquared(delta) < tolerance * tolerance)
		{
			break;
		}
	}

	vVel = b2ClipVector({ m_vVelocity.x, m_vVelocity.y }, m_planes, m_iPlaneCount);
	m_vVelocity.x = vVel.x;
	m_vVelocity.y = vVel.y;

	ptPosInOut.x = transform.p.x;
	ptPosInOut.y = transform.p.y;

	m_vThrottle.x = m_vThrottle.y = 0.0f;
}

/*static*/ bool HyLocomotionPhys2d::PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult *planeResult, void *pContext)
{
	assert(planeResult->hit == true);

	float maxPush = FLT_MAX;
	bool clipVelocity = true;

	HyShape2d *pUserDataShape = static_cast<HyShape2d *>(b2Shape_GetUserData(shapeId));
	if(pUserDataShape != nullptr)
		pUserDataShape->GetCollisionInfo(maxPush, clipVelocity);

	HyLocomotionPhys2d *pThis = static_cast<HyLocomotionPhys2d *>(pContext);
	if(pThis->m_iPlaneCount < m_NUM_PLANES)
	{
		assert(b2IsValidPlane(planeResult->plane));
		pThis->m_planes[pThis->m_iPlaneCount] = { planeResult->plane, maxPush, 0.0f, clipVelocity };
		pThis->m_iPlaneCount += 1;
	}

	return true;
}
