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
																m_pGlyphOffsets(NULL),
																m_uiNumReservedGlyphOffsets(0)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumVerticesPerInstance(4);
}

HyText2d::~HyText2d(void)
{
	delete[] m_pGlyphOffsets;
}

void HyText2d::TextSet(std::string sText)
{
	if(sText == m_sString)
		return;

	m_sString = sText;
	m_bIsDirty = true;
}

//void HyText2d::TextSet(const char *szString, ...)
//{
//	//va_list vl;
//	//va_start(vl, szString);
//	////#ifdef HY_UNICODE
//	////	vswprintf(sm_pTempTextBuffer, HY_TEMP_TEXTBUFFER_SIZE, szString, vl);
//	////#else
//	//vsprintf(sm_pTempTextBuffer, szString, vl);
//	////#endif
//	//va_end(vl);
//
//	//if(m_sString == sm_pTempTextBuffer)
//	//	return;
//	//m_sString = sm_pTempTextBuffer;
//
//	//CalcVertexBuffer();
//}

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

uint32 HyText2d::TextGetNumLayers()
{
	return static_cast<HyText2dData *>(m_pData)->GetNumLayers(m_uiCurFontState);
}

uint32 HyText2d::TextGetNumLayers(uint32 uiStateIndex)
{
	return static_cast<HyText2dData *>(m_pData)->GetNumLayers(uiStateIndex);
}

std::pair<HyAnimVec3 &, HyAnimVec3 &> HyText2d::TextGetLayerColor(uint32 uiLayerIndex)
{
	return std::pair<HyAnimVec3 &, HyAnimVec3 &>(m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor, m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor);
}

