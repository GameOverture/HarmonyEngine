/**************************************************************************
*	HyShader.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
**************************************************************************/
#include "Renderer/Components/HyShader.h"
#include "Renderer/IHyRenderer.h"

/*static*/ HyShaderHandle HyShader::sm_hHandleCount = 0;
/*static*/ IHyRenderer *HyShader::sm_pRenderer = nullptr;

HyShader::HyShader() :	m_hHANDLE(++sm_hHandleCount),
						m_bIsFinalized(false)
{
	for(int i = 0; i < HYNUMSHADERTYPES; ++i)
		m_sSourceCode[i].clear();

	IHyRenderer::AddShader(this);
}

HyShader::~HyShader()
{
	IHyRenderer::RemoveShader(this);
}

HyShaderHandle HyShader::GetHandle()
{
	return m_hHANDLE;
}

bool HyShader::IsFinalized()
{
	return m_bIsFinalized;
}

const std::string &HyShader::GetSourceCode(HyShaderType eType)
{
	return m_sSourceCode[eType];
}

void HyShader::SetSourceCode(std::string sSource, HyShaderType eType)
{
	if(sSource.empty())
		return;

	HyAssert(m_bIsFinalized == false, "HyShader::SetSourceCode() was invoked on a locked shader");
	m_sSourceCode[eType] = sSource;
}

/*virtual*/ void HyShader::AddVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize /*= false*/, uint32 uiInstanceDivisor /*= 0*/)
{
	if(szName == nullptr)
		return;

	HyAssert(m_bIsFinalized == false, "HyShader::AddVertexAttribute() was invoked on a locked shader");

	VertexAttribute vertAttrib;
	vertAttrib.sName = szName;
	vertAttrib.eVarType = eVarType;
	vertAttrib.bNormalized = bNormalize;
	vertAttrib.uiInstanceDivisor = uiInstanceDivisor;

	m_VertexAttributeList.push_back(vertAttrib);
}

void HyShader::ClearVertextAttributes()
{
	m_VertexAttributeList.clear();
}

void HyShader::Finalize(HyShaderProgram eDefaultsFrom)
{
	HyAssert(sm_pRenderer, "HyShader::Finalize was invoked before the renderer has been instanciated");

	sm_pRenderer->UploadShader(eDefaultsFrom, this);
	m_bIsFinalized = true;
}

//void HyShader::OnRenderThread(IHyRenderer &rendererRef)
//{
//	// Load state can be HYLOADSTATE_Inactive if it's a default shader being loaded by the Renderer
//	if(GetLoadableState() == HYLOADSTATE_Inactive)
//	{
//		OnUpload(rendererRef);
//		return;
//	}
//
//	if(GetLoadableState() == HYLOADSTATE_Queued)
//		OnUpload(rendererRef);
//	else
//		OnDelete(rendererRef);
//}
