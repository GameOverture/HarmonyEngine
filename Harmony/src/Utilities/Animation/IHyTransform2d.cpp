/**************************************************************************
*	IHyTransform2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Utilities/Animation/IHyTransform2d.h"

IHyTransform2d::IHyTransform2d(HyType eInstType) :	IHyTransform<HyAnimVec2>(eInstType),
													m_pParent(NULL),
													m_bDirty(true)
{
	SetOnDirtyCallback(OnDirty, this);
}

IHyTransform2d::~IHyTransform2d()
{
}

void IHyTransform2d::QueuePos(float fX, float fY, float fTweenDuration, HyTweenUpdateFunc fpEase, float fDefer /*= 0.0f*/)
{
	m_ActionQueue.AppendAction(fTweenDuration, fDefer, [&]	{
		this->pos.Tween(fX, fY, fTweenDuration, fpEase);
	});
}

void IHyTransform2d::QueueRot(float fX, float fY, float fTweenDuration, HyTweenUpdateFunc fpEase, float fDefer /*= 0.0f*/)
{
}

void IHyTransform2d::QueueScale(float fX, float fY, float fTweenDuration, HyTweenUpdateFunc fpEase, float fDefer /*= 0.0f*/)
{
}

void IHyTransform2d::QueueCallback(void(*fpCallback)(IHyTransform2d *, void *), void *pParam /*= NULL*/, float fDefer /*= 0.0f*/)
{
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

	if(m_eCoordUnit == HYCOORDUNIT_Meters)
		outMtx = glm::translate(outMtx, ptPos * IHyApplication::PixelsPerMeter());
	else
		outMtx = glm::translate(outMtx, ptPos);

	outMtx = glm::translate(outMtx, ptRotPivot);
	//outMtx = glm::rotate(outMtx, 0.0f/*rot.Get().x*/, glm::vec3(1, 0, 0));
	//outMtx = glm::rotate(outMtx, 0.0f/*rot.Get().y*/, glm::vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Get(), glm::vec3(0, 0, 1));
	outMtx = glm::translate(outMtx, ptRotPivot * -1.0f);

	outMtx = glm::scale(outMtx, vScale);
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

	outMtx = glm::scale(outMtx, vScale);
	//outMtx = glm::rotate(outMtx, 0.0f/*rot.Get().x*/, glm::vec3(1, 0, 0));
	//outMtx = glm::rotate(outMtx, 0.0f/*rot.Get().y*/, glm::vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Get(), glm::vec3(0, 0, 1));

	if(m_eCoordUnit == HYCOORDUNIT_Meters)
		outMtx = glm::translate(outMtx, ptPos * IHyApplication::PixelsPerMeter());
	else
		outMtx = glm::translate(outMtx, ptPos);
}

/*virtual*/ void IHyTransform2d::SetOnDirtyCallback(void(*fpOnDirty)(void *), void *pParam /*= NULL*/)
{
	m_fpOnDirty = fpOnDirty;
	m_pOnDirtyParam = pParam;

	pos.SetOnDirtyCallback(m_fpOnDirty, m_pOnDirtyParam);
	rot.SetOnDirtyCallback(m_fpOnDirty, m_pOnDirtyParam);
	scale.SetOnDirtyCallback(m_fpOnDirty, m_pOnDirtyParam);
}

void IHyTransform2d::GetWorldTransform(glm::mat4 &outMtx)
{
	if(m_bDirty)
	{
		if(m_pParent)
		{
			m_pParent->GetWorldTransform(m_mtxCached);
			GetLocalTransform(outMtx);	// Just use 'outMtx' rather than pushing another mat4 on the stack

			m_mtxCached *= outMtx;
		}
		else
			GetLocalTransform(m_mtxCached);

		m_bDirty = false;
	}

	outMtx = m_mtxCached;
}

void IHyTransform2d::AddChild(IHyTransform2d &childInst)
{
	childInst.Detach();

	childInst.m_pParent = this;
	m_ChildList.push_back(&childInst);
}

void IHyTransform2d::Detach()
{
	if(m_pParent == NULL)
		return;

	for(vector<IHyTransform2d *>::iterator iter = m_pParent->m_ChildList.begin(); iter != m_pParent->m_ChildList.end(); ++iter)
	{
		if(*iter == this)
		{
			m_pParent->m_ChildList.erase(iter);
			m_pParent = NULL;
			return;
		}
	}

	HyError("IHyTransform2d::Detach() could not find itself in parent's child list");
}

void IHyTransform2d::SetDirty()
{
	m_bDirty = true;

	for(uint32 i = 0; i < m_ChildList.size(); ++i)
		m_ChildList[i]->SetDirty();
}

/*static*/ void IHyTransform2d::OnDirty(void *pParam)
{
	IHyTransform2d *pThis = reinterpret_cast<IHyTransform2d *>(pParam);
	pThis->SetDirty();
}
