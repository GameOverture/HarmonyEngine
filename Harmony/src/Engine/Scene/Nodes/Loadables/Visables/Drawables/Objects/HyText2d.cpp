/**************************************************************************
 *	HyText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyText2d.h"
#include "Scene/Nodes/Loadables/Visables/Objects/HyEntity2d.h"
#include "Utilities/HyStrManip.h"
#include "Diagnostics/Console/HyConsole.h"

#include <iostream>

#define HYTEXT2D_GlyphIndex(uiCharIndex, uiNumLayers, uiLayerIndex) static_cast<uint32>(uiCharIndex + (m_Utf32CodeList.size() * ((uiNumLayers - 1) - uiLayerIndex)))

HyText2d::HyText2d(const char *szPrefix, const char *szName, HyEntity2d *pParent) :	IHyDrawable2d(HYTYPE_Text2d, szPrefix, szName, pParent),
																					m_bIsDirty(false),
																					m_sRawString(""),
																					m_uiCurFontState(0),
																					m_vBoxDimensions(0.0f, 0.0f),
																					m_fScaleBoxModifier(1.0f),
																					m_uiBoxAttributes(0),
																					m_eAlignment(HYALIGN_Left),
																					m_bMonospacedDigits(false),
																					m_pGlyphInfos(nullptr),
																					m_uiNumReservedGlyphs(0),
																					m_uiNumValidCharacters(0),
																					m_fUsedPixelWidth(0.0f),
																					m_fUsedPixelHeight(0.0f)
{
	m_eRenderMode = HYRENDERMODE_TriangleStrip;
}

HyText2d::HyText2d(const HyText2d &copyRef) :	IHyDrawable2d(copyRef),
												m_bIsDirty(true),
												m_sRawString(copyRef.m_sRawString),
												m_Utf32CodeList(copyRef.m_Utf32CodeList),
												m_uiCurFontState(copyRef.m_uiCurFontState),
												m_uiBoxAttributes(copyRef.m_uiBoxAttributes),
												m_vBoxDimensions(copyRef.m_vBoxDimensions),
												m_fScaleBoxModifier(copyRef.m_fScaleBoxModifier),
												m_eAlignment(copyRef.m_eAlignment),
												m_bMonospacedDigits(copyRef.m_bMonospacedDigits),
												m_pGlyphInfos(nullptr),
												m_uiNumReservedGlyphs(copyRef.m_uiNumReservedGlyphs),
												m_uiNumValidCharacters(copyRef.m_uiNumValidCharacters),
												m_uiNumRenderQuads(copyRef.m_uiNumRenderQuads),
												m_fUsedPixelWidth(copyRef.m_fUsedPixelWidth),
												m_fUsedPixelHeight(copyRef.m_fUsedPixelHeight)
{
}

HyText2d::~HyText2d(void)
{
	delete[] m_pGlyphInfos;
	m_pGlyphInfos = nullptr;

	for(uint32 i = 0; i < static_cast<uint32>(m_StateColors.size()); ++i)
	{
		for(uint32 j = 0; j < static_cast<uint32>(m_StateColors[i]->m_LayerColors.size()); ++j)
			delete m_StateColors[i]->m_LayerColors[j];

		delete m_StateColors[i];
	}
}

const HyText2d &HyText2d::operator=(const HyText2d &rhs)
{
	IHyDrawable2d::operator=(rhs);

	m_bIsDirty = true;
	m_sRawString = rhs.m_sRawString;
	m_Utf32CodeList = rhs.m_Utf32CodeList;
	m_uiCurFontState = rhs.m_uiCurFontState;
	m_uiBoxAttributes = rhs.m_uiBoxAttributes;
	m_vBoxDimensions = rhs.m_vBoxDimensions;
	m_fScaleBoxModifier = rhs.m_fScaleBoxModifier;
	m_eAlignment = rhs.m_eAlignment;
	m_bMonospacedDigits = rhs.m_bMonospacedDigits;
	m_pGlyphInfos = nullptr;
	m_uiNumReservedGlyphs = rhs.m_uiNumReservedGlyphs;
	m_uiNumValidCharacters = rhs.m_uiNumValidCharacters;
	m_uiNumRenderQuads = rhs.m_uiNumRenderQuads;
	m_fUsedPixelWidth = rhs.m_fUsedPixelWidth;
	m_fUsedPixelHeight = rhs.m_fUsedPixelHeight;

	return *this;
}

/*virtual*/ HyText2d *HyText2d::Clone() const
{
	return HY_NEW HyText2d(*this);
}

