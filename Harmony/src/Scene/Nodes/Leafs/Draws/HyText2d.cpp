/**************************************************************************
 *	HyText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Leafs/Draws/HyText2d.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "Utilities/HyStrManip.h"

#include <iostream>

HyText2d::HyText2d(const char *szPrefix, const char *szName, HyEntity2d *pParent /*= nullptr*/) :	IHyLeafDraw2d(HYTYPE_Text2d, szPrefix, szName, pParent),
																									m_bIsDirty(true),
																									m_sCurrentString(""),
																									m_uiCurFontState(0),
																									m_vBoxDimensions(0.0f, 0.0f),
																									m_fScaleBoxModifier(1.0f),
																									m_uiBoxAttributes(0),
																									m_eAlignment(HYALIGN_Left),
																									m_pGlyphOffsets(nullptr),
																									m_uiNumReservedGlyphOffsets(0),
																									m_uiNumValidCharacters(0),
																									m_fUsedPixelWidth(0.0f)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::DRAWINSTANCED);
	m_RenderState.SetShaderId(HYSHADERPROG_QuadBatch);
	m_RenderState.SetNumVerticesPerInstance(4);
}

HyText2d::~HyText2d(void)
{
	delete[] m_pGlyphOffsets;
}

// Assumes UTF-8 encoding. Accepts newline characters '\n'
void HyText2d::TextSet(const std::string &sTextRef)
{
	if(sTextRef == m_sCurrentString)
		return;

	m_sCurrentString = sTextRef;
	m_bIsDirty = true;
}

const std::string &HyText2d::TextGet() const
{
	return m_sCurrentString;
}

uint32 HyText2d::TextGetStrLength()
{
	return static_cast<uint32>(m_sCurrentString.size());
}

float HyText2d::TextGetScaleBoxModifer()
{
	if(m_bIsDirty)
		DrawUpdate();

	return m_fScaleBoxModifier;
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
	return static_cast<HyText2dData *>(AcquireData())->GetNumLayers(m_uiCurFontState);
}

uint32 HyText2d::TextGetNumLayers(uint32 uiStateIndex)
{
	return static_cast<HyText2dData *>(AcquireData())->GetNumLayers(uiStateIndex);
}

std::pair<HyTweenVec3 &, HyTweenVec3 &> HyText2d::TextGetLayerColor(uint32 uiLayerIndex)
{
	return std::pair<HyTweenVec3 &, HyTweenVec3 &>(m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor, m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor);
}

std::pair<HyTweenVec3 &, HyTweenVec3 &> HyText2d::TextGetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex)
{
	return std::pair<HyTweenVec3 &, HyTweenVec3 &>(m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor, m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor);
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

const glm::vec2 &HyText2d::TextGetBox()
{
	return m_vBoxDimensions;
}

void HyText2d::SetAsLine()
{
	m_uiBoxAttributes = 0;
	m_vBoxDimensions.x = 0.0f;
	m_vBoxDimensions.y = 0.0f;

	m_bIsDirty = true;
}

void HyText2d::SetAsColumn(float fWidth, bool bMustFitWithinColumn, bool bSplitWordsToFit /*= false*/)
{
	int32 iFlags = BOXATTRIB_IsColumn | BOXATTRIB_ExtendingBottom;

	if(bMustFitWithinColumn)
		iFlags |= BOXATTRIB_FitWithinBounds;

	if(bSplitWordsToFit)
		iFlags |= BOXATTRIB_SplitWordsToFit;
	
	if(m_uiBoxAttributes == iFlags && m_vBoxDimensions.x == fWidth)
		return;

	m_vBoxDimensions.x = fWidth;
	m_vBoxDimensions.y = 0.0f;

	m_uiBoxAttributes = iFlags;

	m_bIsDirty = true;
}

void HyText2d::SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically /*= true*/)
{
	int32 iFlags = BOXATTRIB_IsScaleBox | BOXATTRIB_FitWithinBounds;

	if(bCenterVertically)
		iFlags |= BOXATTRIB_CenterVertically;

	if(m_uiBoxAttributes == iFlags && m_vBoxDimensions.x == fWidth && m_vBoxDimensions.y == fHeight)
		return;

	m_vBoxDimensions.x = fWidth;
	m_vBoxDimensions.y = fHeight;

	m_uiBoxAttributes = iFlags;

	m_bIsDirty = true;
}

