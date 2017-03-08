/**************************************************************************
*	IHyShader.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
**************************************************************************/
#ifndef __IHyShader_h__
#define __IHyShader_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Loadables/IHyLoadableData.h"

class IHyShader : public IHyLoadableData
{
protected:
	const int32						m_iID;
	const std::string				m_sOPTIONAL_LOAD_PATH;

	bool							m_bIsFinalized;

	struct VertexAttribute
	{
		std::string			sName;
		HyShaderVariable	eVarType;
		bool				bNormalized;
		uint32				uiInstanceDivisor;
	};

	std::string						m_sSourceCode[HYNUMSHADERTYPES];
	std::vector<VertexAttribute>	m_VertexAttributeList;

public:
	IHyShader(int32 iId);
	IHyShader(int32 iId, std::string sPrefix, std::string sName);

	virtual ~IHyShader();

	int32 GetId();
	bool IsFinalized();

	void SetSourceCode(std::string sSource, HyShaderType eType);
	void SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize = false, uint32 uiInstanceDivisor = 0);

	void Finalize(HyShaderProgram eDefaultsFrom);

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;

	virtual void OnSetVertexAttribute(const char *szName, uint32 uiLocation) = 0;
	virtual void OnUpload(IHyRenderer &rendererRef) = 0;
	virtual void OnDelete(IHyRenderer &rendererRef) = 0;
};

#endif /* __IHyShader_h__ */
