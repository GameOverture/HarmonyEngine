/**************************************************************************
*	IHyVisable.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/Visables/IHyVisable.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity3d.h"
#include "Scene/HyScene.h"
#include "Scene/Nodes/IHyNode.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

IHyVisable::IHyVisable() :	m_pScissor(nullptr),
							m_hStencil(HY_UNUSED_HANDLE),
							m_iCoordinateSystem(-1)
{
}

IHyVisable::IHyVisable(const IHyVisable &copyRef) :	m_pScissor(nullptr),
													m_hStencil(copyRef.m_hStencil),
													m_iCoordinateSystem(copyRef.m_iCoordinateSystem)
{
	if(copyRef.m_pScissor)
	{
		m_pScissor = HY_NEW ScissorRect();
		m_pScissor->m_LocalScissorRect = copyRef.m_pScissor->m_LocalScissorRect;
		GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}
}

IHyVisable::~IHyVisable()
{
	delete m_pScissor;
}

const IHyVisable &IHyVisable::operator=(const IHyVisable &rhs)
{
	delete m_pScissor;
	m_pScissor = nullptr;
	if(rhs.m_pScissor)
	{
		m_pScissor = HY_NEW ScissorRect();
		m_pScissor->m_LocalScissorRect = rhs.m_pScissor->m_LocalScissorRect;
		GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}

	m_hStencil = rhs.m_hStencil;
	m_iCoordinateSystem = rhs.m_iCoordinateSystem;

	return *this;
}

bool IHyVisable::IsScissorSet() const
{
	return m_pScissor != nullptr;
}

void IHyVisable::GetLocalScissor(HyScreenRect<int32> &scissorOut) const
{
	if(m_pScissor == nullptr)
		return;

	scissorOut = m_pScissor->m_LocalScissorRect;
}

void IHyVisable::GetWorldScissor(HyScreenRect<int32> &scissorOut)
{
	if(m_pScissor == nullptr)
	{
		scissorOut.iTag = SCISSORTAG_Disabled;
		return;
	}

	if(_VisableGetNodeRef().IsDirty(IHyNode::DIRTY_Scissor))
	{
		bool bHasParent = (_VisableGetParent2dPtr() != nullptr || _VisableGetParent3dPtr() != nullptr);
		if((_VisableGetNodeRef().m_uiExplicitAndTypeFlags & IHyNode::EXPLICIT_Scissor) == 0 && bHasParent)
		{
			if(_VisableGetParent2dPtr())
				_VisableGetParent2dPtr()->GetWorldScissor(m_pScissor->m_WorldScissorRect);
			else
				_VisableGetParent3dPtr()->GetWorldScissor(m_pScissor->m_WorldScissorRect);
		}
		else
		{
			if(m_pScissor->m_LocalScissorRect.iTag == SCISSORTAG_Enabled)
			{
				glm::mat4 mtx;

				if((_VisableGetNodeRef().m_uiExplicitAndTypeFlags & IHyNode::NODETYPE_Is2d) != 0)
					static_cast<IHyNode2d &>(_VisableGetNodeRef()).GetWorldTransform(mtx);
				else
					static_cast<IHyNode3d &>(_VisableGetNodeRef()).GetWorldTransform(mtx);

				m_pScissor->m_WorldScissorRect.x = static_cast<int32>(mtx[3].x + m_pScissor->m_LocalScissorRect.x);
				m_pScissor->m_WorldScissorRect.y = static_cast<int32>(mtx[3].y + m_pScissor->m_LocalScissorRect.y);
				m_pScissor->m_WorldScissorRect.width = static_cast<uint32>(mtx[0].x * m_pScissor->m_LocalScissorRect.width);
				m_pScissor->m_WorldScissorRect.height = static_cast<uint32>(mtx[1].y * m_pScissor->m_LocalScissorRect.height);
				m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Enabled;
			}
			else
			{
				m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;
			}
		}

		_VisableGetNodeRef().ClearDirty(IHyNode::DIRTY_Scissor);
	}

	scissorOut = m_pScissor->m_WorldScissorRect;
}

/*virtual*/ void IHyVisable::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight)
{
	if(m_pScissor == nullptr)
		m_pScissor = HY_NEW ScissorRect();

	m_pScissor->m_LocalScissorRect.x = uiLocalX;
	m_pScissor->m_LocalScissorRect.y = uiLocalY;
	m_pScissor->m_LocalScissorRect.width = uiWidth;
	m_pScissor->m_LocalScissorRect.height = uiHeight;
	m_pScissor->m_LocalScissorRect.iTag = SCISSORTAG_Enabled;

	_VisableGetNodeRef().m_uiExplicitAndTypeFlags |= IHyNode::EXPLICIT_Scissor;
	_VisableGetNodeRef().SetDirty(IHyNode::DIRTY_Scissor);

	GetWorldScissor(m_pScissor->m_WorldScissorRect);
}

