/**************************************************************************
*	IHyDrawInst2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "Renderer/Effects/HyStencil.h"
#include "Renderer/Effects/HyPortal2d.h"
#include "HyEngine.h"

/*static*/ HyAssets *IHyDrawInst2d::sm_pHyAssets = nullptr;

IHyDrawInst2d::IHyDrawInst2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyDraw2d(eNodeType, pParent),
																												m_eLoadState(HYLOADSTATE_Inactive),
																												m_pData(nullptr),
																												m_sNAME(szName ? szName : ""),
																												m_sPREFIX(szPrefix ? szPrefix : ""),
																												m_hShader(HY_UNUSED_HANDLE),
																												m_eRenderMode(HYRENDERMODE_Unknown),
																												m_hTextureHandle(HY_UNUSED_HANDLE),
																												m_BoundingVolume(this)
{
	memset(m_hPortals, HY_UNUSED_HANDLE, sizeof(HyPortal2dHandle) * HY_MAX_PORTAL_HANDLES);
}

IHyDrawInst2d::~IHyDrawInst2d()
{
	for(uint32 i = 0; m_hPortals[i] != HY_UNUSED_HANDLE && i < HY_MAX_PORTAL_HANDLES; ++i)
		IHyRenderer::FindPortal2d(m_hPortals[i])->RemoveInstance(this);

	if(m_eLoadState != HYLOADSTATE_Inactive)
		Unload();
}

void IHyDrawInst2d::SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight)
{
	if(m_hScissor == HY_UNUSED_HANDLE)
		m_hScissor = HY_NEW ScissorRect();

	m_hScissor->m_LocalScissorRect.x = uiLocalX;
	m_hScissor->m_LocalScissorRect.y = uiLocalY;
	m_hScissor->m_LocalScissorRect.width = uiWidth;
	m_hScissor->m_LocalScissorRect.height = uiHeight;
	m_hScissor->m_LocalScissorRect.iTag = SCISSORTAG_Enabled;

	m_uiExplicitFlags |= EXPLICIT_Scissor;

	GetWorldScissor(m_hScissor->m_WorldScissorRect);
}

void IHyDrawInst2d::ClearScissor(bool bUseParentScissor)
{
	if(m_hScissor == HY_UNUSED_HANDLE)
		return;

	m_hScissor->m_LocalScissorRect.iTag = SCISSORTAG_Disabled;
	m_hScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;

	if(bUseParentScissor == false)
		m_uiExplicitFlags |= EXPLICIT_Scissor;
	else
	{
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;
		if(m_pParent)
			m_pParent->GetWorldScissor(m_hScissor->m_WorldScissorRect);
	}
}

void IHyDrawInst2d::SetStencil(HyStencil *pStencil)
{
	if(pStencil == nullptr)
		m_hStencil = HY_UNUSED_HANDLE;
	else
		m_hStencil = pStencil->GetHandle();

	m_uiExplicitFlags |= EXPLICIT_Stencil;
}

void IHyDrawInst2d::ClearStencil(bool bUseParentStencil)
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

void IHyDrawInst2d::UseCameraCoordinates()
{
	m_iCoordinateSystem = -1;
	m_uiExplicitFlags |= EXPLICIT_CoordinateSystem;
}

void IHyDrawInst2d::UseWindowCoordinates(int32 iWindowIndex /*= 0*/)
{
	m_iCoordinateSystem = iWindowIndex;
	m_uiExplicitFlags |= EXPLICIT_CoordinateSystem;
}

void IHyDrawInst2d::SetDisplayOrder(int32 iOrderValue)
{
	m_iDisplayOrder = iOrderValue;
	m_uiExplicitFlags |= EXPLICIT_DisplayOrder;

	HyScene::SetInstOrderingDirty();
}

const std::string &IHyDrawInst2d::GetName() const
{
	return m_sNAME;
}

const std::string &IHyDrawInst2d::GetPrefix() const
{
	return m_sPREFIX;
}

HyRenderMode IHyDrawInst2d::GetRenderMode() const
{
	return m_eRenderMode;
}

HyTextureHandle IHyDrawInst2d::GetTextureHandle() const
{
	return m_hTextureHandle;
}

IHyNodeData *IHyDrawInst2d::AcquireData()
{
	if(m_pData == nullptr)
	{
		HyAssert(sm_pHyAssets != nullptr, "AcquireData was called before the engine has initialized HyAssets");

		sm_pHyAssets->GetNodeData(this, m_pData);
		if(m_pData)
			OnDataAcquired();
		else
			HyAssert(m_eTYPE == HYTYPE_Primitive2d, "Could not find data for: " << GetPrefix() << "/" << GetName());

		if(m_hShader == HY_UNUSED_HANDLE)
			m_hShader = Hy_DefaultShaderHandle(m_eTYPE);
	}

	return m_pData;
}

const HyShape2d &IHyDrawInst2d::GetBoundingVolume()
{
	if(IsDirty(DIRTY_BoundingVolume) || m_BoundingVolume.IsValid() == false)
	{
		CalcBoundingVolume();
		ClearDirty(DIRTY_BoundingVolume);
	}

	return m_BoundingVolume;
}

