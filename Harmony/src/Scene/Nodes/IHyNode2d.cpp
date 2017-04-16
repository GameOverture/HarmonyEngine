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
																pos(*this, HYNODEDIRTY_Transform),
																rot(m_fRotation, *this, HYNODEDIRTY_Transform),
																rot_pivot(*this, HYNODEDIRTY_Transform),
																scale(*this, HYNODEDIRTY_Transform),
																scale_pivot(*this, HYNODEDIRTY_Transform)
{
	scale.Set(1.0f);

	HyScene::AddNode(this);
	
	if(m_pParent)
		m_pParent->ChildAppend(*this);
}

/*virtual*/ IHyNode2d::~IHyNode2d()
{
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

bool IHyNode2d::ParentExists()
{
	return m_pParent != nullptr;
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

	SetDirty(HYNODEDIRTY_Transform);
}

void IHyNode2d::GetLocalTransform(glm::mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	glm::vec3 ptPos(0.0f);
	ptPos.x = pos.X();
	ptPos.y = pos.Y();

	glm::vec3 vScale(1.0f);
	vScale.x = scale.X();
	vScale.y = scale.Y();

	glm::vec3 ptRotPivot(0.0f);
	ptRotPivot.x = rot_pivot.X();
	ptRotPivot.y = rot_pivot.Y();

	glm::vec3 ptScalePivot(0.0f);
	ptScalePivot.x = scale_pivot.X();
	ptScalePivot.y = scale_pivot.Y();

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
	if(IsDirty(HYNODEDIRTY_Transform))
	{
		if(m_pParent)
		{
			m_pParent->GetWorldTransform(m_mtxCached);
			GetLocalTransform(outMtx);	// Just use 'outMtx' rather than pushing another mat4 on the stack

			m_mtxCached *= outMtx;
		}
		else
			GetLocalTransform(m_mtxCached);

		ClearDirty(HYNODEDIRTY_Transform);
	}

	outMtx = m_mtxCached;
}