// Assumes UTF-8 encoding. Accepts newline characters '\n'
void HyText2d::TextSet(const std::string sText)
{
	if(sText == m_sRawString)
		return;

	m_sRawString = sText;

	// Convert 'm_sRawString' from UTF - 8 to UTF - 32LE
	m_Utf32CodeList.clear();
	m_Utf32CodeList.reserve(m_sRawString.size());

	uint32 uiNumBytesUsed = 0;
	for(uint32 i = 0; i < m_sRawString.size(); i += uiNumBytesUsed)
	{
		m_Utf32CodeList.push_back(HyStr::HyUtf8_to_Utf32(&m_sRawString[i], uiNumBytesUsed));
		HyAssert(uiNumBytesUsed > 0, "HyText2d::TextSet failed to convert utf8 -> utf32");
	}

	MarkAsDirty();
}

const std::string &HyText2d::TextGet() const
{
	return m_sRawString;
}

uint32 HyText2d::TextGetNumCharacters() const
{
	return static_cast<uint32>(m_Utf32CodeList.size());
}

uint32 HyText2d::TextGetNumShownCharacters() const
{
	return m_uiNumValidCharacters;
}

uint32 HyText2d::GetNumRenderQuads() const
{
	return m_uiNumRenderQuads;
}

float HyText2d::TextGetScaleBoxModifer()
{
	CalculateGlyphInfos();
	return m_fScaleBoxModifier;
}

glm::vec2 HyText2d::TextGetGlyphOffset(uint32 uiCharIndex, uint32 uiLayerIndex)
{
	CalculateGlyphInfos();

	if(m_pGlyphInfos == nullptr)
		return glm::vec2(0);

	const HyText2dData *pData = static_cast<const HyText2dData *>(AcquireData());
	uint32 uiNumLayers = pData->GetNumLayers(m_uiCurFontState);

	uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNumLayers, uiLayerIndex);
	return m_pGlyphInfos[uiGlyphOffsetIndex].vOffset;
}

glm::vec2 HyText2d::TextGetGlyphSize(uint32 uiCharIndex, uint32 uiLayerIndex)
{
	CalculateGlyphInfos();

	const HyText2dData *pData = static_cast<const HyText2dData *>(AcquireData());
	const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, uiLayerIndex, m_Utf32CodeList[uiCharIndex]);

	glm::vec2 vSize(glyphRef.uiWIDTH, glyphRef.uiHEIGHT);
	vSize *= m_fScaleBoxModifier;
	return vSize;
}

float HyText2d::TextGetGlyphAlpha(uint32 uiCharIndex)
{
	if(m_pGlyphInfos == nullptr)
		return 1.0f;

	const HyText2dData *pData = static_cast<const HyText2dData *>(AcquireData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(m_uiCurFontState);

	uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNUM_LAYERS, 0);
	return m_pGlyphInfos[uiGlyphOffsetIndex].fAlpha;
}

void HyText2d::TextSetGlyphAlpha(uint32 uiCharIndex, float fAlpha)
{
	CalculateGlyphInfos();

	if(m_pGlyphInfos == nullptr)
		return;

	const HyText2dData *pData = static_cast<const HyText2dData *>(AcquireData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(m_uiCurFontState);

	for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
	{
		uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNUM_LAYERS, uiLayerIndex);
		m_pGlyphInfos[uiGlyphOffsetIndex].fAlpha = fAlpha;
	}
}

uint32 HyText2d::TextGetState()
{
	return m_uiCurFontState;
}

void HyText2d::TextSetState(uint32 uiStateIndex)
{
	if(m_uiCurFontState != uiStateIndex)
		MarkAsDirty();
	
	//HyAssert(uiStateIndex < static_cast<HyText2dData *>(AcquireData())->GetNumStates(), "HyText2d::TextSetState set state to invalid index: " << uiStateIndex);
	m_uiCurFontState = uiStateIndex;
}