std::pair<HyAnimVec3 &, HyAnimVec3 &> HyText2d::TextGetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex)
{
	return std::pair<HyAnimVec3 &, HyAnimVec3 &>(m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor, m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor.Set(fR, fG, fB);
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor.Set(fR, fG, fB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fR, float fG, float fB)
{
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor.Set(fR, fG, fB);
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor.Set(fR, fG, fB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB)
{
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor.Set(fTopR, fTopG, fTopB);
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor.Set(fBotR, fBotG, fBotB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB)
{
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor.Set(fTopR, fTopG, fTopB);
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor.Set(fBotR, fBotG, fBotB);
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

void HyText2d::TextSetBox(float fWidth, float fHeight, bool bCenterVertically /*= false*/, bool bScaleDownToFit /*= false*/, bool bScaleUpToFit /*= false*/)
{
	m_rBox.left = 0.0f;
	m_rBox.top = 0.0f;
	m_rBox.right = fWidth;
	m_rBox.bottom = fHeight;
	
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

	for(uint32 i = 0; i < m_StateColors.size(); ++i)
	{
		for(uint32 j = 0; j < m_StateColors[i]->m_LayerColors.size(); ++j)
			delete m_StateColors[i]->m_LayerColors[j];
	
		delete m_StateColors[i];
	}
	m_StateColors.clear();

	for(uint32 i = 0; i < pTextData->GetNumStates(); ++i)
	{
		m_StateColors.push_back(new StateColors());

		for(uint32 j = 0; j < pTextData->GetNumLayers(i); ++j)
		{
			m_StateColors[i]->m_LayerColors.push_back(new StateColors::LayerColor());

			glm::vec3 testTop = pTextData->GetDefaultColor(i, j, true);
			glm::vec3 testBot = pTextData->GetDefaultColor(i, j, true);
			
			const glm::vec3 &testtttTop = m_StateColors[i]->m_LayerColors[j]->topColor.Get();

			m_StateColors[i]->m_LayerColors[j]->topColor.Set(testTop.r, testTop.g, testTop.b);

			//testtttTop = m_StateColors[i].m_LayerColors[j].topColor.Get();
			m_StateColors[i]->m_LayerColors[j]->botColor.Set(testBot.r, testBot.g, testBot.b);
		}
	}
}

/*virtual*/ void HyText2d::OnInstUpdate()
{
	if(m_bIsDirty == false)
		return;

	HyText2dData *pData = static_cast<HyText2dData *>(m_pData);

	uint32 uiNumLayers = pData->GetNumLayers(m_uiCurFontState);
	uint32 uiStrSize = static_cast<uint32>(m_sString.size());
	
	m_RenderState.SetNumInstances(uiStrSize * uiNumLayers);
	
	if(m_pGlyphOffsets == NULL || m_uiNumReservedGlyphOffsets < m_RenderState.GetNumInstances())
	{
		delete[] m_pGlyphOffsets;
		m_uiNumReservedGlyphOffsets = m_RenderState.GetNumInstances();
		m_pGlyphOffsets = new glm::vec2[m_uiNumReservedGlyphOffsets];
	}	
	memset(m_pGlyphOffsets, 0, sizeof(glm::vec2) * m_uiNumReservedGlyphOffsets);

	glm::vec2 *pWritePos = new glm::vec2[uiNumLayers];
	memset(pWritePos, 0, sizeof(glm::vec2) * uiNumLayers);

	if(0 != (m_rBox.iTag & BOXATTRIB_IsUsed))
	{
		for(uint32 i = 0; i < uiNumLayers; ++i)
			pWritePos[i].y -= pData->GetLineGap();
	}

	uint32 uiLastSpaceIndex = 0;
	uint32 uiNewlineIndex = 0;

	for(uint32 iStrIndex = 0; iStrIndex < m_sString.length(); ++iStrIndex)
	{
		bool bDoNewline = false;

		if(m_sString[iStrIndex] == ' ')
			uiLastSpaceIndex = iStrIndex;

		// Handle every layer for this character
		for(uint32 iLayerIndex = 0; iLayerIndex < uiNumLayers; ++iLayerIndex)
		{
			const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, iLayerIndex, static_cast<uint32>(m_sString[iStrIndex]));
			float fKerning = 0.0f;

			uint32 iGlyphOffsetIndex = static_cast<uint32>(iStrIndex + (m_sString.length() * ((uiNumLayers - 1) - iLayerIndex)));

			m_pGlyphOffsets[iGlyphOffsetIndex].x = pWritePos[iLayerIndex].x + fKerning + glyphRef.iOFFSET_X;
			m_pGlyphOffsets[iGlyphOffsetIndex].y = pWritePos[iLayerIndex].y - (glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y);

			pWritePos[iLayerIndex].x += glyphRef.fADVANCE_X;

			if(iStrIndex != uiLastSpaceIndex && uiNewlineIndex != uiLastSpaceIndex)
			{
				if(0 != (m_rBox.iTag & BOXATTRIB_IsUsed) && pWritePos[iLayerIndex].x > m_rBox.right)
				{
					bDoNewline = true;
					break;
				}
			}
		}

		if(bDoNewline)
		{
			for(uint32 i = 0; i < uiNumLayers; ++i)
			{
				pWritePos[i].x = 0.0f;
				pWritePos[i].y -= pData->GetLineGap();
			}

			// Restart calculation of glyph offsets at the beginning of this this word (on a newline)
			iStrIndex = uiLastSpaceIndex; // The for-loop will increment to the character after the space
			uiNewlineIndex = iStrIndex;
		}
	}

	// TODO: Fix each line to match proper alignment

	delete[] pWritePos;

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
	for(int32 i = uiNumLayers - 1; i >= 0; --i)
	{
		for(uint32 j = 0; j < uiStrSize; ++j, ++iOffsetIndex)
		{
			const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, i, static_cast<uint32>(m_sString[j]));

			glm::vec2 vSize(glyphRef.uiWIDTH, glyphRef.uiHEIGHT);
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_pGlyphOffsets[iOffsetIndex];
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = m_StateColors[m_uiCurFontState]->m_LayerColors[i]->topColor.Get();
			pRefDataWritePos += sizeof(glm::vec3);
			*reinterpret_cast<float *>(pRefDataWritePos) = topColor.A();
			pRefDataWritePos += sizeof(float);

			*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = m_StateColors[m_uiCurFontState]->m_LayerColors[i]->botColor.Get();
			pRefDataWritePos += sizeof(glm::vec3);
			*reinterpret_cast<float *>(pRefDataWritePos) = botColor.A();
			pRefDataWritePos += sizeof(float);

			*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(pData->GetTextureIndex());
			pRefDataWritePos += sizeof(float);

			glm::vec2 vUV;

			vUV.x = glyphRef.rSRC_RECT.right;//1.0f;
			vUV.y = glyphRef.rSRC_RECT.top;//1.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.left;//0.0f;
			vUV.y = glyphRef.rSRC_RECT.top;//1.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.right;//1.0f;
			vUV.y = glyphRef.rSRC_RECT.bottom;//0.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.left;//0.0f;
			vUV.y = glyphRef.rSRC_RECT.bottom;//0.0f;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::mat4 *>(pRefDataWritePos) = mtxTransform;
			pRefDataWritePos += sizeof(glm::mat4);
		}
	}
}