/*virtual*/ void IHyVisable::ClearScissor(bool bUseParentScissor)
{
	if(m_pScissor == nullptr)
		return;

	m_pScissor->m_LocalScissorRect.iTag = SCISSORTAG_Disabled;
	m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;

	if(bUseParentScissor == false)
		_VisableGetNodeRef().m_uiExplicitAndTypeFlags |= IHyNode::EXPLICIT_Scissor;
	else
	{
		_VisableGetNodeRef().m_uiExplicitAndTypeFlags &= ~IHyNode::EXPLICIT_Scissor;
		if(_VisableGetParent2dPtr() != nullptr || _VisableGetParent3dPtr() != nullptr)
		{
			if(_VisableGetParent2dPtr())
				_VisableGetParent2dPtr()->GetWorldScissor(m_pScissor->m_WorldScissorRect);
			else
				_VisableGetParent3dPtr()->GetWorldScissor(m_pScissor->m_WorldScissorRect);
		}
	}
}

bool IHyVisable::IsStencilSet() const
{
	return m_hStencil != HY_UNUSED_HANDLE;
}

HyStencil *IHyVisable::GetStencil() const
{
	return IHyRenderer::FindStencil(m_hStencil);
}

/*virtual*/ void IHyVisable::SetStencil(HyStencil *pStencil)
{
	if(pStencil == nullptr)
		m_hStencil = HY_UNUSED_HANDLE;
	else
		m_hStencil = pStencil->GetHandle();

	_VisableGetNodeRef().m_uiExplicitAndTypeFlags |= IHyNode::EXPLICIT_Stencil;
}

/*virtual*/ void IHyVisable::ClearStencil(bool bUseParentStencil)
{
	m_hStencil = HY_UNUSED_HANDLE;

	if(bUseParentStencil == false)
		_VisableGetNodeRef().m_uiExplicitAndTypeFlags |= IHyNode::EXPLICIT_Stencil;
	else
	{
		_VisableGetNodeRef().m_uiExplicitAndTypeFlags &= ~IHyNode::EXPLICIT_Stencil;
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

int32 IHyVisable::GetCoordinateSystem() const
{
	return m_iCoordinateSystem;
}

/*virtual*/ void IHyVisable::UseCameraCoordinates()
{
	m_iCoordinateSystem = -1;
	_VisableGetNodeRef().m_uiExplicitAndTypeFlags |= IHyNode::EXPLICIT_CoordinateSystem;
}

/*virtual*/ void IHyVisable::UseWindowCoordinates(int32 iWindowIndex /*= 0*/)
{
	m_iCoordinateSystem = iWindowIndex;
	_VisableGetNodeRef().m_uiExplicitAndTypeFlags |= IHyNode::EXPLICIT_CoordinateSystem;
}

/*virtual*/ void IHyVisable::_SetScissor(const ScissorRect *pParentScissor, bool bIsOverriding)
{
	if(bIsOverriding)
		_VisableGetNodeRef().m_uiExplicitAndTypeFlags &= ~IHyNode::EXPLICIT_Scissor;

	if(0 == (_VisableGetNodeRef().m_uiExplicitAndTypeFlags & IHyNode::EXPLICIT_Scissor))
	{
		if(pParentScissor)
		{
			if(m_pScissor == nullptr)
				m_pScissor = HY_NEW ScissorRect();

			m_pScissor->m_WorldScissorRect = pParentScissor->m_WorldScissorRect;
		}
		else
		{
			delete m_pScissor;
			m_pScissor = nullptr;
		}
	}
}

/*virtual*/ void IHyVisable::_SetStencil(HyStencilHandle hHandle, bool bIsOverriding)
{
	if(bIsOverriding)
		_VisableGetNodeRef().m_uiExplicitAndTypeFlags &= ~IHyNode::EXPLICIT_Stencil;

	if(0 == (_VisableGetNodeRef().m_uiExplicitAndTypeFlags & IHyNode::EXPLICIT_Stencil))
		m_hStencil = hHandle;
}

/*virtual*/ void IHyVisable::_SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding)
{
	if(bIsOverriding)
		_VisableGetNodeRef().m_uiExplicitAndTypeFlags &= ~IHyNode::EXPLICIT_CoordinateSystem;

	if(0 == (_VisableGetNodeRef().m_uiExplicitAndTypeFlags & IHyNode::EXPLICIT_CoordinateSystem))
		m_iCoordinateSystem = iWindowIndex;
}