const b2AABB &IHyDrawInst2d::GetWorldAABB()
{
	if(IsDirty(DIRTY_WorldAABB))
	{
		glm::mat4 mtxWorld;
		GetWorldTransform(mtxWorld);
		float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);

		GetBoundingVolume(); // This will update BV if it's dirty
		m_BoundingVolume.GetB2Shape()->ComputeAABB(&m_aabbCached, b2Transform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians)), 0);

		ClearDirty(DIRTY_WorldAABB);
	}

	return m_aabbCached;
}

void IHyDrawInst2d::SetCustomShader(HyShader *pShader)
{
	if(pShader)
	{
		HyAssert(pShader->IsFinalized(), "IHyDrawInst2d::SetCustomShader tried to set a non-finalized shader");
		m_hShader = pShader->GetHandle();
	}
	else
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);
}

HyShaderHandle IHyDrawInst2d::GetShaderHandle()
{
	return m_hShader;
}

bool IHyDrawInst2d::SetPortal(HyPortal2d *pPortal)
{
	for(uint32 i = 0; i < HY_MAX_PORTAL_HANDLES; ++i)
	{
		if(m_hPortals[i] == pPortal->GetHandle())
			return true;
		else if(m_hPortals[i] == HY_UNUSED_HANDLE)
		{
			pPortal->AddInstance(this);
			m_hPortals[i] = pPortal->GetHandle();
			return true;
		}
	}

	HyLogWarning("IHyDrawInst2d::SetPortal() - Too many portals have been set for this instance. Max is: " << HY_MAX_PORTAL_HANDLES);
	return false;
}

bool IHyDrawInst2d::ClearPortal(HyPortal2d *pPortal)
{
	for(uint32 i = 0; i < HY_MAX_PORTAL_HANDLES; ++i)
	{
		// If found, shift all handles to the "left" so handles won't get fragmented in array.
		if(m_hPortals[i] == pPortal->GetHandle())
		{
			pPortal->RemoveInstance(this);

			if(i != HY_MAX_PORTAL_HANDLES - 1)
				memmove(&m_hPortals[i], &m_hPortals[i+1], sizeof(HyPortal2dHandle) * (HY_MAX_PORTAL_HANDLES - i+1));

			m_hPortals[HY_MAX_PORTAL_HANDLES - 1] = HY_UNUSED_HANDLE;
			return true;
		}
	}

	return false;
}

/*virtual*/ bool IHyDrawInst2d::IsLoaded() const /*override*/
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

/*virtual*/ void IHyDrawInst2d::Load() /*override*/
{
	HyAssert(sm_pHyAssets, "IHyDraw2d::Load was invoked before engine has been initialized");

	// Don't load if the name is blank, and it's required by this node type
	if(m_sNAME.empty() && m_eTYPE != HYTYPE_Entity2d && m_eTYPE != HYTYPE_Primitive2d && m_eTYPE != HYTYPE_TexturedQuad2d)
		return;

	if(m_eTYPE != HYTYPE_Entity2d)
		sm_pHyAssets->LoadNodeData(this);
}

/*virtual*/ void IHyDrawInst2d::Unload() /*override*/
{
	HyAssert(sm_pHyAssets, "IHyDraw2d::Unload was invoked before engine has been initialized");	
	sm_pHyAssets->RemoveNodeData(this);
}

/*virtual*/ void IHyDrawInst2d::NodeUpdate() /*override final*/
{
	if(IsLoaded())
	{
		// This update will set the appearance of the instance to its current state
		DrawLoadedUpdate();
		OnUpdateUniforms();
	}
}

IHyNodeData *IHyDrawInst2d::UncheckedGetData()
{
	return m_pData;
}

void IHyDrawInst2d::WriteShaderUniformBuffer(char *&pRefDataWritePos)
{
	m_ShaderUniforms.WriteUniformsBufferData(pRefDataWritePos);
}

/*virtual*/ void IHyDrawInst2d::_SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_Scissor;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Scissor))
	{
		if(m_hScissor == HY_UNUSED_HANDLE)
			m_hScissor = HY_NEW ScissorRect();

		m_hScissor->m_WorldScissorRect = worldScissorRectRef;
	}
}

/*virtual*/ void IHyDrawInst2d::_SetStencil(HyStencilHandle hHandle, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_Stencil;

	if(0 == (m_uiExplicitFlags & EXPLICIT_Stencil))
		m_hStencil = hHandle;
}

/*virtual*/ int32 IHyDrawInst2d::_SetDisplayOrder(int32 iOrderValue, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_DisplayOrder;

	if(0 == (m_uiExplicitFlags & EXPLICIT_DisplayOrder))
	{
		m_iDisplayOrder = iOrderValue;
		iOrderValue += 1;

		HyScene::SetInstOrderingDirty();
	}

	return iOrderValue;
}

/*virtual*/ void IHyDrawInst2d::_SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding) /*override*/
{
	if(bIsOverriding)
		m_uiExplicitFlags &= ~EXPLICIT_CoordinateSystem;

	if(0 == (m_uiExplicitFlags & EXPLICIT_CoordinateSystem))
		UseWindowCoordinates(iWindowIndex);
}
