/**************************************************************************
*	IHyDraw3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Draws/IHyDraw2d.h"
#include "Scene/Nodes/Draws/Entities/HyEntity3d.h"
#include "Renderer/IHyRenderer.h"

IHyDraw3d::IHyDraw3d(HyType eNodeType, HyEntity3d *pParent) :	IHyNode3d(eNodeType, pParent),
																m_fAlpha(1.0f),
																m_fCachedAlpha(1.0f),
																m_pScissor(nullptr),
																m_hStencil(HY_UNUSED_HANDLE),
																m_iCoordinateSystem(-1),
																m_iDisplayOrder(0),
																topColor(*this, DIRTY_Color),
																botColor(*this, DIRTY_Color),
																alpha(m_fAlpha, *this, DIRTY_Color)
{
	topColor.Set(1.0f);
	botColor.Set(1.0f);
	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyDraw3d::IHyDraw3d(const IHyDraw3d &copyRef) :	IHyNode3d(copyRef),
													m_fAlpha(copyRef.m_fAlpha),
													m_pScissor(nullptr),
													m_hStencil(copyRef.m_hStencil),
													m_iCoordinateSystem(copyRef.m_iCoordinateSystem),
													m_iDisplayOrder(copyRef.m_iDisplayOrder),
													topColor(*this, DIRTY_Color),
													botColor(*this, DIRTY_Color),
													alpha(m_fAlpha, *this, DIRTY_Color)
{
	if(copyRef.m_pScissor)
	{
		m_pScissor = HY_NEW ScissorRect();
		m_pScissor->m_LocalScissorRect = copyRef.m_pScissor->m_LocalScissorRect;
		GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}

	topColor.Set(copyRef.topColor.Get());
	botColor.Set(copyRef.botColor.Get());
	alpha.Set(copyRef.alpha.Get());
}

IHyDraw3d::~IHyDraw3d()
{
	delete m_pScissor;
}

const IHyDraw3d &IHyDraw3d::operator=(const IHyDraw3d &rhs)
{
	IHyNode3d::operator=(rhs);

	m_fAlpha = rhs.m_fAlpha;

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
	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor.Set(rhs.topColor.Get());
	botColor.Set(rhs.botColor.Get());
	alpha.Set(rhs.alpha.Get());

	return *this;
}

bool IHyDraw3d::IsScissorSet() const
{
	return m_pScissor != nullptr;
}

void IHyDraw3d::GetLocalScissor(HyScreenRect<int32> &scissorOut) const
{
	if(m_pScissor == nullptr)
		return;

	scissorOut = m_pScissor->m_LocalScissorRect;
}

void IHyDraw3d::GetWorldScissor(HyScreenRect<int32> &scissorOut)
{
	if(m_pScissor == nullptr)
		return;

	if(IsDirty(DIRTY_Scissor))
	{
		if((m_uiExplicitFlags & EXPLICIT_Scissor) == 0 && m_pParent)
			m_pParent->GetWorldScissor(m_pScissor->m_WorldScissorRect);
		else
		{
			if(m_pScissor->m_LocalScissorRect.iTag == SCISSORTAG_Enabled)
			{
				glm::mat4 mtx;
				GetWorldTransform(mtx);

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

		ClearDirty(DIRTY_Scissor);
	}

	scissorOut = m_pScissor->m_WorldScissorRect;
}

bool IHyDraw3d::IsStencilSet() const
{
	return m_hStencil != HY_UNUSED_HANDLE;
}

HyStencil *IHyDraw3d::GetStencil() const
{
	return IHyRenderer::FindStencil(m_hStencil);
}

