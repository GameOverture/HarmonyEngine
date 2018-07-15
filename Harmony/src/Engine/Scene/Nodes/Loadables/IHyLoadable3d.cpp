/**************************************************************************
*	IHyLoadable3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"
#include "Scene/Nodes/Loadables/Entities/HyEntity3d.h"
#include "Renderer/IHyRenderer.h"
#include "Renderer/Effects/HyStencil.h"

/*static*/ HyAssets *IHyLoadable3d::sm_pHyAssets = nullptr;

IHyLoadable3d::IHyLoadable3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent) :	IHyNode3d(eNodeType, pParent),
																												m_eLoadState(HYLOADSTATE_Inactive),
																												m_pData(nullptr),
																												m_sName(szName ? szName : ""),
																												m_sPrefix(szPrefix ? szPrefix : ""),
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

IHyLoadable3d::IHyLoadable3d(const IHyLoadable3d &copyRef) :	IHyNode3d(copyRef),
																m_eLoadState(HYLOADSTATE_Inactive),
																m_pData(nullptr),
																m_sName(copyRef.m_sName),
																m_sPrefix(copyRef.m_sPrefix),
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

IHyLoadable3d::~IHyLoadable3d()
{
	delete m_pScissor;
}

const IHyLoadable3d &IHyLoadable3d::operator=(const IHyLoadable3d &rhs)
{
	IHyNode3d::operator=(rhs);

	if(m_sPrefix != rhs.m_sPrefix || m_sName != rhs.m_sName)
	{
		if(m_eLoadState != HYLOADSTATE_Inactive)
			Unload();

		m_sPrefix = rhs.m_sPrefix;
		m_sName = rhs.m_sName;
		m_pData = nullptr;			// Ensures virtual OnDataAcquired() is invoked when the below Load() is invoked
	}

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

	//CalculateColor();

	if(rhs.IsLoaded())
		Load();

	return *this;
}

const std::string &IHyLoadable3d::GetName() const
{
	return m_sName;
}

const std::string &IHyLoadable3d::GetPrefix() const
{
	return m_sPrefix;
}

const IHyNodeData *IHyLoadable3d::AcquireData()
{
	if(m_pData == nullptr)
	{
		HyAssert(sm_pHyAssets != nullptr, "AcquireData was called before the engine has initialized HyAssets");

		sm_pHyAssets->AcquireNodeData(this, m_pData);
		if(m_pData || m_eTYPE == HYTYPE_Primitive2d || m_eTYPE == HYTYPE_Entity2d)
			OnDataAcquired();
		else
			HyError("Could not find data for: " << GetPrefix() << "/" << GetName());
	}

	return m_pData;
}

/*virtual*/ bool IHyLoadable3d::IsLoaded() const /*override*/
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

/*virtual*/ void IHyLoadable3d::Load() /*override*/
{
	HyAssert(sm_pHyAssets, "IHyLoadable2d::Load was invoked before engine has been initialized");

	// Don't load if the name is blank, and it's required by this node type
	if(m_sName.empty() && m_eTYPE != HYTYPE_Entity2d && m_eTYPE != HYTYPE_Primitive2d && m_eTYPE != HYTYPE_TexturedQuad2d)
		return;

	sm_pHyAssets->LoadNodeData(this);
}

/*virtual*/ void IHyLoadable3d::Unload() /*override*/
{
	HyAssert(sm_pHyAssets, "IHyLoadable2d::Unload was invoked before engine has been initialized");
	sm_pHyAssets->RemoveNodeData(this);
}

bool IHyLoadable3d::IsScissorSet() const
{
	return m_pScissor != nullptr;
}

void IHyLoadable3d::GetLocalScissor(HyScreenRect<int32> &scissorOut) const
{
	if(m_pScissor == nullptr)
		return;

	scissorOut = m_pScissor->m_LocalScissorRect;
}

void IHyLoadable3d::GetWorldScissor(HyScreenRect<int32> &scissorOut)
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

/*virtual*/ void IHyLoadable3d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight)
{
	if(m_pScissor == nullptr)
		m_pScissor = HY_NEW ScissorRect();

	m_pScissor->m_LocalScissorRect.x = uiLocalX;
	m_pScissor->m_LocalScissorRect.y = uiLocalY;
	m_pScissor->m_LocalScissorRect.width = uiWidth;
	m_pScissor->m_LocalScissorRect.height = uiHeight;
	m_pScissor->m_LocalScissorRect.iTag = SCISSORTAG_Enabled;

	m_uiExplicitFlags |= EXPLICIT_Scissor;

	GetWorldScissor(m_pScissor->m_WorldScissorRect);
}

/*virtual*/ void IHyLoadable3d::ClearScissor(bool bUseParentScissor)
{
	if(m_pScissor == nullptr)
		return;

	m_pScissor->m_LocalScissorRect.iTag = SCISSORTAG_Disabled;
	m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;

	if(bUseParentScissor == false)
		m_uiExplicitFlags |= EXPLICIT_Scissor;
	else
	{
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;
		if(m_pParent)
			m_pParent->GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}
}

bool IHyLoadable3d::IsStencilSet() const
{
	return m_hStencil != HY_UNUSED_HANDLE;
}

HyStencil *IHyLoadable3d::GetStencil() const
{
	return IHyRenderer::FindStencil(m_hStencil);
}

/*virtual*/ void IHyLoadable3d::SetStencil(HyStencil *pStencil)
{
	if(pStencil == nullptr)
		m_hStencil = HY_UNUSED_HANDLE;
	else
		m_hStencil = pStencil->GetHandle();

	m_uiExplicitFlags |= EXPLICIT_Stencil;
}

/*virtual*/ void IHyLoadable3d::ClearStencil(bool bUseParentStencil)
{
	m_hStencil = HY_UNUSED_HANDLE;

	if(bUseParentStencil == false)
		m_uiExplicitFlags |= EXPLICIT_Stencil;
	else
	{
		m_uiExplicitFlags &= ~EXPLICIT_Stencil;
		if(m_pParent)
		{
			HyStencil *pStencil = m_pParent->GetStencil();
			m_hStencil = pStencil ? pStencil->GetHandle() : HY_UNUSED_HANDLE;
		}
	}
}

int32 IHyLoadable3d::GetCoordinateSystem() const
{
	return m_iCoordinateSystem;
}

/*virtual*/ void IHyLoadable3d::UseCameraCoordinates()
{
	m_iCoordinateSystem = -1;
	m_uiExplicitFlags |= EXPLICIT_CoordinateSystem;
}

/*virtual*/ void IHyLoadable3d::UseWindowCoordinates(int32 iWindowIndex /*= 0*/)
{
	m_iCoordinateSystem = iWindowIndex;
	m_uiExplicitFlags |= EXPLICIT_CoordinateSystem;
}

const IHyNodeData *IHyLoadable3d::UncheckedGetData()
{
	return m_pData;
}
