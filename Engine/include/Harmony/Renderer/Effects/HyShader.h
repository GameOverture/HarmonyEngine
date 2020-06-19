/**************************************************************************
*	HyShader.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
**************************************************************************/
#ifndef HyShader_h__
#define HyShader_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/IHyFile.h"

struct HyShaderVertexAttribute
{
	std::string								sName;
	HyShaderVariable						eVarType;
	bool									bNormalized;
	uint32									uiInstanceDivisor;
};

class HyShader : public IHyFile
{
	friend class IHyRenderer;

	static HyShaderHandle					sm_hHandleCount;
	const HyShaderHandle					m_hHANDLE;
	const HyShaderProgramDefaults			m_eDEFAULTS_FROM;

	bool									m_bIsFinalized;
	std::string								m_sSourceCode[HYNUM_SHADERTYPES];

	std::vector<HyShaderVertexAttribute>	m_VertexAttributeList;
	size_t									m_uiStride;

public:
	HyShader(HyShaderProgramDefaults eDefaultsFrom);
private:
	virtual ~HyShader();
public:
	void Destroy();

	HyShaderHandle GetHandle();
	bool IsFinalized();
	int32 GetStride();

	HyShaderProgramDefaults GetDefaults() const;

	const std::string &GetSourceCode(HyShaderType eType);
	void SetSourceCode(std::string sSource, HyShaderType eType);

	void AddVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);
	std::vector<HyShaderVertexAttribute> &GetVertextAttributes();

	void Finalize();

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

#endif /* HyShader_h__ */
