/**************************************************************************
*	HyShader.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
**************************************************************************/
#ifndef HyShader_h__
#define HyShader_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/IHyLoadableData.h"

struct HyShaderVertexAttribute
{
	std::string					sName;
	HyShaderVariable			eVarType;
	bool						bNormalized;
	uint32						uiInstanceDivisor;
};

class HyShader
{
	friend class IHyRenderer;

	static HyShaderHandle					sm_hHandleCount;
	static IHyRenderer *					sm_pRenderer;

	const HyShaderHandle					m_hHANDLE;

	bool									m_bIsFinalized;
	std::string								m_sSourceCode[HYNUMSHADERTYPES];

	std::vector<HyShaderVertexAttribute>	m_VertexAttributeList;
	size_t									m_uiStride;

public:
	HyShader();
private: ~HyShader();
public:
	void Destroy();

	HyShaderHandle GetHandle();
	bool IsFinalized();
	int32 GetStride();

	const std::string &GetSourceCode(HyShaderType eType);
	void SetSourceCode(std::string sSource, HyShaderType eType);

	void AddVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);
	std::vector<HyShaderVertexAttribute> &GetVertextAttributes();

	void Finalize(HyShaderProgramDefaults eDefaultsFrom);
};

#endif /* HyShader_h__ */
