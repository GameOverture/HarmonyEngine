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

IHyShader::IHyShader()
{
}

IHyShader::~IHyShader()
{
}

void IHyShader::SetSourceCode(const char *szSource, HyShaderType eType)
{
	m_sSourceCode[eType] = szSource;
}