uint32 HyText2d::TextGetNumLayers()
{
	return static_cast<const HyText2dData *>(AcquireData())->GetNumLayers(m_uiCurFontState);
}

uint32 HyText2d::TextGetNumLayers(uint32 uiStateIndex)
{
	return static_cast<const HyText2dData *>(AcquireData())->GetNumLayers(uiStateIndex);
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
	AcquireData();
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor.Set(fR, fG, fB);
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor.Set(fR, fG, fB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fR, float fG, float fB)
{
	AcquireData();
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor.Set(fR, fG, fB);
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor.Set(fR, fG, fB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB)
{
	AcquireData();
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->topColor.Set(fTopR, fTopG, fTopB);
	m_StateColors[m_uiCurFontState]->m_LayerColors[uiLayerIndex]->botColor.Set(fBotR, fBotG, fBotB);
}

void HyText2d::TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB)
{
	AcquireData();
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topColor.Set(fTopR, fTopG, fTopB);
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botColor.Set(fBotR, fBotG, fBotB);
}

HyAlign HyText2d::TextGetAlignment()
{
	return m_eAlignment;
}

void HyText2d::TextSetAlignment(HyAlign eAlignment)
{
	if(m_eAlignment != eAlignment)
		MarkAsDirty();
	
	m_eAlignment = eAlignment;
}

bool HyText2d::TextIsMonospacedDigits()
{
	return m_bMonospacedDigits;
}

void HyText2d::TextSetMonospacedDigits(bool bSet)
{
	if(m_bMonospacedDigits != bSet)
		MarkAsDirty();
	
	m_bMonospacedDigits = bSet;
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

	MarkAsDirty();
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

	MarkAsDirty();
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

	MarkAsDirty();
}

/*virtual*/ bool HyText2d::IsLoadDataValid() /*override*/
{
	const HyText2dData *pData = static_cast<const HyText2dData *>(AcquireData());
	return pData->GetNumStates() != 0;
}

/*virtual*/ bool HyText2d::OnIsValid() /*override*/
{
	return m_uiNumValidCharacters > 0;
}

/*virtual*/ void HyText2d::OnCalcBoundingVolume() /*override*/
{
	glm::vec2 ptCenter(0.0f, 0.0f);

	if(0 != (m_uiBoxAttributes & BOXATTRIB_IsScaleBox))
	{
		ptCenter.x = TextGetBox().x * 0.5f;
		ptCenter.y = TextGetBox().y * 0.5f;
	}

	// TODO: I don't trust this is accurate in all text types (SetAsLine, SetAsColumn, SetAsScaleBox) - needs testing
	m_LocalBoundingVolume.SetAsBox(m_fUsedPixelWidth * 0.5f, m_fUsedPixelHeight * 0.5f, ptCenter, rot.Get());
}

/*virtual*/ void HyText2d::DrawLoadedUpdate() /*override*/
{
	CalculateGlyphInfos();
}

/*virtual*/ void HyText2d::OnDataAcquired() /*override*/
{
	IHyDrawable2d::OnDataAcquired();

	const HyText2dData *pTextData = static_cast<const HyText2dData *>(UncheckedGetData());

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

	MarkAsDirty();
}

/*virtual*/ void HyText2d::OnLoaded() /*override*/
{
	IHyDrawable2d::OnLoaded();

	const HyText2dData *pTextData = static_cast<const HyText2dData *>(UncheckedGetData());
	if(pTextData == nullptr)
		return;

	if(pTextData->GetAtlas())
		m_hTextureHandle = pTextData->GetAtlas()->GetTextureHandle();

	MarkAsDirty();
}

/*virtual*/ void HyText2d::OnWriteVertexData(char *&pWritePositionRef)
{
	// CalculateGlyphInfos called here to ensure 'm_uiNumValidCharacters' is up to date with 'm_sCurrentString'
	CalculateGlyphInfos();

	const HyText2dData *pData = static_cast<const HyText2dData *>(UncheckedGetData());

	uint32 uiNumLayers = pData->GetNumLayers(m_uiCurFontState);

	glm::mat4 mtxTransform;
	GetWorldTransform(mtxTransform);

	uint32 iOffsetIndex = 0;
	for(int32 i = uiNumLayers - 1; i >= 0; --i)
	{
		for(uint32 j = 0; j < m_uiNumValidCharacters; ++j, ++iOffsetIndex)
		{
			uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(j, uiNumLayers, i);

			if(m_Utf32CodeList[j] == '\n')
				continue;

			const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, i, m_Utf32CodeList[j]);

			glm::vec2 vSize(glyphRef.uiWIDTH, glyphRef.uiHEIGHT);
			vSize *= m_fScaleBoxModifier;
			*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vSize;
			pWritePositionRef += sizeof(glm::vec2);

			*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = m_pGlyphInfos[uiGlyphOffsetIndex].vOffset;
			pWritePositionRef += sizeof(glm::vec2);

			*reinterpret_cast<glm::vec3 *>(pWritePositionRef) = m_StateColors[m_uiCurFontState]->m_LayerColors[i]->topColor.Get();
			pWritePositionRef += sizeof(glm::vec3);
			*reinterpret_cast<float *>(pWritePositionRef) = CalculateAlpha() * m_pGlyphInfos[uiGlyphOffsetIndex].fAlpha;
			pWritePositionRef += sizeof(float);

			*reinterpret_cast<glm::vec3 *>(pWritePositionRef) = m_StateColors[m_uiCurFontState]->m_LayerColors[i]->botColor.Get();
			pWritePositionRef += sizeof(glm::vec3);
			*reinterpret_cast<float *>(pWritePositionRef) = CalculateAlpha() * m_pGlyphInfos[uiGlyphOffsetIndex].fAlpha;
			pWritePositionRef += sizeof(float);

			glm::vec2 vUV;

			vUV.x = glyphRef.rSRC_RECT.right;//1.0f;
			vUV.y = glyphRef.rSRC_RECT.top;//1.0f;
			*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vUV;
			pWritePositionRef += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.left;//0.0f;
			vUV.y = glyphRef.rSRC_RECT.top;//1.0f;
			*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vUV;
			pWritePositionRef += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.right;//1.0f;
			vUV.y = glyphRef.rSRC_RECT.bottom;//0.0f;
			*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vUV;
			pWritePositionRef += sizeof(glm::vec2);

			vUV.x = glyphRef.rSRC_RECT.left;//0.0f;
			vUV.y = glyphRef.rSRC_RECT.bottom;//0.0f;
			*reinterpret_cast<glm::vec2 *>(pWritePositionRef) = vUV;
			pWritePositionRef += sizeof(glm::vec2);

			*reinterpret_cast<glm::mat4 *>(pWritePositionRef) = mtxTransform;
			pWritePositionRef += sizeof(glm::mat4);
		}
	}
}

