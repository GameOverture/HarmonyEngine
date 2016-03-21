/**************************************************************************
*	IHyShader.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Renderer/IHyShader.h"

IHyShader::IHyShader(uint32 iIndex) :	m_uiINDEX(iIndex),
										m_eLoadState(HYLOADSTATE_Inactive)
{
}

IHyShader::~IHyShader()
{
}

void IHyShader::SetSourceCode(const char *szSource, HyShaderType eType)
{
	if(szSource == NULL)
		return;

	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "HyShader::SetSourceCode() was invoked on a locked shader");
	m_sSourceCode[eType] = szSource;
}

void IHyShader::SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize /*= false*/, uint32 uiInstanceDivisor /*= 0*/)
{
	if(szName == NULL)
		return;

	HyAssert(m_eLoadState == HYLOADSTATE_Inactive, "HyShader::SetVertexAttribute() was invoked on a locked shader");

	VertexAttribute vertAttrib;
	vertAttrib.sName = szName;
	vertAttrib.eVarType = eVarType;
	vertAttrib.bNormalized = bNormalize;
	vertAttrib.uiInstanceDivisor = uiInstanceDivisor;

	m_vVertexAttributes.push_back(vertAttrib);
}

void IHyShader::Lock()
{
	m_eLoadState = HYLOADSTATE_Queued;
}
