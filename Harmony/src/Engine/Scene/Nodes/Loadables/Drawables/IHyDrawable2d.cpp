/**************************************************************************
*	IHyDrawable2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable2d.h"
#include "HyEngine.h"

HyScene *IHyDrawable2d::sm_pScene = nullptr;

IHyDrawable2d::IHyDrawable2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyLoadable2d(eNodeType, szPrefix, szName, pParent),
																												m_hShader(HY_UNUSED_HANDLE),
																												m_eRenderMode(HYRENDERMODE_Unknown),
																												m_hTextureHandle(HY_UNUSED_HANDLE),
																												m_LocalBoundingVolume(this)
{
}

IHyDrawable2d::IHyDrawable2d(const IHyDrawable2d &copyRef) :	IHyLoadable2d(copyRef),
																m_hShader(copyRef.m_hShader),
																m_eRenderMode(copyRef.m_eRenderMode),
																m_hTextureHandle(copyRef.m_hTextureHandle),
																m_ShaderUniforms(copyRef.m_ShaderUniforms),
																m_LocalBoundingVolume(this, copyRef.m_LocalBoundingVolume)
{
}

IHyDrawable2d::~IHyDrawable2d()
{
	if(m_eLoadState != HYLOADSTATE_Inactive)
		Unload();
}

const IHyDrawable2d &IHyDrawable2d::operator=(const IHyDrawable2d &rhs)
{
	IHyLoadable2d::operator=(rhs);

	m_hShader = rhs.m_hShader;
	m_eRenderMode = rhs.m_eRenderMode;
	m_hTextureHandle = rhs.m_hTextureHandle;
	m_ShaderUniforms = m_ShaderUniforms;
	
	m_LocalBoundingVolume = rhs.m_LocalBoundingVolume;
	m_aabbCached = rhs.m_aabbCached;

	return *this;
}

bool IHyDrawable2d::IsValid()
{
	return m_bEnabled && OnIsValid();
}

HyRenderMode IHyDrawable2d::GetRenderMode() const
{
	return m_eRenderMode;
}

HyTextureHandle IHyDrawable2d::GetTextureHandle() const
{
	return m_hTextureHandle;
}

const HyShape2d &IHyDrawable2d::GetLocalBoundingVolume()
{
	if(IsDirty(DIRTY_BoundingVolume) || m_LocalBoundingVolume.IsValid() == false)
	{
		CalcBoundingVolume();
		ClearDirty(DIRTY_BoundingVolume);
	}

	return m_LocalBoundingVolume;
}

/*virtual*/ const b2AABB &IHyDrawable2d::GetWorldAABB() /*override*/
{
	if(IsDirty(DIRTY_WorldAABB))
	{
		glm::mat4 mtxWorld;
		GetWorldTransform(mtxWorld);
		float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);

		GetLocalBoundingVolume(); // This will update BV if it's dirty
		m_LocalBoundingVolume.GetB2Shape()->ComputeAABB(&m_aabbCached, b2Transform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians)), 0);

		ClearDirty(DIRTY_WorldAABB);
	}

	return m_aabbCached;
}

void IHyDrawable2d::SetShader(HyShader *pShader)
{
	if(pShader)
	{
		HyAssert(pShader->IsFinalized(), "IHyDrawable2d::SetShader tried to set a non-finalized shader");
		m_hShader = pShader->GetHandle();
	}
	else
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);
}

HyShaderHandle IHyDrawable2d::GetShaderHandle()
{
	return m_hShader;
}

/*virtual*/ void IHyDrawable2d::NodeUpdate() /*override final*/
{
	if(IsLoaded())
	{
		// This update will set the appearance of the instance to its current state
		DrawLoadedUpdate();
		OnUpdateUniforms();
	}
}

void IHyDrawable2d::WriteShaderUniformBuffer(char *&pRefDataWritePos)
{
	m_ShaderUniforms.WriteUniformsBufferData(pRefDataWritePos);
}

/*virtual*/ void IHyDrawable2d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);

	sm_pScene->AddNode_Loaded(this);
}

/*virtual*/ void IHyDrawable2d::OnUnloaded() /*override*/
{
	sm_pScene->RemoveNode_Loaded(this);
}
