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

HyText2d::HyText2d(const char *szPrefix, const char *szName) :	IHyInst2d(HYINST_Text2d, szPrefix, szName),
																m_bIsDirty(true),
																m_sString("<not set>"),
																m_uiCurFontState(0),
																m_eAlignment(HYALIGN_Left),
																m_rBox(0.0f, 0.0f, 0.0f, 0.0f),
																m_pVertexBuffer(NULL),
																m_uiBufferSizeBytes(0)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumVerticesPerInstance(4);
}

HyText2d::~HyText2d(void)
{
}

void HyText2d::TextSet(std::string sText)
{
	if(sText == m_sString)
		return;

	m_sString = sText;
	m_bIsDirty = true;
}

void HyText2d::TextSet(const char *szString, ...)
{
	//va_list vl;
	//va_start(vl, szString);
	////#ifdef HY_UNICODE
	////	vswprintf(sm_pTempTextBuffer, HY_TEMP_TEXTBUFFER_SIZE, szString, vl);
	////#else
	//vsprintf(sm_pTempTextBuffer, szString, vl);
	////#endif
	//va_end(vl);

	//if(m_sString == sm_pTempTextBuffer)
	//	return;
	//m_sString = sm_pTempTextBuffer;

	//CalcVertexBuffer();
}

std::string HyText2d::TextGet()
{
	return m_sString;
}

uint32 HyText2d::TextGetLength()
{
	return static_cast<uint32>(m_sString.size());
}

uint32 HyText2d::TextGetState()
{
	return m_uiCurFontState;
}

void HyText2d::TextSetState(uint32 uiStateIndex)
{
	m_uiCurFontState = uiStateIndex;
}

HyAlign HyText2d::TextGetAlignment()
{
	return m_eAlignment;
}

void HyText2d::TextSetAlignment(HyAlign eAlignment)
{
	m_eAlignment = eAlignment;
}

HyRectangle<float> HyText2d::TextGetBox()
{
	return m_rBox;
}

void HyText2d::TextSetBox(HyRectangle<float> rBox, bool bCenterVertically /*= false*/, bool bScaleDownToFit /*= false*/, bool bScaleUpToFit /*= false*/)
{
	m_rBox = rBox;
	
	m_rBox.iTag = BOXATTRIB_IsUsed;
	if(bCenterVertically)
		m_rBox.iTag |= BOXATTRIB_CenterVertically;
	if(bScaleDownToFit)
		m_rBox.iTag |= BOXATTRIB_ScaleDown;
	if(bScaleUpToFit)
		m_rBox.iTag |= BOXATTRIB_ScaleUp;
}

void HyText2d::TextClearBox()
{
	m_rBox.iTag &= ~BOXATTRIB_IsUsed;
}

/*virtual*/ void HyText2d::OnDataLoaded()
{
	HyText2dData *pTextData = reinterpret_cast<HyText2dData *>(m_pData);
}

/*virtual*/ void HyText2d::OnInstUpdate()
{
	if(m_bIsDirty == false)
		return;



	m_bIsDirty = false;
}

/*virtual*/ void HyText2d::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
}

/*virtual*/ void HyText2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{

}