/*virtual*/ void HyText2d::OnDataAcquired()
{
	HyText2dData *pTextData = reinterpret_cast<HyText2dData *>(UncheckedGetData());

	for(uint32 i = 0; i < m_StateColors.size(); ++i)
	{
		for(uint32 j = 0; j < m_StateColors[i]->m_LayerColors.size(); ++j)
			delete m_StateColors[i]->m_LayerColors[j];
	
		delete m_StateColors[i];
	}
	m_StateColors.clear();

	for(uint32 i = 0; i < pTextData->GetNumStates(); ++i)
	{
		m_StateColors.push_back(HY_NEW StateColors());

		for(uint32 j = 0; j < pTextData->GetNumLayers(i); ++j)
		{
			m_StateColors[i]->m_LayerColors.push_back(HY_NEW StateColors::LayerColor(*this));

			m_StateColors[i]->m_LayerColors[j]->topColor.Set(pTextData->GetDefaultColor(i, j, true));
			m_StateColors[i]->m_LayerColors[j]->botColor.Set(pTextData->GetDefaultColor(i, j, false));
		}
	}
}

/*virtual*/ void HyText2d::OnLoaded()
{
	HyText2dData *pTextData = reinterpret_cast<HyText2dData *>(UncheckedGetData());
	m_RenderState.SetTextureHandle(pTextData->GetAtlas()->GetGfxApiHandle());
}