void HyText2d::MarkAsDirty()
{
	if(IsLoaded())
		m_bIsDirty = true;
}

void HyText2d::CalculateGlyphInfos()
{
	if(m_bIsDirty == false)
		return;

	const HyText2dData *pData = static_cast<const HyText2dData *>(AcquireData());

	m_uiNumValidCharacters = 0;
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(m_uiCurFontState);
	const uint32 uiSTR_SIZE = static_cast<uint32>(m_Utf32CodeList.size());

	if(m_pGlyphInfos == nullptr || m_uiNumReservedGlyphs < uiSTR_SIZE * uiNUM_LAYERS)
	{
		delete[] m_pGlyphInfos;
		m_pGlyphInfos = nullptr;

		m_uiNumReservedGlyphs = uiSTR_SIZE * uiNUM_LAYERS;
		if(m_uiNumReservedGlyphs == 0)
			return;

		m_pGlyphInfos = HY_NEW GlyphInfo[m_uiNumReservedGlyphs];
	}

	glm::vec2 *pWritePos = HY_NEW glm::vec2[uiNUM_LAYERS];

	bool bScaleBoxModiferIsSet = false;
	m_fScaleBoxModifier = 1.0f;

	float *pMonospaceWidths = nullptr;
	float *pMonospaceAscender = nullptr;
	float *pMonospaceDecender = nullptr;
	if(m_bMonospacedDigits)
	{
		pMonospaceWidths = HY_NEW float[uiNUM_LAYERS];
		pMonospaceAscender = HY_NEW float[uiNUM_LAYERS];
		pMonospaceDecender = HY_NEW float[uiNUM_LAYERS];

		memset(pMonospaceWidths, 0, sizeof(float) * uiNUM_LAYERS);
		memset(pMonospaceAscender, 0, sizeof(float) * uiNUM_LAYERS);
		memset(pMonospaceDecender, 0, sizeof(float) * uiNUM_LAYERS);

		// Determine the largest digit width for each layer which will become the "digit mono-space" amount
		for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
		{
			// UTF-32 value of '48' == zero (...and '57' == nine)
			for(uint32 iDigit = 48; iDigit < 58; ++iDigit)
			{
				const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, uiLayerIndex, iDigit);
				if(pMonospaceWidths[uiLayerIndex] < glyphRef.fADVANCE_X)
					pMonospaceWidths[uiLayerIndex] = glyphRef.fADVANCE_X;

				if(pMonospaceAscender[uiLayerIndex] < glyphRef.iOFFSET_Y)
					pMonospaceAscender[uiLayerIndex] = static_cast<float>(glyphRef.iOFFSET_Y);

				if(pMonospaceDecender[uiLayerIndex] < (glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y))
					pMonospaceDecender[uiLayerIndex] = static_cast<float>(glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y);
			}
		}
	}

