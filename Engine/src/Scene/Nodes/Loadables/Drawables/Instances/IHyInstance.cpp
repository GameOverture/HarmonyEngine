/**************************************************************************
*	IHyDrawable.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance.h"
#include "HyEngine.h"

extern HyShaderHandle Hy_DefaultShaderHandle(HyType eType);

HyScene *IHyInstance::sm_pScene = nullptr;

IHyInstance::IHyInstance() :
	m_hShader(HY_UNUSED_HANDLE),
	m_eRenderMode(HYRENDERMODE_Unknown),
	m_hTextureHandle(HY_UNUSED_HANDLE)
{
}

IHyInstance::IHyInstance(const IHyInstance &copyRef) :
	m_hShader(copyRef.m_hShader),
	m_eRenderMode(copyRef.m_eRenderMode),
	m_hTextureHandle(copyRef.m_hTextureHandle),
	m_ShaderUniforms(copyRef.m_ShaderUniforms)
{
}

IHyInstance::IHyInstance(IHyInstance &&donor) :
	m_hShader(std::move(donor.m_hShader)),
	m_eRenderMode(std::move(donor.m_eRenderMode)),
	m_hTextureHandle(std::move(donor.m_hTextureHandle)),
	m_ShaderUniforms(std::move(donor.m_ShaderUniforms))
{
}

IHyInstance::~IHyInstance()
{
}

IHyInstance &IHyInstance::operator=(const IHyInstance &rhs)
{
	m_hShader = rhs.m_hShader;
	m_eRenderMode = rhs.m_eRenderMode;
	m_hTextureHandle = rhs.m_hTextureHandle;
	m_ShaderUniforms = rhs.m_ShaderUniforms;

	return *this;
}

IHyInstance &IHyInstance::operator=(IHyInstance &&donor)
{
	m_hShader = std::move(donor.m_hShader);
	m_eRenderMode = std::move(donor.m_eRenderMode);
	m_hTextureHandle = std::move(donor.m_hTextureHandle);
	m_ShaderUniforms = std::move(donor.m_ShaderUniforms);

	return *this;
}

HyRenderMode IHyInstance::GetRenderMode() const
{
	return m_eRenderMode;
}

HyTextureHandle IHyInstance::GetTextureHandle() const
{
	return m_hTextureHandle;
}

void IHyInstance::SetShader(HyShader *pShader)
{
	if(pShader)
	{
		HyAssert(pShader->IsFinalized(), "IHyInstance::SetShader tried to set a non-finalized shader");
		m_hShader = pShader->GetHandle();
	}
	else
		m_hShader = Hy_DefaultShaderHandle(_DrawableGetNodeRef().GetType());
}

HyShaderHandle IHyInstance::GetShaderHandle()
{
	return m_hShader;
}

const HyShaderUniforms &IHyInstance::GetShaderUniforms() const
{
	return m_ShaderUniforms;
}
