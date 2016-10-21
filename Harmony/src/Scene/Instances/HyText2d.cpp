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
																m_rBox(0.0f, 0.0f, 0.0f, 0.0f)
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
	m_bIsDirty = true;
}

HyAlign HyText2d::TextGetAlignment()
{
	return m_eAlignment;
}

void HyText2d::TextSetAlignment(HyAlign eAlignment)
{
	m_eAlignment = eAlignment;
	m_bIsDirty = true;
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

	m_bIsDirty = true;
}

void HyText2d::TextClearBox()
{
	if(0 == (m_rBox.iTag & BOXATTRIB_IsUsed))
		return;

	m_rBox.iTag &= ~BOXATTRIB_IsUsed;
	m_bIsDirty = true;
}

/*virtual*/ void HyText2d::OnDataLoaded()
{
	HyText2dData *pTextData = reinterpret_cast<HyText2dData *>(m_pData);
}

/*virtual*/ void HyText2d::OnInstUpdate()
{
	if(m_bIsDirty == false)
		return;

	HyText2dData *pData = static_cast<HyText2dData *>(m_pData);

	uint32 uiNumLayers = pData->GetNumLayers(m_uiCurFontState);
	uint32 uiStrSize = static_cast<uint32>(m_sString.size());
	
	m_RenderState.SetNumInstances(uiStrSize * uiNumLayers);
	
	m_GlyphOffsets.reserve(m_RenderState.GetNumInstances());
	m_GlyphOffsets.clear();
	
	for(uint32 i = 0; i < uiNumLayers; ++i)
	{
		glm::vec2 ptWritePos(0.0f, 0.0f);
		float fCurWordSize = 0.0f;
		float fCurLineSize = 0.0f;
		
		for(uint32 j = 0; j < uiStrSize; ++j)
		{
			const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, i, static_cast<uint32>(m_sString[i]));
			float fKerning = 0.0f;

			m_GlyphOffsets.push_back(glm::vec2(ptWritePos.x + fKerning + glyphRef.iOFFSET_X,
											   ptWritePos.y - (glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y)));

			ptWritePos.x += glyphRef.fADVANCE_X;
		}
	}

	m_bIsDirty = false;
}

/*virtual*/ void HyText2d::OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef)
{
}

/*virtual*/ void HyText2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	HyText2dData *pData = static_cast<HyText2dData *>(m_pData);

	uint32 uiNumLayers = pData->GetNumLayers(m_uiCurFontState);
	uint32 uiStrSize = static_cast<uint32>(m_sString.size());

	glm::mat4 mtxTransform;
	GetWorldTransform(mtxTransform);

	uint32 iOffsetIndex = 0;
	for(uint32 i = 0; i < uiNumLayers; ++i)
	{
		for(uint32 j = 0; j < uiStrSize; ++j, ++iOffsetIndex)
		{
			const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, 0, 'a');

			glm::vec2 vSize(glyphRef.uiWIDTH, glyphRef.uiHEIGHT);
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_GlyphOffsets[iOffsetIndex];
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::vec4 *>(pRefDataWritePos) = topColor.Get();
			pRefDataWritePos += sizeof(glm::vec4);

			*reinterpret_cast<glm::vec4 *>(pRefDataWritePos) = botColor.Get();
			pRefDataWritePos += sizeof(glm::vec4);

			// TODO: GET ACTUAL TEXTURE INDEX
			*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(0);
			pRefDataWritePos += sizeof(float);

			glm::vec2 vUV;

			vUV.x = glyphRef.rSRC_RECT.right;//1.0f;
			vUV.y = glyphRef.rSRC_RECT.top;//0.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.left;//0.0f;
			vUV.y = glyphRef.rSRC_RECT.top;//0.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.right;//1.0f;
			vUV.y = glyphRef.rSRC_RECT.bottom;//1.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.left;//0.0f;
			vUV.y = glyphRef.rSRC_RECT.bottom;//1.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::mat4 *>(pRefDataWritePos) = mtxTransform;
			pRefDataWritePos += sizeof(glm::mat4);
		}
	}
}
