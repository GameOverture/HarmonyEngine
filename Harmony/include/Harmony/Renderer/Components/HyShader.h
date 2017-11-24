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

class HyShader
{
	friend class IHyRenderer;

	static HyShaderHandle			sm_hHandleCount;
	static IHyRenderer *			sm_pRenderer;

protected:
	const HyShaderHandle			m_hHANDLE;

	bool							m_bIsFinalized;
	std::string						m_sSourceCode[HYNUMSHADERTYPES];

	struct VertexAttribute
	{
		std::string					sName;
		HyShaderVariable			eVarType;
		bool						bNormalized;
		uint32						uiInstanceDivisor;
	};
	std::vector<VertexAttribute>	m_VertexAttributeList;

public:
	HyShader();

	virtual ~HyShader();

	HyShaderHandle GetHandle();
	bool IsFinalized();

	const std::string &GetSourceCode(HyShaderType eType);
	void SetSourceCode(std::string sSource, HyShaderType eType);

	void AddVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);
	void ClearVertextAttributes();

	void Finalize(HyShaderProgram eDefaultsFrom);

	//virtual void OnRenderThread(IHyRenderer &rendererRef) override; // ::IHyLoadableData

//protected:
//	virtual void OnUpload(IHyRenderer &rendererRef) = 0;
//	virtual void OnDelete(IHyRenderer &rendererRef) = 0;
};

#endif /* HyShader_h__ */
