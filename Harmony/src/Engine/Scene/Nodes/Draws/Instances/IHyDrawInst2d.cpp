/**************************************************************************
*	IHyDrawInst2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Draws/Instances/IHyDrawInst2d.h"
#include "HyEngine.h"

/*static*/ HyAssets *IHyDrawInst2d::sm_pHyAssets = nullptr;

IHyDrawInst2d::IHyDrawInst2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyDraw2d(eNodeType, pParent),
																												m_eLoadState(HYLOADSTATE_Inactive),
																												m_pData(nullptr),
																												m_sName(szName ? szName : ""),
																												m_sPrefix(szPrefix ? szPrefix : ""),
																												m_hShader(HY_UNUSED_HANDLE),
																												m_eRenderMode(HYRENDERMODE_Unknown),
																												m_hTextureHandle(HY_UNUSED_HANDLE),
																												m_LocalBoundingVolume(this)
{
}

IHyDrawInst2d::IHyDrawInst2d(const IHyDrawInst2d &copyRef) :	IHyDraw2d(copyRef),
																m_eLoadState(HYLOADSTATE_Inactive),
																m_pData(nullptr),
																m_sName(copyRef.m_sName),
																m_sPrefix(copyRef.m_sPrefix),
																m_hShader(copyRef.m_hShader),
																m_eRenderMode(copyRef.m_eRenderMode),
																m_hTextureHandle(copyRef.m_hTextureHandle),
																m_ShaderUniforms(copyRef.m_ShaderUniforms),
																m_LocalBoundingVolume(this, copyRef.m_LocalBoundingVolume)
{
}

IHyDrawInst2d::~IHyDrawInst2d()
{
	if(m_eLoadState != HYLOADSTATE_Inactive)
		Unload();
}

const IHyDrawInst2d &IHyDrawInst2d::operator=(const IHyDrawInst2d &rhs)
{
	IHyDraw2d::operator=(rhs);

	if(m_sPrefix != rhs.m_sPrefix || m_sName != rhs.m_sName)
	{
		if(m_eLoadState != HYLOADSTATE_Inactive)
			Unload();

		m_sPrefix = rhs.m_sPrefix;
		m_sName = rhs.m_sName;
		m_pData = nullptr;			// Ensures virtual OnDataAcquired() is invoked when the below Load() is invoked
	}

	m_hShader = rhs.m_hShader;
	m_eRenderMode = rhs.m_eRenderMode;
	m_hTextureHandle = rhs.m_hTextureHandle;
	m_ShaderUniforms = m_ShaderUniforms;
	
	m_LocalBoundingVolume = rhs.m_LocalBoundingVolume;
	m_aabbCached = rhs.m_aabbCached;

	if(rhs.IsLoaded())
		Load();

	return *this;
}

bool IHyDrawInst2d::IsValid()
{
	return m_bEnabled && OnIsValid();
}

const std::string &IHyDrawInst2d::GetName() const
{
	return m_sName;
}

const std::string &IHyDrawInst2d::GetPrefix() const
{
	return m_sPrefix;
}

HyRenderMode IHyDrawInst2d::GetRenderMode() const
{
	return m_eRenderMode;
}

HyTextureHandle IHyDrawInst2d::GetTextureHandle() const
{
	return m_hTextureHandle;
}

const IHyNodeData *IHyDrawInst2d::AcquireData()
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

const HyShape2d &IHyDrawInst2d::GetLocalBoundingVolume()
{
	if(IsDirty(DIRTY_BoundingVolume) || m_LocalBoundingVolume.IsValid() == false)
	{
		CalcBoundingVolume();
		ClearDirty(DIRTY_BoundingVolume);
	}

	return m_LocalBoundingVolume;
}

/*virtual*/ const b2AABB &IHyDrawInst2d::GetWorldAABB() /*override*/
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

void IHyDrawInst2d::SetShader(HyShader *pShader)
{
	if(pShader)
	{
		HyAssert(pShader->IsFinalized(), "IHyDrawInst2d::SetShader tried to set a non-finalized shader");
		m_hShader = pShader->GetHandle();
	}
	else
		m_hShader = Hy_DefaultShaderHandle(m_eTYPE);
}

HyShaderHandle IHyDrawInst2d::GetShaderHandle()
{
	return m_hShader;
}

/*virtual*/ bool IHyDrawInst2d::IsLoaded() const /*override*/
{
	return m_eLoadState == HYLOADSTATE_Loaded;
}

/*virtual*/ void IHyDrawInst2d::Load() /*override final*/
{
	HyAssert(sm_pHyAssets, "IHyDraw2d::Load was invoked before engine has been initialized");

	// Don't load if the name is blank, and it's required by this node type
	if(m_sName.empty() && m_eTYPE != HYTYPE_Entity2d && m_eTYPE != HYTYPE_Primitive2d && m_eTYPE != HYTYPE_TexturedQuad2d)
		return;

	if(m_eTYPE != HYTYPE_Entity2d)
		sm_pHyAssets->LoadNodeData(this);
}

/*virtual*/ void IHyDrawInst2d::Unload() /*override final*/
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

const IHyNodeData *IHyDrawInst2d::UncheckedGetData()
{
	return m_pData;
}

void IHyDrawInst2d::WriteShaderUniformBuffer(char *&pRefDataWritePos)
{
	m_ShaderUniforms.WriteUniformsBufferData(pRefDataWritePos);
}
