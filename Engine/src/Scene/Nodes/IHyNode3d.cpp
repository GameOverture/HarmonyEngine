/**************************************************************************
*	IHyNode3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode3d.h"
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"

IHyNode3d::IHyNode3d(HyType eNodeType, HyEntity3d *pParent) :
	IHyNode(eNodeType),
	m_pParent(pParent),
	pos(*this, DIRTY_Position | DIRTY_Scissor | DIRTY_WorldAABB),
	rot(*this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	rot_pivot(*this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	scale(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB),
	scale_pivot(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB)
{
	scale.Set(1.0f);

	if(m_pParent)
		_CtorSetupNewChild(*m_pParent, *this);
}

IHyNode3d::IHyNode3d(const IHyNode3d &copyRef) :
	IHyNode(copyRef),
	pos(*this, DIRTY_Position | DIRTY_Scissor | DIRTY_WorldAABB),
	rot(*this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	rot_pivot(*this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
	scale(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB),
	scale_pivot(*this, DIRTY_Scale | DIRTY_Scissor | DIRTY_WorldAABB)
{
	pos.Set(copyRef.pos.Get());
	rot.Set(copyRef.rot.Get());
	rot_pivot.Set(copyRef.rot_pivot.Get());
	scale.Set(copyRef.scale.Get());
	scale_pivot.Set(copyRef.scale_pivot.Get());
}

IHyNode3d::IHyNode3d(IHyNode3d &&donor) :
	IHyNode(std::move(donor)),
	m_pParent(donor.ParentGet()),
	m_mtxCached(std::move(donor.m_mtxCached)),
	pos(*this, DIRTY_Position | DIRTY_Scissor | DIRTY_WorldAABB),
	rot(*this, DIRTY_Rotation | DIRTY_Scissor | DIRTY_WorldAABB),
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

IHyNode3d::~IHyNode3d()
{
}

IHyNode3d &IHyNode3d::operator=(const IHyNode3d &rhs)
{
	IHyNode::operator=(rhs);

	m_mtxCached = rhs.m_mtxCached;

	pos = rhs.pos;
	rot = rhs.rot;
	rot_pivot = rhs.rot_pivot;
	scale = rhs.scale;
	scale_pivot = rhs.scale_pivot;

	if(rhs.m_pParent)
		rhs.m_pParent->ChildAppend(*this);

	return *this;
}

IHyNode3d &IHyNode3d::operator=(IHyNode3d &&donor)
{
	IHyNode::operator=(std::move(donor));
	
	pos = std::move(donor.pos);
	rot = std::move(donor.rot);
	rot_pivot = std::move(donor.rot_pivot);
	scale = std::move(donor.scale);
	scale_pivot = std::move(donor.scale_pivot);

	if(donor.m_pParent)
		donor.m_pParent->ChildAppend(*this);

	donor.ParentDetach();

	return *this;
}

void IHyNode3d::ParentDetach()
{
	if(m_pParent == nullptr)
		return;

	m_pParent->ChildRemove(this);
}

HyEntity3d *IHyNode3d::ParentGet() const
{
	return m_pParent;
}

void IHyNode3d::GetLocalTransform(glm::mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	outMtx = glm::translate(outMtx, pos.Get());

	outMtx = glm::translate(outMtx, rot_pivot.Get());
	outMtx = glm::rotate(outMtx, rot.Get().x, glm::vec3(1, 0, 0));
	outMtx = glm::rotate(outMtx, rot.Get().y, glm::vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Get().z, glm::vec3(0, 0, 1));
	outMtx = glm::translate(outMtx, rot_pivot.Get() * -1.0f);

	outMtx = glm::translate(outMtx, scale_pivot.Get());
	outMtx = glm::scale(outMtx, scale.Get());
	outMtx = glm::translate(outMtx, scale_pivot.Get() * -1.0f);
}

const glm::mat4 &IHyNode3d::GetWorldTransform()
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

/*virtual*/ void IHyNode3d::Update() /*override*/
{
	IHyNode::Update();
}

/*friend*/ void _CtorSetupNewChild(HyEntity3d &parentRef, IHyNode3d &childRef)
{
	_CtorChildAppend(parentRef, childRef);

	childRef._SetVisible(parentRef.IsVisible(), false);
	childRef._SetPauseUpdate(parentRef.IsPauseUpdate(), false);
}
