/**************************************************************************
*	IHyNode2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "HyEngine.h"

IHyNode2d::IHyNode2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode(eNodeType),
																m_pParent(pParent),
																m_eCoordUnit(HYCOORDUNIT_Default),
																m_fRotation(0.0f),
																m_BoundingVolume(*this),
																m_pPhysicsBody(nullptr),
																pos(*this, DIRTY_Transform),
																rot(m_fRotation, *this, DIRTY_Transform),
																rot_pivot(*this, DIRTY_Transform),
																scale(*this, DIRTY_Transform),
																scale_pivot(*this, DIRTY_Transform)
{
	scale.Set(1.0f);

	HyScene::AddNode(this);
	
	if(m_pParent)
		m_pParent->ChildAppend(*this);
}

/*virtual*/ IHyNode2d::~IHyNode2d()
{
	if(m_pPhysicsBody)
	{
		Hy_Physics2d().DestroyBody(m_pPhysicsBody);
		m_pPhysicsBody = nullptr;
	}

	ParentDetach();
	HyScene::RemoveNode(this);

	if(m_bPauseOverride)
		HyScene::RemoveNode_PauseUpdate(this);
}

void IHyNode2d::ParentDetach()
{
	if(m_pParent == nullptr)
		return;

	m_pParent->ChildRemove(this);
}

HyEntity2d *IHyNode2d::ParentGet()
{
	return m_pParent;
}

HyCoordinateUnit IHyNode2d::GetCoordinateUnit()
{
	return m_eCoordUnit;
}

// TODO: This needs to apply to everything in its hierarchy
void IHyNode2d::SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion)
{
	if(eCoordUnit == HYCOORDUNIT_Default)
		eCoordUnit = HyDefaultCoordinateUnit();

	if(m_eCoordUnit == eCoordUnit)
		return;

	if(bDoConversion)
	{
		switch(eCoordUnit)
		{
		case HYCOORDUNIT_Meters:	pos *= HyPixelsPerMeter();	break;
		case HYCOORDUNIT_Pixels:	pos /= HyPixelsPerMeter();	break;
		}
	}
	m_eCoordUnit = eCoordUnit;

	SetDirty(DIRTY_Transform);
}

void IHyNode2d::GetLocalTransform(glm::mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	glm::vec3 ptPos = pos.Extrapolate();
	glm::vec3 vScale = scale.Extrapolate();
	vScale.z = 1.0f;
	glm::vec3 ptRotPivot = rot_pivot.Extrapolate();
	glm::vec3 ptScalePivot = scale_pivot.Extrapolate();
	
	if(m_eCoordUnit == HYCOORDUNIT_Meters)
		outMtx = glm::translate(outMtx, ptPos * HyPixelsPerMeter());
	else
		outMtx = glm::translate(outMtx, ptPos);

	outMtx = glm::translate(outMtx, ptRotPivot);
	outMtx = glm::rotate(outMtx, rot.Get(), glm::vec3(0, 0, 1));
	outMtx = glm::translate(outMtx, ptRotPivot * -1.0f);

	outMtx = glm::translate(outMtx, ptScalePivot);
	outMtx = glm::scale(outMtx, vScale);
	outMtx = glm::translate(outMtx, ptScalePivot * -1.0f);
}

void IHyNode2d::GetWorldTransform(glm::mat4 &outMtx)
{
	if(IsDirty(DIRTY_Transform))
	{
		if(m_pParent)
		{
			m_pParent->GetWorldTransform(m_mtxCached);
			GetLocalTransform(outMtx);	// Just use 'outMtx' rather than pushing another mat4 on the stack

			m_mtxCached *= outMtx;
		}
		else
			GetLocalTransform(m_mtxCached);

		ClearDirty(DIRTY_Transform);
	}

	outMtx = m_mtxCached;
}

const HyShape2d &IHyNode2d::GetBoundingVolume()
{
	if(IsDirty(DIRTY_BoundingVolume) || m_BoundingVolume.IsValid() == false)
	{
		CalcBoundingVolume();
		ClearDirty(DIRTY_BoundingVolume);
	}

	return m_BoundingVolume;
}

b2Shape *IHyNode2d::GetBoundingVolumeIndex(uint32 uiIndex)
{
	return nullptr;
	//return m_BoundingVolumeList[uiIndex].Get;
}

void IHyNode2d::PhysicsInit(b2BodyDef &bodyDefOut)
{
	b2World &worldRef = Hy_Physics2d();

	if(m_pPhysicsBody)
		worldRef.DestroyBody(m_pPhysicsBody);
	
	bodyDefOut.userData = this;
	bodyDefOut.position.Set(pos.X(), pos.Y());
	bodyDefOut.angle = rot.Get();

	m_pPhysicsBody = worldRef.CreateBody(&bodyDefOut);
}

b2Body *IHyNode2d::PhysicsBody()
{
	return m_pPhysicsBody;
}

/*virtual*/ void IHyNode2d::PhysicsUpdate() /*override*/
{


	if(m_pPhysicsBody != nullptr && m_pPhysicsBody->IsActive())
	{
		pos.Set(m_pPhysicsBody->GetPosition().x, m_pPhysicsBody->GetPosition().y);
		rot.Set(glm::degrees(m_pPhysicsBody->GetAngle()));
	}
}
