/**************************************************************************
*	IHyDrawable.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable.h"
#include "HyEngine.h"

HyScene *IHyDrawable::sm_pScene = nullptr;

IHyDrawable::IHyDrawable() :	m_hShader(HY_UNUSED_HANDLE),
								m_eRenderMode(HYRENDERMODE_Unknown),
								m_hTextureHandle(HY_UNUSED_HANDLE)
{
}

IHyDrawable::IHyDrawable(const IHyDrawable &copyRef) :	m_hShader(copyRef.m_hShader),
														m_eRenderMode(copyRef.m_eRenderMode),
														m_hTextureHandle(copyRef.m_hTextureHandle),
														m_ShaderUniforms(copyRef.m_ShaderUniforms)
{
}

IHyDrawable::~IHyDrawable()
{
}

const IHyDrawable &IHyDrawable::operator=(const IHyDrawable &rhs)
{
	m_hShader = rhs.m_hShader;
	m_eRenderMode = rhs.m_eRenderMode;
	m_hTextureHandle = rhs.m_hTextureHandle;
	m_ShaderUniforms = m_ShaderUniforms;

	return *this;
}

HyRenderMode IHyDrawable::GetRenderMode() const
{
	return m_eRenderMode;
}

HyTextureHandle IHyDrawable::GetTextureHandle() const
{
	return m_hTextureHandle;
}

void IHyDrawable::SetShader(HyShader *pShader)
{
	if(pShader)
	{
		HyAssert(pShader->IsFinalized(), "IHyDrawable::SetShader tried to set a non-finalized shader");
		m_hShader = pShader->GetHandle();
	}
	else
		m_hShader = Hy_DefaultShaderHandle(_DrawableGetType());
}

HyShaderHandle IHyDrawable::GetShaderHandle()
{
	return m_hShader;
}

void IHyDrawable::WriteShaderUniformBuffer(uint8 *&pWritePositionRef)
{
	m_ShaderUniforms.WriteUniformsBufferData(pWritePositionRef);
}