/*virtual*/ void HyText2d::DrawUpdate()
{
	if(m_bIsDirty == false)
		return;

	HyText2dData *pData = static_cast<HyText2dData *>(AcquireData());

	m_uiNumValidCharacters = 0;
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(m_uiCurFontState);


	// TODO: Convert 'm_sCurrentString' from UTF-8 to UTF-32LE

	const uint32 uiSTR_SIZE = static_cast<uint32>(m_sCurrentString.size());
	
	if(m_pGlyphOffsets == nullptr || m_uiNumReservedGlyphOffsets < uiSTR_SIZE * uiNUM_LAYERS)
	{
		delete[] m_pGlyphOffsets;
		m_uiNumReservedGlyphOffsets = uiSTR_SIZE * uiNUM_LAYERS;
		m_pGlyphOffsets = HY_NEW glm::vec2[m_uiNumReservedGlyphOffsets];
	}

	glm::vec2 *pWritePos = HY_NEW glm::vec2[uiNUM_LAYERS];
	
	bool bScaleBoxModiferIsSet = false;
	m_fScaleBoxModifier = 1.0f;

offsetCalculation:

	memset(m_pGlyphOffsets, 0, sizeof(glm::vec2) * m_uiNumReservedGlyphOffsets);
	memset(pWritePos, 0, sizeof(glm::vec2) * uiNUM_LAYERS);

	// vNewlineInfo is used to set text alignment of center, right, or justified. (left alignment is already accomplished by default)
	struct LineInfo
	{
		const float fUSED_WIDTH;
		const float fUSED_HEIGHT;
		const uint32 uiSTART_CHARACTER_INDEX;

		LineInfo(float fUsedWidth, float fUsedHeight, uint32 uiStartCharIndex) : fUSED_WIDTH(fUsedWidth), fUSED_HEIGHT(fUsedHeight), uiSTART_CHARACTER_INDEX(uiStartCharIndex)
		{ }
	};
	std::vector<LineInfo> vNewlineInfo;
	float fLastSpaceWidth = 0.0f;
	float fLastCharWidth = 0.0f;
	float fCurLineWidth = 0.0f;
	float fCurLineAscender = 0.0f;
	float fCurLineDecender = 0.0f;	// Stored as positive value

	uint32 uiLastSpaceIndex = 0;
	uint32 uiNewlineIndex = 0;
	uint32 uiNumNewlineCharacters = 0;
	bool bTerminatedEarly = false;
	bool bFirstCharacterOnNewLine = true;
	float fFirstCharacterNudgeRightAmt = 0.0f;
	
	bool bFirstLine = true;

	for(uint32 uiStrIndex = 0; uiStrIndex < uiSTR_SIZE; ++uiStrIndex)
	{
		bool bDoNewline = false;

		if(m_sCurrentString[uiStrIndex] == ' ')
		{
			uiLastSpaceIndex = uiStrIndex;
			fLastSpaceWidth = fCurLineWidth;
		}

		if(m_sCurrentString[uiStrIndex] == '\n')
		{
			++uiNumNewlineCharacters;

			bDoNewline = true;
			++uiStrIndex;	// increment past the '\n' since the algorithm assumes a regular character to be the uiNewlineIndex

			uiLastSpaceIndex = uiNewlineIndex;	// Assigning uiLastSpaceIndex to be equal to uiNewlineIndex will "trick" the algorithm below to NOT split the line at the last ' ' character
			fLastCharWidth = fCurLineWidth;	// Since we aren't technically splitting to the previous character, this will assign the proper line width to vNewlineInfo
		}
		else
		{
			// Handle every layer for this character
			for(uint32 iLayerIndex = 0; iLayerIndex < uiNUM_LAYERS; ++iLayerIndex)
			{
				const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, iLayerIndex, HyUtf8_to_Utf32(&m_sCurrentString[uiStrIndex]));

				float fKerning = 0.0f;
				if(bFirstCharacterOnNewLine)
				{
					if(glyphRef.iOFFSET_X < 0 && fFirstCharacterNudgeRightAmt < abs(glyphRef.iOFFSET_X))
						fFirstCharacterNudgeRightAmt = static_cast<float>(abs(glyphRef.iOFFSET_X));
				}
				else
				{
					// TODO: Apply kerning if it isn't the first character of a newline
					fKerning = 0.0f;
				}

				uint32 iGlyphOffsetIndex = static_cast<uint32>(uiStrIndex + (uiSTR_SIZE * ((uiNUM_LAYERS - 1) - iLayerIndex)));
				m_pGlyphOffsets[iGlyphOffsetIndex].x = pWritePos[iLayerIndex].x + ((fKerning + glyphRef.iOFFSET_X) * m_fScaleBoxModifier);
				m_pGlyphOffsets[iGlyphOffsetIndex].y = pWritePos[iLayerIndex].y - ((glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y) * m_fScaleBoxModifier);

				if(fLastCharWidth < pWritePos[iLayerIndex].x)
					fLastCharWidth = pWritePos[iLayerIndex].x;

				pWritePos[iLayerIndex].x += (glyphRef.fADVANCE_X * m_fScaleBoxModifier);

				if(fCurLineWidth < m_pGlyphOffsets[iGlyphOffsetIndex].x + (glyphRef.uiWIDTH * m_fScaleBoxModifier))
					fCurLineWidth = m_pGlyphOffsets[iGlyphOffsetIndex].x + (glyphRef.uiWIDTH * m_fScaleBoxModifier);

				if(fCurLineAscender < (glyphRef.iOFFSET_Y * m_fScaleBoxModifier))
					fCurLineAscender = (glyphRef.iOFFSET_Y * m_fScaleBoxModifier);

				if(fCurLineDecender < ((glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y) * m_fScaleBoxModifier))
					fCurLineDecender = ((glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y) * m_fScaleBoxModifier);

				// If drawing text within a box, and we advance past our width, determine if we should newline
				if((m_uiBoxAttributes & BOXATTRIB_IsScaleBox) == 0 &&
				   (m_uiBoxAttributes & BOXATTRIB_IsColumn) != 0 &&
				   fCurLineWidth > m_vBoxDimensions.x)
				{
					// If splitting words is ok, continue. Otherwise ensure this isn't the only word on the line
					if((m_uiBoxAttributes & BOXATTRIB_SplitWordsToFit) != 0 ||
					   ((m_uiBoxAttributes & BOXATTRIB_SplitWordsToFit) == 0 && uiNewlineIndex != uiLastSpaceIndex))
					{
						// Don't newline on ' ' characters
						if(uiStrIndex != uiLastSpaceIndex)
						{
							bDoNewline = true;
							break;
						}
					}
				}
			}

			if(bFirstCharacterOnNewLine)
			{
				if(0 != (m_uiBoxAttributes & BOXATTRIB_FitWithinBounds))
				{
					// Handle every layer for this character
					for(uint32 iLayerIndex = 0; iLayerIndex < uiNUM_LAYERS; ++iLayerIndex)
					{
						uint32 iGlyphOffsetIndex = static_cast<uint32>(uiStrIndex + (uiSTR_SIZE * ((uiNUM_LAYERS - 1) - iLayerIndex)));
						m_pGlyphOffsets[iGlyphOffsetIndex].x += (fFirstCharacterNudgeRightAmt * m_fScaleBoxModifier);
						pWritePos[iLayerIndex].x += (fFirstCharacterNudgeRightAmt * m_fScaleBoxModifier);

						const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, iLayerIndex, HyUtf8_to_Utf32(&m_sCurrentString[uiStrIndex]));
						if(fCurLineWidth < m_pGlyphOffsets[iGlyphOffsetIndex].x + (glyphRef.uiWIDTH * m_fScaleBoxModifier))
							fCurLineWidth = m_pGlyphOffsets[iGlyphOffsetIndex].x + (glyphRef.uiWIDTH * m_fScaleBoxModifier);
					}
				}

				fFirstCharacterNudgeRightAmt = 0.0f;
				bFirstCharacterOnNewLine = false;
			}
		}

		// If this is the first line, and we're a BOXATTRIB_ScaleBox, then place text snug against the top of the bounds box
		if(bFirstLine &&
		   (bDoNewline || uiStrIndex == (uiSTR_SIZE - 1)))
		{
			if(0 != (m_uiBoxAttributes & BOXATTRIB_IsScaleBox))
			{
				for(int32 iFirstLineStrIndex = static_cast<int32>(uiStrIndex); iFirstLineStrIndex >= 0; --iFirstLineStrIndex)
				{
					// Handle every layer for this character
					for(uint32 iLayerIndex = 0; iLayerIndex < uiNUM_LAYERS; ++iLayerIndex)
					{
						uint32 iGlyphOffsetIndex = static_cast<uint32>(iFirstLineStrIndex + (uiSTR_SIZE * ((uiNUM_LAYERS - 1) - iLayerIndex)));

						pWritePos[iLayerIndex].y = m_vBoxDimensions.y;
						pWritePos[iLayerIndex].y -= fCurLineAscender;

						// Because this is the first line, we know that the previous value of 'm_pGlyphOffsets' is positioned from 0.0f (y-axis)
						m_pGlyphOffsets[iGlyphOffsetIndex].y += pWritePos[iLayerIndex].y;
					}
				}
			}

			bFirstLine = false;
		}

		if(bDoNewline)
		{
			if(uiStrIndex == 0 && m_sCurrentString[uiStrIndex] != '\n')
			{
				// Text box is too small to fit a single character
				m_uiNumValidCharacters = 0;
				bTerminatedEarly = true;
				break;
			}

			// Reset the write position onto a newline
			for(uint32 i = 0; i < uiNUM_LAYERS; ++i)
			{
				pWritePos[i].x = 0.0f;
				pWritePos[i].y -= (pData->GetLineHeight(m_uiCurFontState) * m_fScaleBoxModifier);
			}

			// Restart calculation of glyph offsets at the beginning of this this word (on a newline)
			if(uiNewlineIndex != uiLastSpaceIndex)
			{
				uiStrIndex = uiLastSpaceIndex;
				fCurLineWidth = fLastSpaceWidth;
			}
			else // Splitting mid-word, go back one character to place on newline
			{
				--uiStrIndex;
				fCurLineWidth = fLastCharWidth;
			}

			// Push back this line of text's info, and initialize for the next
			vNewlineInfo.push_back(LineInfo(fCurLineWidth, (fCurLineAscender + fCurLineDecender), uiNewlineIndex));
			fLastSpaceWidth = 0.0f;
			fLastCharWidth = 0.0f;
			fCurLineWidth = 0.0f;
			fCurLineAscender = 0.0f;
			fCurLineDecender = 0.0f;
			
			// The next for-loop iteration will increment uiStrIndex to the character after the ' '. Assign uiNewlineIndex and uiLastSpaceIndex a '+1' to compensate
			uiNewlineIndex = uiLastSpaceIndex = uiStrIndex + 1;

			bFirstCharacterOnNewLine = true;
		}
	}

	if(bTerminatedEarly == false)
	{
		m_uiNumValidCharacters = static_cast<uint32>(m_sCurrentString.size());
		vNewlineInfo.push_back(LineInfo(fCurLineWidth, (fCurLineAscender + fCurLineDecender), uiNewlineIndex));	// Push the final line (row)
	}

	m_RenderState.SetNumInstances((m_uiNumValidCharacters * uiNUM_LAYERS) - uiNumNewlineCharacters);

	// Fix each text line to match proper alignment (HYALIGN_Left is already set at this point)
	if(m_eAlignment != HYALIGN_Left)
	{
		for(uint32 i = 0; i < vNewlineInfo.size(); ++i)
		{
			float fNudgeAmt = (m_vBoxDimensions.x - vNewlineInfo[i].fUSED_WIDTH);// - (pData->GetLeftSideNudgeAmt(m_uiCurFontState) * m_fScaleBoxModifier);
			fNudgeAmt *= (m_eAlignment == HYALIGN_Center) ? 0.5f : 1.0f;

			uint32 uiStrIndex = vNewlineInfo[i].uiSTART_CHARACTER_INDEX;
			uint32 uiEndIndex = (i + 1) < vNewlineInfo.size() ? vNewlineInfo[i + 1].uiSTART_CHARACTER_INDEX : m_uiNumValidCharacters;

			if(m_eAlignment != HYALIGN_Justify)
			{
				for(; uiStrIndex < uiEndIndex; ++uiStrIndex)
				{
					for(uint32 iLayerIndex = 0; iLayerIndex < uiNUM_LAYERS; ++iLayerIndex)
					{
						uint32 iGlyphOffsetIndex = static_cast<uint32>(uiStrIndex + (uiSTR_SIZE * ((uiNUM_LAYERS - 1) - iLayerIndex)));
						m_pGlyphOffsets[iGlyphOffsetIndex].x += fNudgeAmt;
					}
				}
			}
			else
			{
				// Justify doesn't affect the last line
				if(i == vNewlineInfo.size() - 1)
					continue;

				// Count number of words on this line. Combine any consecutive ' ' characters, and eat any trailing ' '
				uint32 uiNumWords = 0;
				bool bSpaceFound = false;
				for(; uiStrIndex < uiEndIndex; ++uiStrIndex)
				{
					if(m_sCurrentString[uiStrIndex] == ' ')
						bSpaceFound = true;

					if(bSpaceFound && m_sCurrentString[uiStrIndex] != ' ')
					{
						++uiNumWords;
						bSpaceFound = false;
					}
				}

				fNudgeAmt /= uiNumWords;
				
				uiStrIndex = vNewlineInfo[i].uiSTART_CHARACTER_INDEX;
				uint32 uiCurWord = 0;
				bSpaceFound = false;
				for(; uiStrIndex < uiEndIndex; ++uiStrIndex)
				{
					if(m_sCurrentString[uiStrIndex] == ' ')
						bSpaceFound = true;

					if(bSpaceFound && m_sCurrentString[uiStrIndex] != ' ')
					{
						++uiCurWord;
						bSpaceFound = false;
					}

					for(uint32 iLayerIndex = 0; iLayerIndex < uiNUM_LAYERS; ++iLayerIndex)
					{
						uint32 iGlyphOffsetIndex = static_cast<uint32>(uiStrIndex + (uiSTR_SIZE * ((uiNUM_LAYERS - 1) - iLayerIndex)));
						m_pGlyphOffsets[iGlyphOffsetIndex].x += ((fNudgeAmt * uiCurWord) * m_fScaleBoxModifier);
					}
				}
			}
		}
	}

	m_fUsedPixelWidth = 0.0f;
	for(uint32 i = 0; i < vNewlineInfo.size(); ++i)
	{
		if(m_fUsedPixelWidth < vNewlineInfo[i].fUSED_WIDTH)
			m_fUsedPixelWidth = vNewlineInfo[i].fUSED_WIDTH;
	}

	if(0 != (m_uiBoxAttributes & BOXATTRIB_IsScaleBox))
	{
		float fTotalHeight = 0.0f;
		for(uint32 i = 0; i < vNewlineInfo.size(); ++i)
			fTotalHeight += vNewlineInfo[i].fUSED_HEIGHT;

		if(bScaleBoxModiferIsSet == false)
		{
			float fScaleX = m_vBoxDimensions.x / m_fUsedPixelWidth;
			float fScaleY = m_vBoxDimensions.y / fTotalHeight;

			m_fScaleBoxModifier = HyMin(fScaleX, fScaleY);

			bScaleBoxModiferIsSet = true;
			goto offsetCalculation;
		}
		else if(0 != (m_uiBoxAttributes & BOXATTRIB_CenterVertically))
		{
			float fCenterNudgeAmt = (m_vBoxDimensions.y - fTotalHeight) * 0.5f;
			for(uint32 i = 0; i < m_uiNumReservedGlyphOffsets; ++i)
				m_pGlyphOffsets[i].y -= fCenterNudgeAmt;
		}
	}

	delete[] pWritePos;

	m_bIsDirty = false;
}

