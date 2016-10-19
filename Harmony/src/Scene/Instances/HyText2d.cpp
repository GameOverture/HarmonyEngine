/**************************************************************************
 *	HyText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Instances/HyText2d.h"

#include "Utilities/HyStrManip.h"

#include <iostream>

/*static*/ char HyText2d::sm_pTempTextBuffer[HY_TEMP_TEXTBUFFER_SIZE] = { 0 };

HyText2d::HyText2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYINST_Text2d, szPrefix, szName),
																m_pVertexBuffer(NULL),
																m_uiCurFontIndex(0)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumVertices(4);
}

HyText2d::~HyText2d(void)
{
}

void HyText2d::SetString(const char *szString, ...)
{
	va_list vl;
	va_start(vl, szString);
//#ifdef HY_UNICODE
//	vswprintf(sm_pTempTextBuffer, HY_TEMP_TEXTBUFFER_SIZE, szString, vl);
//#else
	vsprintf(sm_pTempTextBuffer, szString, vl);
//#endif
	va_end(vl);

	if(m_sString == sm_pTempTextBuffer)
		return;
	m_sString = sm_pTempTextBuffer;

	CalcVertexBuffer();
}

void HyText2d::CalcVertexBuffer()
{
}

/*virtual*/ void HyText2d::OnDataLoaded()
{
	HyText2dData *pTextData = reinterpret_cast<HyText2dData *>(m_pData);
	//m_uiCurFontSize = pTextData->GetMinFontSize();

	CalcVertexBuffer();
}

/*virtual*/ void HyText2d::OnInstUpdate()
{

}

/*virtual*/ void HyText2d::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
}

/*virtual*/ void HyText2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{

}
