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
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable.h"
#include "HyEngine.h"

IHyDrawable::IHyDrawable() :
	m_hShader(HY_UNUSED_HANDLE)
{
}

IHyDrawable::IHyDrawable(const IHyDrawable &copyRef) :
	m_hShader(copyRef.m_hShader),
	m_ShaderUniforms(copyRef.m_ShaderUniforms)
{
}

IHyDrawable::IHyDrawable(IHyDrawable &&donor) noexcept :
	m_hShader(std::move(donor.m_hShader)),
	m_ShaderUniforms(std::move(donor.m_ShaderUniforms))
{
}

IHyDrawable::~IHyDrawable()
{
}

IHyDrawable &IHyDrawable::operator=(const IHyDrawable &rhs)
{
	m_hShader = rhs.m_hShader;
	m_ShaderUniforms = rhs.m_ShaderUniforms;

	return *this;
}

IHyDrawable &IHyDrawable::operator=(IHyDrawable &&donor) noexcept
{
	m_hShader = std::move(donor.m_hShader);
	m_ShaderUniforms = std::move(donor.m_ShaderUniforms);

	return *this;
}

void IHyDrawable::SetShader(HyShader *pShader)
{
	if(pShader)
	{
		HyAssert(pShader->IsFinalized(), "IHyDrawable::SetShader tried to set a non-finalized shader");
		m_hShader = pShader->GetHandle();
	}
	else
		m_hShader = HyEngine::DefaultShaderHandle(_DrawableGetNodeRef().GetType());
}

HyShaderHandle IHyDrawable::GetShaderHandle()
{
	return m_hShader;
}

HyShaderUniforms &IHyDrawable::GetShaderUniforms()
{
	return m_ShaderUniforms;
}
