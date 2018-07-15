/**************************************************************************
*	IHyDrawable3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable3d.h"
#include "HyEngine.h"

HyScene *IHyDrawable3d::sm_pScene = nullptr;

IHyDrawable3d::IHyDrawable3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent) :	IHyLoadable3d(eNodeType, szPrefix, szName, pParent),
																												m_hShader(HY_UNUSED_HANDLE),
																												m_eRenderMode(HYRENDERMODE_Unknown),
																												m_hTextureHandle(HY_UNUSED_HANDLE)
{
}

IHyDrawable3d::IHyDrawable3d(const IHyDrawable3d &copyRef) :	IHyLoadable3d(copyRef),
																m_hShader(copyRef.m_hShader),
																m_eRenderMode(copyRef.m_eRenderMode),
																m_hTextureHandle(copyRef.m_hTextureHandle),
																m_ShaderUniforms(copyRef.m_ShaderUniforms)
{
}

IHyDrawable3d::~IHyDrawable3d()
{
	if(m_eLoadState != HYLOADSTATE_Inactive)
		Unload();
}

const IHyDrawable3d &IHyDrawable3d::operator=(const IHyDrawable3d &rhs)
{
	IHyLoadable3d::operator=(rhs);

	m_hShader = rhs.m_hShader;
	m_eRenderMode = rhs.m_eRenderMode;
	m_hTextureHandle = rhs.m_hTextureHandle;
	m_ShaderUniforms = m_ShaderUniforms;
	
	//m_aabbCached = rhs.m_aabbCached;

	return *this;
}

bool IHyDrawable3d::IsValid()
{
	return m_bEnabled && OnIsValid();
}

HyRenderMode IHyDrawable3d::GetRenderMode() const
{
	return m_eRenderMode;
}

HyTextureHandle IHyDrawable3d::GetTextureHandle() const
{
	return m_hTextureHandle;
}

//const HyShape2d &IHyDrawable3d::GetLocalBoundingVolume()
//{
//	if(IsDirty(DIRTY_BoundingVolume) || m_LocalBoundingVolume.IsValid() == false)
//	{
//		CalcBoundingVolume();
//		ClearDirty(DIRTY_BoundingVolume);
//	}
//
//	return m_LocalBoundingVolume;
//}
//
///*virtual*/ const b2AABB &IHyDrawable3d::GetWorldAABB() /*override*/
//{
//	if(IsDirty(DIRTY_WorldAABB))
//	{
//		glm::mat4 mtxWorld;
//		GetWorldTransform(mtxWorld);
//		float fWorldRotationRadians = glm::atan(mtxWorld[0][1], mtxWorld[0][0]);
//
//		GetLocalBoundingVolume(); // This will update BV if it's dirty
//		m_LocalBoundingVolume.GetB2Shape()->ComputeAABB(&m_aabbCached, b2Transform(b2Vec2(mtxWorld[3].x, mtxWorld[3].y), b2Rot(fWorldRotationRadians)), 0);
//
//		ClearDirty(DIRTY_WorldAABB);
//	}
//
//	return m_aabbCached;
//}

void IHyDrawable3d::SetShader(HyShader *pShader)
{
	if(pShader)
	{
		HyAssert(pShader->IsFinalized(), "IHyDrawable3d::SetShader tried to set a non-finalized shader");
		m_hShader = pShader->GetHandle();
	}
	else
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);
}

HyShaderHandle IHyDrawable3d::GetShaderHandle()
{
	return m_hShader;
}

/*virtual*/ void IHyDrawable3d::NodeUpdate() /*override final*/
{
	if(IsLoaded())
	{
		// This update will set the appearance of the instance to its current state
		DrawLoadedUpdate();
		OnUpdateUniforms();
	}
}

void IHyDrawable3d::WriteShaderUniformBuffer(char *&pRefDataWritePos)
{
	m_ShaderUniforms.WriteUniformsBufferData(pRefDataWritePos);
}

/*virtual*/ void IHyDrawable3d::OnLoaded() /*override*/
{
	if(m_hShader == HY_UNUSED_HANDLE)
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);

	sm_pScene->AddNode_Loaded(this);
}

/*virtual*/ void IHyDrawable3d::OnUnloaded() /*override*/
{
	sm_pScene->RemoveNode_Loaded(this);
}
