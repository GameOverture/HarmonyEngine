/**************************************************************************
*	IHyBody.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/IHyNode.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyBody::IHyBody() :
	m_iCoordinateSystem(-1),
	m_hStencil(HY_UNUSED_HANDLE)
{
}

IHyBody::IHyBody(const IHyBody &copyRef) :
	m_iCoordinateSystem(copyRef.m_iCoordinateSystem),
	m_hStencil(copyRef.m_hStencil)
{
}

IHyBody::IHyBody(IHyBody &&donor) :
	m_iCoordinateSystem(std::move(donor.m_iCoordinateSystem)),
	m_hStencil(std::move(donor.m_hStencil))
{
}

IHyBody::~IHyBody()
{
}

IHyBody &IHyBody::operator=(const IHyBody &rhs)
{
	m_iCoordinateSystem = rhs.m_iCoordinateSystem;
	m_hStencil = rhs.m_hStencil;

	return *this;
}

IHyBody &IHyBody::operator=(IHyBody &&donor)
{
	m_iCoordinateSystem = std::move(donor.m_iCoordinateSystem);
	m_hStencil = std::move(donor.m_hStencil);

	return *this;
}

bool IHyBody::IsStencilSet() const
{
	return m_hStencil != HY_UNUSED_HANDLE;
}

HyStencil *IHyBody::GetStencil() const
{
	return IHyRenderer::FindStencil(m_hStencil);
}

HyStencilHandle IHyBody::GetStencilHandle() const
{
	return m_hStencil;
}

/*virtual*/ void IHyBody::SetStencil(HyStencil *pStencil)
{
	if(pStencil == nullptr)
		m_hStencil = HY_UNUSED_HANDLE;
	else
		m_hStencil = pStencil->GetHandle();

	_VisableGetNodeRef().m_uiFlags |= IHyNode::EXPLICIT_Stencil;
}

/*virtual*/ void IHyBody::ClearStencil(bool bUseParentStencil)
{
	m_hStencil = HY_UNUSED_HANDLE;

	if(bUseParentStencil == false)
		_VisableGetNodeRef().m_uiFlags |= IHyNode::EXPLICIT_Stencil;
	else
	{
		_VisableGetNodeRef().m_uiFlags &= ~IHyNode::EXPLICIT_Stencil;
		if(_VisableGetParent2dPtr() != nullptr || _VisableGetParent3dPtr() != nullptr)
		{
			HyStencil *pStencil = nullptr;
			
			if(_VisableGetParent2dPtr())
				pStencil = _VisableGetParent2dPtr()->GetStencil();
			else
				pStencil = _VisableGetParent3dPtr()->GetStencil();

			m_hStencil = pStencil ? pStencil->GetHandle() : HY_UNUSED_HANDLE;
		}
	}
}

int32 IHyBody::GetCoordinateSystem() const
{
	return m_iCoordinateSystem;
}

/*virtual*/ void IHyBody::UseCameraCoordinates()
{
	m_iCoordinateSystem = -1;
	_VisableGetNodeRef().m_uiFlags |= IHyNode::EXPLICIT_CoordinateSystem;
}

/*virtual*/ void IHyBody::UseWindowCoordinates(int32 iWindowIndex /*= 0*/)
{
	m_iCoordinateSystem = iWindowIndex;
	_VisableGetNodeRef().m_uiFlags |= IHyNode::EXPLICIT_CoordinateSystem;
}

void IHyBody::SetCameraCoordinates()
{
	UseCameraCoordinates();
}

void IHyBody::SetWindowCoordinates(int32 iWindowIndex /*= 0*/)
{
	UseWindowCoordinates(iWindowIndex);
}

/*virtual*/ void IHyBody::_SetStencil(HyStencilHandle hHandle, bool bIsOverriding)
{
	if(bIsOverriding)
		_VisableGetNodeRef().m_uiFlags &= ~IHyNode::EXPLICIT_Stencil;

	if(0 == (_VisableGetNodeRef().m_uiFlags & IHyNode::EXPLICIT_Stencil))
		m_hStencil = hHandle;
}

/*virtual*/ void IHyBody::_SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding)
{
	if(bIsOverriding)
		_VisableGetNodeRef().m_uiFlags &= ~IHyNode::EXPLICIT_CoordinateSystem;

	if(0 == (_VisableGetNodeRef().m_uiFlags & IHyNode::EXPLICIT_CoordinateSystem))
		m_iCoordinateSystem = iWindowIndex;
}
