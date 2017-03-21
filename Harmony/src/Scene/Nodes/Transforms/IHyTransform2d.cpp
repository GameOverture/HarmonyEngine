/**************************************************************************
*	IHyTransform2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Transforms/IHyTransform2d.h"
#include "HyEngine.h"

IHyTransform2d::IHyTransform2d(HyType eInstType, IHyNode *pParent /*= nullptr*/) :	IHyTransform<HyTweenVec2>(eInstType, pParent),
																					m_eCoordUnit(HYCOORDUNIT_Default),
																					m_fRotation(0.0f),
																					m_BoundingVolume(*this),
																					rot(m_fRotation, *this)
{
}

IHyTransform2d::~IHyTransform2d()
{
}

HyCoordinateUnit IHyTransform2d::GetCoordinateUnit()
{
	return m_eCoordUnit;
}

void IHyTransform2d::SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion)
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

/*virtual*/ void IHyTransform2d::GetLocalTransform(glm::mat4 &outMtx) const
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

/*virtual*/ void IHyTransform2d::GetLocalTransform_SRT(glm::mat4 &outMtx) const
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

	// TODO: Use ptRotPivot and ptScalePivot below

	outMtx = glm::scale(outMtx, vScale);
	outMtx = glm::rotate(outMtx, rot.Get(), glm::vec3(0, 0, 1));

	if(m_eCoordUnit == HYCOORDUNIT_Meters)
		outMtx = glm::translate(outMtx, ptPos * HyPixelsPerMeter());
	else
		outMtx = glm::translate(outMtx, ptPos);
}

void IHyTransform2d::GetWorldTransform(glm::mat4 &outMtx)
{
	if(m_bDirty)
	{
		if(m_pParent)
		{
			static_cast<IHyTransform2d *>(m_pParent)->GetWorldTransform(m_mtxCached);
			GetLocalTransform(outMtx);	// Just use 'outMtx' rather than pushing another mat4 on the stack

			m_mtxCached *= outMtx;
		}
		else
			GetLocalTransform(m_mtxCached);

		m_bDirty = false;
	}

	outMtx = m_mtxCached;
}
