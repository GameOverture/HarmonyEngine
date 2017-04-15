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
#include "HyEngine.h"

IHyNode2d::IHyNode2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode(eNodeType),
																m_bDirty(false),
																m_bIsDraw2d(false),
																m_bPauseOverride(false),
																m_pParent(pParent),
																m_uiExplicitFlags(0),
																m_eCoordUnit(HYCOORDUNIT_Default),
																m_fRotation(0.0f),
																m_BoundingVolume(*this),
																pos(*this),
																rot(m_fRotation, *this),
																rot_pivot(*this),
																scale(*this),
																scale_pivot(*this)
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

bool IHyNode2d::IsDraw2d()
{
	return m_bIsDraw2d;
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

	SetDirty();
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
	if(m_bDirty)
	{
		if(m_pParent)
		{
			static_cast<IHyNode2d *>(m_pParent)->GetWorldTransform(m_mtxCached);
			GetLocalTransform(outMtx);	// Just use 'outMtx' rather than pushing another mat4 on the stack

			m_mtxCached *= outMtx;
		}
		else
			GetLocalTransform(m_mtxCached);

		m_bDirty = false;
	}

	outMtx = m_mtxCached;
}

void IHyNode2d::SetDirty()
{
	m_bDirty = true;
}

/*virtual*/ void IHyNode2d::_SetEnabled(bool bEnabled, bool bOverrideExplicitChildren)
{
	if(bOverrideExplicitChildren)
		m_uiExplicitFlags &= ~EXPLICIT_Enabled;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Enabled))
		m_bEnabled = bEnabled;
}

/*virtual*/ void IHyNode2d::_SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren)
{
	if(bOverrideExplicitChildren)
		m_uiExplicitFlags &= ~EXPLICIT_PauseUpdate;

	if(0 == (m_uiExplicitFlags & EXPLICIT_PauseUpdate))
	{
		if(bUpdateWhenPaused)
		{
			if(m_bPauseOverride == false)
				HyScene::AddNode_PauseUpdate(this);
		}
		else
		{
			if(m_bPauseOverride == true)
				HyScene::RemoveNode_PauseUpdate(this);
		}

		m_bPauseOverride = bUpdateWhenPaused;
		m_uiExplicitFlags |= EXPLICIT_PauseUpdate;
	}
}
