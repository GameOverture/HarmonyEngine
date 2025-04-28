/**************************************************************************
*	HyLocomotion.cpp
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyLocomotion.h"
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

HyLocomotion2d::HyLocomotion2d() : // NOTE: These default values are assuming pixels per second for use in UpdateSimple()
	m_vThrottle(0.0f),
	m_vVelocity(0.0f),
	m_fMinSpeed(0.0f),
	m_fMaxSpeed(250.0f),
	m_fAccel(100.0f),
	m_fDecel(300.0f),
	m_fPogoVelocity(0.0f),
	m_fJumpSpeed(10.0f),
	m_fMoverGravity(30.0f),
	m_fFriction(8.0f),
	m_fAirSteer(0.2f)
{
}

HyLocomotion2d::HyLocomotion2d(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration) :
	m_vThrottle(0.0f),
	m_vVelocity(0.0f),
	m_fMinSpeed(fMinSpeed),
	m_fMaxSpeed(fMaxSpeed),
	m_fAccel(fAcceleration),
	m_fDecel(fDeceleration),
	m_fPogoVelocity(0.0f),
	m_fJumpSpeed(10.0f),
	m_fMoverGravity(30.0f),
	m_fFriction(8.0f),
	m_fAirSteer(0.2f)
{
}

HyLocomotion2d::HyLocomotion2d(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer) :
	m_vThrottle(0.0f),
	m_vVelocity(0.0f),
	m_fMinSpeed(fMinSpeed),
	m_fMaxSpeed(fMaxSpeed),
	m_fAccel(fAcceleration),
	m_fDecel(fDeceleration),
	m_fPogoVelocity(0.0f),
	m_fJumpSpeed(fJumpSpeed),
	m_fMoverGravity(fMoverGravity),
	m_fFriction(fFriction),
	m_fAirSteer(fAirSteer)
{
}

HyLocomotion2d::~HyLocomotion2d()
{
}

bool HyLocomotion2d::IsMoving() const
{
	return m_vThrottle.x != 0.0f || m_vThrottle.y != 0.0f || m_vVelocity.x != 0.0f || m_vVelocity.y != 0.0f;
}

glm::vec2 HyLocomotion2d::GetVelocity() const
{
	return m_vVelocity;
}

void HyLocomotion2d::SetVelocity(glm::vec2 vVelocity)
{
	m_vVelocity = vVelocity;
}

void HyLocomotion2d::SetVelocityX(float fVelocityX)
{
	m_vVelocity.x = fVelocityX;
}

void HyLocomotion2d::SetVelocityY(float fVelocityY)
{
	m_vVelocity.y = fVelocityY;
}

void HyLocomotion2d::SetupSimple(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration)
{
	m_fMinSpeed = fMinSpeed;
	m_fMaxSpeed = fMaxSpeed;
	m_fAccel = fAcceleration;
	m_fDecel = fDeceleration;
}

void HyLocomotion2d::SetupPhysical(float fMinSpeed, float fMaxSpeed, float fAcceleration, float fDeceleration, float fJumpSpeed, float fMoverGravity, float fFriction, float fAirSteer)
{
	m_fMinSpeed = fMinSpeed;
	m_fMaxSpeed = fMaxSpeed;
	m_fAccel = fAcceleration;
	m_fDecel = fDeceleration;

	m_fJumpSpeed = fJumpSpeed;
	m_fMoverGravity = fMoverGravity;
	m_fFriction = fFriction;
	m_fAirSteer = fAirSteer;
}

void HyLocomotion2d::GoUp()
{
	m_vThrottle.y = 1.0f;
}

void HyLocomotion2d::GoDown()
{
	m_vThrottle.y = -1.0f;
}

void HyLocomotion2d::GoLeft()
{
	m_vThrottle.x = -1.0f;
}

void HyLocomotion2d::GoRight()
{
	m_vThrottle.x = 1.0f;
}

void HyLocomotion2d::SetThrottle(glm::vec2 vThrottle)
{
	m_vThrottle.x = HyMath::Clamp(vThrottle.x, -1.0f, 1.0f);
	m_vThrottle.y = HyMath::Clamp(vThrottle.y, -1.0f, 1.0f);
}

void HyLocomotion2d::Jump()
{
	m_vVelocity.y = m_fJumpSpeed;
}

void HyLocomotion2d::StopX()
{
	m_vVelocity.x = 0.0f;
}

void HyLocomotion2d::StopY()
{
	m_vVelocity.y = 0.0f;
}

void HyLocomotion2d::UpdateSimple()
{
	const float fSpeedLimitX = m_fMaxSpeed * m_vThrottle.x;
	const float fSpeedLimitY = m_fMaxSpeed * m_vThrottle.y;

	// LEFT/RIGHT
	if(m_vThrottle.x < 0.0f) // LEFT
	{
		if(fSpeedLimitX <= m_vVelocity.x)
			m_vVelocity.x = HyMath::Max(fSpeedLimitX, m_vVelocity.x - (m_fAccel * HyEngine::DeltaTime()));
		else
			m_vVelocity.x = HyMath::Min(0.0f, m_vVelocity.x + (m_fDecel * HyEngine::DeltaTime()));
	}
	else if(m_vThrottle.x > 0.0f) // RIGHT
	{
		if(fSpeedLimitX >= m_vVelocity.x)
			m_vVelocity.x = HyMath::Min(fSpeedLimitX, m_vVelocity.x + (m_fAccel * HyEngine::DeltaTime()));
		else
			m_vVelocity.x = HyMath::Max(0.0f, m_vVelocity.x - (m_fDecel * HyEngine::DeltaTime()));
	}
	else // m_vThrottle.x == 0.0f
	{
		if(m_vVelocity.x > 0.0f)
			m_vVelocity.x = HyMath::Max(0.0f, m_vVelocity.x - (m_fDecel * HyEngine::DeltaTime()));
		else if(m_vVelocity.x < 0.0f)
			m_vVelocity.x = HyMath::Min(0.0f, m_vVelocity.x + (m_fDecel * HyEngine::DeltaTime()));
	}

	// UP/DOWN
	if(m_vThrottle.y > 0.0f) // UP
	{
		if(fSpeedLimitY >= m_vVelocity.y)
			m_vVelocity.y = HyMath::Min(fSpeedLimitY, m_vVelocity.y + (m_fAccel * HyEngine::DeltaTime()));
		else
			m_vVelocity.y = HyMath::Max(0.0f, m_vVelocity.y - (m_fDecel * HyEngine::DeltaTime()));
	}
	else if(m_vThrottle.y < 0.0f) // DOWN
	{
		if(fSpeedLimitY <= m_vVelocity.y)
			m_vVelocity.y = HyMath::Max(fSpeedLimitY, m_vVelocity.y - (m_fAccel * HyEngine::DeltaTime()));
		else
			m_vVelocity.y = HyMath::Min(0.0f, m_vVelocity.y + (m_fDecel * HyEngine::DeltaTime()));
	}
	else // m_vThrottle.y == 0.0f
	{
		if(m_vVelocity.y > 0.0f)
			m_vVelocity.y = HyMath::Max(0.0f, m_vVelocity.y - (m_fDecel * HyEngine::DeltaTime()));
		else if(m_vVelocity.y < 0.0f)
			m_vVelocity.y = HyMath::Min(0.0f, m_vVelocity.y + (m_fDecel * HyEngine::DeltaTime()));
	}

	m_vThrottle.x = m_vThrottle.y = 0.0f;
}

// https://github.com/id-Software/Quake/blob/master/QW/client/pmove.c#L390
// https://github.com/erincatto/box2d/blob/main/samples/sample_character.cpp#L230
// pogoFilter = { MoverBit, StaticBit | DynamicBit };
// collideFilter = { MoverBit, StaticBit | DynamicBit | MoverBit }; // Mover overlap filter
// castFilter = { MoverBit, StaticBit | DynamicBit }; // Movers don't sweep against other movers, allows for soft collision
void HyLocomotion2d::UpdatePhysical(b2WorldId hWorld, glm::vec2 &ptPosOut, bool &bOnGroundOut, const b2Capsule &moverCapsule, b2QueryFilter pogoFilter, b2QueryFilter collideFilter, b2QueryFilter castFilter)
{
	b2Transform transform = { { ptPosOut.x, ptPosOut.y }, b2Rot_identity };

	// Friction
	float fSpeed = b2Length({ m_vVelocity.x, m_vVelocity.y });
	if(fSpeed < m_fMinSpeed)
	{
		m_vVelocity.x = 0.0f;
		m_vVelocity.y = 0.0f;
	}
	else if(bOnGroundOut)
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

	if(bOnGroundOut)
		m_vVelocity.y = 0.0f;

	// Accelerate
	float fCurrentSpeed = b2Dot({ m_vVelocity.x, m_vVelocity.y }, vDesiredDirection);
	float fAddSpeed = fDesiredSpeed - fCurrentSpeed;
	if(fAddSpeed > 0.0f)
	{
		float fSteer = bOnGroundOut ? 1.0f : m_fAirSteer;
		float fAccelSpeed = fSteer * m_fAccel * m_fMaxSpeed * HyEngine::DeltaTime();
		if(fAccelSpeed > fAddSpeed)
			fAccelSpeed = fAddSpeed;

		b2Vec2 v = fAccelSpeed * vDesiredDirection;
		m_vVelocity.x += v.x;
		m_vVelocity.y += v.y;
	}

	// TODO: Apply mover gravity in the same direction b2World_GetGravity() is pointing
	m_vVelocity.y -= m_fMoverGravity * HyEngine::DeltaTime();

	b2Vec2 ptOrigin = b2TransformPoint(transform, moverCapsule.center1);
	b2Circle circle = { ptOrigin, 0.5f * moverCapsule.radius };
	b2Vec2 vSegmentOffset = { 0.75f * moverCapsule.radius, 0.0f };
	b2Segment segment;
	segment.point1 = ptOrigin - vSegmentOffset;
	segment.point2 = ptOrigin + vSegmentOffset;

	b2ShapeProxy proxy = {};
	b2Vec2 translation;

	float fPogoRestLength = 3.0f * moverCapsule.radius;
	float fRayLength = fPogoRestLength + moverCapsule.radius;

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
	if(bOnGroundOut == false)
		bOnGroundOut = castResult.hit && m_vVelocity.y <= 0.01f; // TODO: Need to account for direction of gravity b2World_GetGravity() is pointing
	else
		bOnGroundOut = castResult.hit;

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
		m_planeCount = 0;

		b2Capsule mover;
		mover.center1 = b2TransformPoint(transform, moverCapsule.center1);
		mover.center2 = b2TransformPoint(transform, moverCapsule.center2);
		mover.radius = moverCapsule.radius;

		b2World_CollideMover(hWorld, &mover, collideFilter, PlaneResultFcn, this);
		b2PlaneSolverResult result = b2SolvePlanes(target, m_planes, m_planeCount);

		itotalIterations += result.iterationCount;

		b2Vec2 moverTranslation = result.position - transform.p;

		float fraction = b2World_CastMover(hWorld, &moverCapsule, moverTranslation, castFilter);

		b2Vec2 delta = fraction * moverTranslation;
		transform.p += delta;

		if(b2LengthSquared(delta) < tolerance * tolerance)
		{
			break;
		}
	}

	vVel = b2ClipVector({ m_vVelocity.x, m_vVelocity.y }, m_planes, m_planeCount);
	m_vVelocity.x = vVel.x;
	m_vVelocity.y = vVel.y;

	ptPosOut.x = transform.p.x;
	ptPosOut.y = transform.p.y;

	m_vThrottle.x = m_vThrottle.y = 0.0f;
}

/*static*/ bool HyLocomotion2d::PlaneResultFcn(b2ShapeId shapeId, const b2PlaneResult *planeResult, void *context)
{
	assert(planeResult->hit == true);

	float maxPush = FLT_MAX;
	bool clipVelocity = true;

	HyShape2d *pUserDataShape = static_cast<HyShape2d *>(b2Shape_GetUserData(shapeId));
	if(pUserDataShape != nullptr)
		pUserDataShape->GetCollisionInfo(maxPush, clipVelocity);

	HyLocomotion2d *self = static_cast<HyLocomotion2d *>(context);
	if(self->m_planeCount < m_planeCapacity)
	{
		assert(b2IsValidPlane(planeResult->plane));
		self->m_planes[self->m_planeCount] = { planeResult->plane, maxPush, 0.0f, clipVelocity };
		self->m_planeCount += 1;
	}

	return true;
}