offsetCalculation:

	for(uint32 i = 0; i < m_uiNumReservedGlyphs; ++i)
		m_pGlyphInfos[i].vOffset.x = m_pGlyphInfos[i].vOffset.y = 0.0f;

	memset(pWritePos, 0, sizeof(glm::vec2) * uiNUM_LAYERS);

	// vNewlineInfo is used to set text alignment of center, right, or justified. (left alignment is already accomplished by default)
	//				It can also be used to calculate the total used space
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

		if(m_Utf32CodeList[uiStrIndex] == 32)	// 32 = ' ' character
		{
			uiLastSpaceIndex = uiStrIndex;
			fLastSpaceWidth = fCurLineWidth;
		}

		if(m_Utf32CodeList[uiStrIndex] == '\n')
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
			for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
			{
				uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiStrIndex, uiNUM_LAYERS, uiLayerIndex);

				const HyText2dGlyphInfo &glyphRef = pData->GetGlyph(m_uiCurFontState, uiLayerIndex, m_Utf32CodeList[uiStrIndex]);

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

				float fAdvanceAmtX = glyphRef.fADVANCE_X;
				float fAscender = static_cast<float>(glyphRef.iOFFSET_Y);
				float fDecender = static_cast<float>(glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y);
				float fOffsetX = static_cast<float>(glyphRef.iOFFSET_X);

				if(m_bMonospacedDigits && m_Utf32CodeList[uiStrIndex] >= 48 && m_Utf32CodeList[uiStrIndex] <= 57)
				{
					fAdvanceAmtX = pMonospaceWidths[uiLayerIndex];
					fAscender = pMonospaceAscender[uiLayerIndex];
					fDecender = pMonospaceDecender[uiLayerIndex];

					fOffsetX += (fAdvanceAmtX - glyphRef.fADVANCE_X) * 0.5f;	// This will center monospaced digits (horizontally) within their alloted space
				}

				m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.x = pWritePos[uiLayerIndex].x + ((fKerning + fOffsetX) * m_fScaleBoxModifier);
				m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.y = pWritePos[uiLayerIndex].y - ((glyphRef.uiHEIGHT - glyphRef.iOFFSET_Y) * m_fScaleBoxModifier);

				if(fLastCharWidth < pWritePos[uiLayerIndex].x)
					fLastCharWidth = pWritePos[uiLayerIndex].x;

				pWritePos[uiLayerIndex].x += (fAdvanceAmtX * m_fScaleBoxModifier);

				if(fCurLineWidth < pWritePos[uiLayerIndex].x)
					fCurLineWidth = pWritePos[uiLayerIndex].x;

				if(fCurLineAscender < (fAscender * m_fScaleBoxModifier))
					fCurLineAscender = (fAscender * m_fScaleBoxModifier);

				if(fCurLineDecender < (fDecender * m_fScaleBoxModifier))
					fCurLineDecender = (fDecender * m_fScaleBoxModifier);

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
					for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
					{
						uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiStrIndex, uiNUM_LAYERS, uiLayerIndex);
						m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.x += (fFirstCharacterNudgeRightAmt * m_fScaleBoxModifier);
						pWritePos[uiLayerIndex].x += (fFirstCharacterNudgeRightAmt * m_fScaleBoxModifier);

						if(fCurLineWidth < pWritePos[uiLayerIndex].x)
							fCurLineWidth = pWritePos[uiLayerIndex].x;
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
					for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
					{
						uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(iFirstLineStrIndex, uiNUM_LAYERS, uiLayerIndex);

						pWritePos[uiLayerIndex].y = m_vBoxDimensions.y;
						pWritePos[uiLayerIndex].y -= fCurLineAscender;

						// Because this is the first line, we know that the previous value of 'm_pGlyphInfos' is positioned from 0.0f (y-axis)
						m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.y += pWritePos[uiLayerIndex].y;
					}
				}
			}

			bFirstLine = false;
		}

		if(bDoNewline)
		{
			if(uiStrIndex == 0 && m_Utf32CodeList[uiStrIndex] != '\n')
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
		m_uiNumValidCharacters = uiSTR_SIZE;
		vNewlineInfo.push_back(LineInfo(fCurLineWidth, (fCurLineAscender + fCurLineDecender), uiNewlineIndex));	// Push the final line (row)
	}

	m_uiNumRenderQuads = ((m_uiNumValidCharacters - uiNumNewlineCharacters) * uiNUM_LAYERS);

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
					for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
					{
						uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiStrIndex, uiNUM_LAYERS, uiLayerIndex);
						m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.x += fNudgeAmt;
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
					if(m_Utf32CodeList[uiStrIndex] == ' ')
						bSpaceFound = true;

					if(bSpaceFound && m_Utf32CodeList[uiStrIndex] != ' ')
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
					if(m_Utf32CodeList[uiStrIndex] == ' ')
						bSpaceFound = true;

					if(bSpaceFound && m_Utf32CodeList[uiStrIndex] != ' ')
					{
						++uiCurWord;
						bSpaceFound = false;
					}

					for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
					{
						uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiStrIndex, uiNUM_LAYERS, uiLayerIndex);
						m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.x += ((fNudgeAmt * uiCurWord) * m_fScaleBoxModifier);
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

	m_fUsedPixelHeight = 0.0f;
	for(uint32 i = 0; i < vNewlineInfo.size(); ++i)
		m_fUsedPixelHeight += vNewlineInfo[i].fUSED_HEIGHT;

	if(0 != (m_uiBoxAttributes & BOXATTRIB_IsScaleBox))
	{
		if(bScaleBoxModiferIsSet == false)
		{
			float fScaleX = m_vBoxDimensions.x / m_fUsedPixelWidth;
			float fScaleY = m_vBoxDimensions.y / m_fUsedPixelHeight;

			m_fScaleBoxModifier = HyMin(fScaleX, fScaleY);

			bScaleBoxModiferIsSet = true;
			goto offsetCalculation;
		}
		else if(0 != (m_uiBoxAttributes & BOXATTRIB_CenterVertically))
		{
			float fCenterNudgeAmt = (m_vBoxDimensions.y - m_fUsedPixelHeight) * 0.5f;
			for(uint32 i = 0; i < m_uiNumReservedGlyphs; ++i)
				m_pGlyphInfos[i].vOffset.y -= fCenterNudgeAmt;
		}
	}

	delete[] pWritePos;
	delete[] pMonospaceWidths;
	delete[] pMonospaceAscender;
	delete[] pMonospaceDecender;

	m_bIsDirty = false;
}
