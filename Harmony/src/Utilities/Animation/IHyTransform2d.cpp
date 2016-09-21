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

IHyTransform2d::IHyTransform2d()
{
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
