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
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"

extern void HyNodeCtorAppend(HyEntity3d *pEntity, IHyNode3d *pChildNode);

IHyNode3d::IHyNode3d(HyType eNodeType, HyEntity3d *pParent) :
	IHyNode(eNodeType),
	m_pParent(pParent),
	pos(*this, DIRTY_Transform | DIRTY_SceneAABB),
	rot(*this, DIRTY_Transform | DIRTY_SceneAABB),
	rot_pivot(*this, DIRTY_Transform | DIRTY_SceneAABB),
	scale(*this, DIRTY_Transform | DIRTY_SceneAABB),
	scale_pivot(*this, DIRTY_Transform | DIRTY_SceneAABB)
{
	scale.SetAll(1.0f);

	if(m_pParent)
	{
		HyNodeCtorAppend(m_pParent, this);
		if(m_pParent->IsVisible() == false)
		{
			m_uiFlags &= ~SETTING_IsVisible;
		}
		if(m_pParent->IsPauseUpdate())
		{
			m_uiFlags |= SETTING_IsPauseUpdate;
			HyScene::AddNode_PauseUpdate(this);
		}
	}
}

IHyNode3d::IHyNode3d(const IHyNode3d &copyRef) :
	IHyNode(copyRef),
	pos(*this, DIRTY_Transform | DIRTY_SceneAABB),
	rot(*this, DIRTY_Transform | DIRTY_SceneAABB),
	rot_pivot(*this, DIRTY_Transform | DIRTY_SceneAABB),
	scale(*this, DIRTY_Transform | DIRTY_SceneAABB),
	scale_pivot(*this, DIRTY_Transform | DIRTY_SceneAABB)
{
	pos.Set(copyRef.pos.Get());
	rot.Set(copyRef.rot.Get());
	rot_pivot.Set(copyRef.rot_pivot.Get());
	scale.Set(copyRef.scale.Get());
	scale_pivot.Set(copyRef.scale_pivot.Get());
}

IHyNode3d::IHyNode3d(IHyNode3d &&donor) noexcept :
	IHyNode(std::move(donor)),
	m_pParent(donor.ParentGet()),
	m_mtxCached(std::move(donor.m_mtxCached)),
	pos(*this, DIRTY_Transform | DIRTY_SceneAABB),
	rot(*this, DIRTY_Transform | DIRTY_SceneAABB),
	rot_pivot(*this, DIRTY_Transform | DIRTY_SceneAABB),
	scale(*this, DIRTY_Transform | DIRTY_SceneAABB),
	scale_pivot(*this, DIRTY_Transform | DIRTY_SceneAABB)
{
	m_uiFlags |= NODETYPE_Is2d;

	pos = std::move(donor.pos);
	rot = std::move(donor.rot);
	rot_pivot = std::move(donor.rot_pivot);
	scale = std::move(donor.scale);
	scale_pivot = std::move(donor.scale_pivot);

	donor.ParentDetach();
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

void IHyNode3d::GetLocalTransform(glm::mat4 &outMtx, float fExtrapolatePercent) const
{
	outMtx = glm::mat4(1.0f);

	outMtx = glm::translate(outMtx, pos.Extrapolate(fExtrapolatePercent));

	outMtx = glm::translate(outMtx, rot_pivot.Extrapolate(fExtrapolatePercent));
	outMtx = glm::rotate(outMtx, rot.Extrapolate(fExtrapolatePercent).x, glm::vec3(1, 0, 0));
	outMtx = glm::rotate(outMtx, rot.Extrapolate(fExtrapolatePercent).y, glm::vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Extrapolate(fExtrapolatePercent).z, glm::vec3(0, 0, 1));
	outMtx = glm::translate(outMtx, rot_pivot.Extrapolate(fExtrapolatePercent) * -1.0f);

	outMtx = glm::translate(outMtx, scale_pivot.Extrapolate(fExtrapolatePercent));
	outMtx = glm::scale(outMtx, scale.Extrapolate(fExtrapolatePercent));
	outMtx = glm::translate(outMtx, scale_pivot.Extrapolate(fExtrapolatePercent) * -1.0f);
}

const glm::mat4 &IHyNode3d::GetSceneTransform(float fExtrapolatePercent)
{
	if(IsDirty(DIRTY_Transform) || fExtrapolatePercent != 0.0f)
	{
		if(m_pParent)
		{
			m_mtxCached = m_pParent->GetSceneTransform(fExtrapolatePercent);
			
			glm::mat4 mtxLocal;
			GetLocalTransform(mtxLocal, fExtrapolatePercent);

			m_mtxCached *= mtxLocal;
		}
		else
			GetLocalTransform(m_mtxCached, fExtrapolatePercent);

		ClearDirty(DIRTY_Transform);
	}

	return m_mtxCached;
}
