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

#include <vector>

class IHyRenderer;

class IHyShader
{
public:
	enum eShaderProgram
	{
		SHADER_QuadBatch = 0,
		SHADER_Primitive,

		SHADER_CustomStartIndex
	};
protected:

	struct VertexAttribute
	{
		std::string			sName;
		HyShaderVariable	eVarType;
		bool				bNormalized;
		uint32				uiInstanceDivisor;
	};

	const uint32												m_uiINDEX;

	HyLoadState													m_eLoadState;
	std::string													m_sSourceCode[HYNUMSHADERTYPES];
	std::vector<VertexAttribute>								m_vVertexAttributes;

	IHyShader(uint32 iIndex);
public:
	virtual ~IHyShader();

	void SetSourceCode(const char *szSource, HyShaderType eType);
	void SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);

	void Lock();

	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
};

#endif /* __IHyShader_h__ */