/*virtual*/ void HyText2d::OnUpdateUniforms()
{
	//m_ShaderUniforms.Set(...);
}

/*virtual*/ void HyText2d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
	// DrawUpdate called here to ensure 'm_uiNumValidCharacters' is up to date with 'm_sCurrentString'
	DrawUpdate();

	HyText2dData *pData = static_cast<HyText2dData *>(UncheckedGetData());

	uint32 uiNumLayers = pData->GetNumLayers(m_uiCurFontState);

	glm::mat4 mtxTransform;
	GetWorldTransform(mtxTransform);

	uint32 iOffsetIndex = 0;
	for(int32 i = uiNumLayers - 1; i >= 0; --i)
	{
		for(uint32 j = 0; j < m_uiNumValidCharacters; ++j, ++iOffsetIndex)
		{
			if(m_sCurrentString[j] == '\n')
				continue;

			const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, i, HyUtf8_to_Utf32(&m_sCurrentString[j]));

			glm::vec2 vSize(glyphRef.uiWIDTH, glyphRef.uiHEIGHT);
			vSize *= m_fScaleBoxModifier;
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vSize;
			pRefDataWritePos += sizeof(glm::vec2);

			uint32 iGlyphOffsetIndex = static_cast<uint32>(j + (m_sCurrentString.size() * ((uiNumLayers - 1) - i)));
			*reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_pGlyphOffsets[iGlyphOffsetIndex];
			pRefDataWritePos += sizeof(glm::vec2);

			*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = m_StateColors[m_uiCurFontState]->m_LayerColors[i]->topColor.Get();
			pRefDataWritePos += sizeof(glm::vec3);
			*reinterpret_cast<float *>(pRefDataWritePos) = CalculateAlpha();
			pRefDataWritePos += sizeof(float);

			*reinterpret_cast<glm::vec3 *>(pRefDataWritePos) = m_StateColors[m_uiCurFontState]->m_LayerColors[i]->botColor.Get();
			pRefDataWritePos += sizeof(glm::vec3);
			*reinterpret_cast<float *>(pRefDataWritePos) = CalculateAlpha();
			pRefDataWritePos += sizeof(float);

			//*reinterpret_cast<float *>(pRefDataWritePos) = static_cast<float>(pData->GetAtlas()->GetActualGfxApiTextureIndex(pData->GetAtlasGroupTextureIndex()));
			//pRefDataWritePos += sizeof(float);

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
