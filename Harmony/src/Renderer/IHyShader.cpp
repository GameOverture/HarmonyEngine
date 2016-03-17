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

IHyShader::IHyShader() : m_bLocked(false)
{
}

IHyShader::~IHyShader()
{
}

void IHyShader::SetSourceCode(const char *szSource, HyShaderType eType)
{
	HyAssert(m_bLocked == false, "HyShader::SetSourceCode() was invoked on a locked shader");
	m_sSourceCode[eType] = szSource;
}

void IHyShader::Lock()
{
	m_bLocked = true;
}
