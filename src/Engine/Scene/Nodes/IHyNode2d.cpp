/**************************************************************************
*	IHyNode2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.h"
#include "HyEngine.h"

IHyNode2d::IHyNode2d(HyType eNodeType, HyEntity2d *pParent) :
	IHyNode(eNodeType),
	m_pParent(pParent),
	m_fRotation(0.0f),
	pos(*this, DIRTY_Position | DIRTY_Scissor | DIRTY_WorldAABB),
	rot(m_fRotation, *this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	rot_pivot(*this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	scale(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB),
	scale_pivot(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB)
{
	m_uiFlags |= NODETYPE_Is2d;
	scale.Set(1.0f);

	if(m_pParent)
		_CtorSetupNewChild(*m_pParent, *this);
}

IHyNode2d::IHyNode2d(const IHyNode2d &copyRef) :
	IHyNode(copyRef),
	m_pParent(copyRef.m_pParent),
	m_mtxCached(copyRef.m_mtxCached),
	m_fRotation(copyRef.m_fRotation),
	m_WorldAABB(copyRef.m_WorldAABB),
	pos(*this, DIRTY_Position | DIRTY_Scissor | DIRTY_WorldAABB),
	rot(m_fRotation, *this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	rot_pivot(*this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	scale(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB),
	scale_pivot(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB)
{
	m_uiFlags |= NODETYPE_Is2d;

	pos = copyRef.pos;
	rot = copyRef.rot;
	rot_pivot = copyRef.rot_pivot;
	scale = copyRef.scale;
	scale_pivot = copyRef.scale_pivot;

	if(m_pParent)
		_CtorSetupNewChild(*m_pParent, *this);
}

IHyNode2d::IHyNode2d(IHyNode2d &&donor) :
	IHyNode(std::move(donor)),
	m_pParent(donor.ParentGet()),
	m_mtxCached(std::move(donor.m_mtxCached)),
	m_fRotation(donor.m_fRotation),
	m_WorldAABB(std::move(donor.m_WorldAABB)),
	pos(*this, DIRTY_Position | DIRTY_Scissor | DIRTY_WorldAABB),
	rot(m_fRotation, *this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	rot_pivot(*this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	scale(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB),
	scale_pivot(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB)
{
	m_uiFlags |= NODETYPE_Is2d;

	pos = std::move(donor.pos);
	rot = std::move(donor.rot);
	rot_pivot = std::move(donor.rot_pivot);
	scale = std::move(donor.scale);
	scale_pivot = std::move(donor.scale_pivot);

	donor.ParentDetach();

	if(m_pParent)
		_CtorSetupNewChild(*m_pParent, *this);
}

/*virtual*/ IHyNode2d::~IHyNode2d()
{
	ParentDetach();
}

IHyNode2d &IHyNode2d::operator=(const IHyNode2d &rhs)
{
	IHyNode::operator=(rhs);
	
	pos = rhs.pos;
	rot = rhs.rot; // This will set 'm_fRotation'
	rot_pivot = rhs.rot_pivot;
	scale = rhs.scale;
	scale_pivot = rhs.scale_pivot;

	if(rhs.m_pParent)
		rhs.m_pParent->ChildAppend(*this);

	return *this;
}

IHyNode2d &IHyNode2d::operator=(IHyNode2d &&donor)
{
	IHyNode::operator=(std::move(donor));

	pos = std::move(donor.pos);
	rot = std::move(donor.rot); // This will set 'm_fRotation'
	rot_pivot = std::move(donor.rot_pivot);
	scale = std::move(donor.scale);
	scale_pivot = std::move(donor.scale_pivot);

	if(donor.m_pParent)
		donor.m_pParent->ChildAppend(*this);

	donor.ParentDetach();

	return *this;
}

void IHyNode2d::ParentDetach()
{
	if(m_pParent == nullptr)
		return;

	m_pParent->ChildRemove(this);
}

HyEntity2d *IHyNode2d::ParentGet() const
{
	return m_pParent;
}

void IHyNode2d::GetLocalTransform(glm::mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	glm::vec3 ptPos = pos.Extrapolate();
	glm::vec3 vScale = scale.Extrapolate();
	vScale.z = 1.0f;
	glm::vec3 ptRotPivot = rot_pivot.Extrapolate();
	glm::vec3 ptScalePivot = scale_pivot.Extrapolate();
	
	outMtx = glm::translate(outMtx, ptPos);

	outMtx = glm::translate(outMtx, ptRotPivot);
	outMtx = glm::rotate(outMtx, glm::radians(rot.Get()), glm::vec3(0, 0, 1));
	outMtx = glm::translate(outMtx, ptRotPivot * -1.0f);

	outMtx = glm::translate(outMtx, ptScalePivot);
	outMtx = glm::scale(outMtx, vScale);
	outMtx = glm::translate(outMtx, ptScalePivot * -1.0f);
}

const glm::mat4 &IHyNode2d::GetWorldTransform()
{
	if(IsDirty(DIRTY_Position | DIRTY_Rotation | DIRTY_Scale))
	{
		if(m_pParent)
		{
			m_mtxCached = m_pParent->GetWorldTransform();

			glm::mat4 mtxLocal;
			GetLocalTransform(mtxLocal);

			m_mtxCached *= mtxLocal;
		}
		else
			GetLocalTransform(m_mtxCached);

		ClearDirty(DIRTY_Position | DIRTY_Rotation | DIRTY_Scale);
	}

	return m_mtxCached;
}

/*virtual*/ const b2AABB &IHyNode2d::GetWorldAABB()
{
	return m_WorldAABB;
}

/*friend*/ void _CtorSetupNewChild(HyEntity2d &parentRef, IHyNode2d &childRef)
{
	_CtorChildAppend(parentRef, childRef);

	childRef._SetVisible(parentRef.IsVisible(), false);
	childRef._SetPauseUpdate(parentRef.IsPauseUpdate(), false);
}
