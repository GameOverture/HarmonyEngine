/**************************************************************************
*	IHyShader.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
**************************************************************************/
#include "Assets/Loadables/IHyShader.h"
#include "Utilities/HyStrManip.h"
#include "Renderer/OpenGL/HyOpenGLShaderSrc.h"

IHyShader::IHyShader(int32 iId) :	IHyLoadableData(HYGFXTYPE_CustomShader),
									m_iID(iId),
									m_sOPTIONAL_LOAD_PATH(""),
									m_bIsFinalized(false)
{
	for(int i = 0; i < HYNUMSHADERTYPES; ++i)
		m_sSourceCode[i].clear();
}

IHyShader::IHyShader(int32 iId, std::string sPrefix, std::string sName) :	IHyLoadableData(HYGFXTYPE_CustomShader),
																			m_iID(iId),
																			m_sOPTIONAL_LOAD_PATH(MakeStringProperPath(std::string(sPrefix + "/" + sName).c_str(), ".hyglsl", false))
{
	for(int i = 0; i < HYNUMSHADERTYPES; ++i)
		m_sSourceCode[i].clear();
}

IHyShader::~IHyShader()
{
}

int32 IHyShader::GetId()
{
	return m_iID;
}

bool IHyShader::IsFinalized()
{
	return m_bIsFinalized;
}

void IHyShader::SetSourceCode(std::string sSource, HyShaderType eType)
{
	if(sSource.empty())
		return;

	HyAssert(m_bIsFinalized == false, "HyShader::SetSourceCode() was invoked on a locked shader");
	m_sSourceCode[eType] = sSource;
}

/*virtual*/ void IHyShader::SetVertexAttribute(const char *szName, HyShaderVariable eVarType, bool bNormalize /*= false*/, uint32 uiInstanceDivisor /*= 0*/)
{
	if(szName == NULL)
		return;

	HyAssert(m_bIsFinalized == false, "HyShader::SetVertexAttribute() was invoked on a locked shader");

	VertexAttribute vertAttrib;
	vertAttrib.sName = szName;
	vertAttrib.eVarType = eVarType;
	vertAttrib.bNormalized = bNormalize;
	vertAttrib.uiInstanceDivisor = uiInstanceDivisor;

	m_VertexAttributeList.push_back(vertAttrib);

	// TODO: Handle vertex attribs that take multiple locations
	//OnSetVertexAttribute(szName, m_VertexAttributeList.size() - 1);
}

void IHyShader::Finalize(HyShaderProgram eDefaultsFrom)
{
	// If unassigned vertex shader, fill in defaults
	if(m_sSourceCode[HYSHADER_Vertex].empty())
	{
		m_VertexAttributeList.clear();

		switch(eDefaultsFrom)
		{
		case HYSHADERPROG_QuadBatch:
			SetSourceCode(szHYQUADBATCH_VERTEXSHADER, HYSHADER_Vertex);
			SetVertexAttribute("size", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("offset", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("topTint", HYSHADERVAR_vec4, false, 1);
			SetVertexAttribute("botTint", HYSHADERVAR_vec4, false, 1);
			SetVertexAttribute("UVcoord0", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("UVcoord1", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("UVcoord2", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("UVcoord3", HYSHADERVAR_vec2, false, 1);
			SetVertexAttribute("mtxLocalToWorld", HYSHADERVAR_mat4, false, 1);
			break;

		case HYSHADERPROG_Primitive:
			SetSourceCode(szHYPRIMATIVE_VERTEXSHADER, HYSHADER_Vertex);
			SetVertexAttribute("a_vPosition", HYSHADERVAR_vec2);
			break;

		case HYSHADERPROG_Lines2d:
			SetSourceCode(szHYLINES2D_VERTEXSHADER, HYSHADER_Vertex);
			SetVertexAttribute("a_vPosition", HYSHADERVAR_vec2);
			SetVertexAttribute("a_vNormal", HYSHADERVAR_vec2);
			break;
		}
	}

	// If unassigned fragment shader, fill in defaults
	if(m_sSourceCode[HYSHADER_Fragment].empty())
	{
		switch(eDefaultsFrom)
		{
		case HYSHADERPROG_QuadBatch:
			SetSourceCode(szHYQUADBATCH_FRAGMENTSHADER, HYSHADER_Fragment);
			break;

		case HYSHADERPROG_Primitive:
			SetSourceCode(szHYPRIMATIVE_FRAGMENTSHADER, HYSHADER_Fragment);
			break;

		case HYSHADERPROG_Lines2d:
			SetSourceCode(szHYLINES2D_FRAGMENTSHADER, HYSHADER_Fragment);
			break;
		}
	}

	m_bIsFinalized = true;
}

void IHyShader::OnLoadThread()
{
	HyAssert(m_bIsFinalized, "IHyShader::OnLoadThread processed an non-finalized shader");

	if(m_sOPTIONAL_LOAD_PATH.empty())
		return;

	HyError("IHyShader::OnLoadThread not implemented. Need to parse hy shader file and get setup shader source and vertex attribs");
}

void IHyShader::OnRenderThread(IHyRenderer &rendererRef)
{
	// Load state can be HYLOADSTATE_Inactive if it's a default shader being loaded by the Renderer
	if(GetLoadableState() == HYLOADSTATE_Inactive)
	{
		OnUpload(rendererRef);
		return;
	}

	if(GetLoadableState() == HYLOADSTATE_Queued)
		OnUpload(rendererRef);
	else
		OnDelete(rendererRef);
}
