/**************************************************************************
*	IHyShader.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyShader_h__
#define __IHyShader_h__

#include "Afx/HyStdAfx.h"

class IHyShader
{
protected:
	bool				m_bLocked;
	std::string			m_sSourceCode[HYNUMSHADERS];

	IHyShader();
public:
	virtual ~IHyShader();

	void SetSourceCode(const char *szSource, HyShaderType eType);

	void Lock();
};

#endif /* __IHyShader_h__ */
