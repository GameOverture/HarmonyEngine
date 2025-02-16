/**************************************************************************
 *	HyText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/IHyText.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/IHyDrawable3d.h"
#include "Assets/Nodes/Objects/HyTextData.h"
#include "Assets/Files/HyFileAtlas.h"
#include "Utilities/HyMath.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "HyEngine.h"

template<typename NODETYPE, typename ENTTYPE>
IHyText<NODETYPE, ENTTYPE>::IHyText(const HyNodePath &nodePath, ENTTYPE *pParent) :
	NODETYPE(HYTYPE_Text, nodePath, pParent),
	m_uiTextAttributes(0),
	m_sRawString(""),
	m_vBoxDimensions(0.0f, 0.0f),
	m_fScaleBoxModifier(1.0f),
	m_eAlignment(HYALIGN_Left),
	m_uiIndent(0),
	m_pGlyphInfos(nullptr),
	m_uiNumReservedGlyphs(0),
	m_uiNumValidCharacters(0),
	m_fUsedPixelWidth(0.0f),
	m_fUsedPixelHeight(0.0f)
{
	this->m_ShaderUniforms.SetNumTexUnits(1);
}

template<typename NODETYPE, typename ENTTYPE>
IHyText<NODETYPE, ENTTYPE>::IHyText(const IHyText &copyRef) :
	NODETYPE(copyRef),
	m_uiTextAttributes(copyRef.m_uiTextAttributes),
	m_sRawString(copyRef.m_sRawString),
	m_Utf32CodeList(copyRef.m_Utf32CodeList),
	m_vBoxDimensions(copyRef.m_vBoxDimensions),
	m_fScaleBoxModifier(copyRef.m_fScaleBoxModifier),
	m_eAlignment(copyRef.m_eAlignment),
	m_uiIndent(copyRef.m_uiIndent),
	m_pGlyphInfos(nullptr),
	m_uiNumReservedGlyphs(copyRef.m_uiNumReservedGlyphs),
	m_uiNumValidCharacters(copyRef.m_uiNumValidCharacters),
	m_uiNumRenderQuads(copyRef.m_uiNumRenderQuads),
	m_fUsedPixelWidth(copyRef.m_fUsedPixelWidth),
	m_fUsedPixelHeight(copyRef.m_fUsedPixelHeight)
{
	this->m_ShaderUniforms.SetNumTexUnits(1);
	MarkAsDirty();
}

template<typename NODETYPE, typename ENTTYPE>
IHyText<NODETYPE, ENTTYPE>::~IHyText(void)
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

template<typename NODETYPE, typename ENTTYPE>
const IHyText<NODETYPE, ENTTYPE> &IHyText<NODETYPE, ENTTYPE>::operator=(const IHyText<NODETYPE, ENTTYPE> &rhs)
{
	NODETYPE::operator=(rhs);

	m_uiTextAttributes = rhs.m_uiTextAttributes;
	m_sRawString = rhs.m_sRawString;
	m_Utf32CodeList = rhs.m_Utf32CodeList;
	m_vBoxDimensions = rhs.m_vBoxDimensions;
	m_fScaleBoxModifier = rhs.m_fScaleBoxModifier;
	m_eAlignment = rhs.m_eAlignment;
	m_uiIndent = rhs.m_uiIndent;
	m_pGlyphInfos = nullptr;
	m_uiNumReservedGlyphs = rhs.m_uiNumReservedGlyphs;
	m_uiNumValidCharacters = rhs.m_uiNumValidCharacters;
	m_uiNumRenderQuads = rhs.m_uiNumRenderQuads;
	m_fUsedPixelWidth = rhs.m_fUsedPixelWidth;
	m_fUsedPixelHeight = rhs.m_fUsedPixelHeight;
	MarkAsDirty();

	return *this;
}

template<typename NODETYPE, typename ENTTYPE>
HyTextType IHyText<NODETYPE, ENTTYPE>::GetTextType() const
{
	return static_cast<HyTextType>(m_uiTextAttributes & TEXTATTRIB_TypeMask);
}

template<typename NODETYPE, typename ENTTYPE>
const std::string &IHyText<NODETYPE, ENTTYPE>::GetUtf8String() const
{
	return m_sRawString;
}

template<typename NODETYPE, typename ENTTYPE>
std::string IHyText<NODETYPE, ENTTYPE>::GetUtf8Character(uint32 uiCharIndex) const
{
	if(uiCharIndex >= m_Utf32CodeList.size())
		return "";
	
	return HyIO::Utf32_to_Utf8(m_Utf32CodeList[uiCharIndex]);
}

// Assumes UTF-8 encoding. Accepts newline characters '\n'
template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetText(const std::string &sUtf8Text)
{
	if(sUtf8Text == m_sRawString)
		return;

	m_sRawString = sUtf8Text;

	// Convert 'm_sRawString' from UTF - 8 to UTF - 32LE
	m_Utf32CodeList.clear();
	m_Utf32CodeList.reserve(m_sRawString.size());

	uint32 uiNumBytesUsed = 0;
	for(uint32 i = 0; i < m_sRawString.size(); i += uiNumBytesUsed)
	{
		m_Utf32CodeList.push_back(HyIO::Utf8_to_Utf32(&m_sRawString[i], uiNumBytesUsed));
		HyAssert(uiNumBytesUsed > 0, "IHyText<NODETYPE, ENTTYPE>::TextSet failed to convert utf8 -> utf32");
	}

	MarkAsDirty();
}

// Assumes UTF-8 encoding. Accepts newline characters '\n'
template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetText(const std::stringstream &ssUtf8Text)
{
	SetText(ssUtf8Text.str());
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ float IHyText<NODETYPE, ENTTYPE>::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(m_vBoxDimensions.x != 0.0f)
		return m_vBoxDimensions.x * fPercent;

	CalculateGlyphInfos();
	return m_fUsedPixelWidth * fPercent;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ float IHyText<NODETYPE, ENTTYPE>::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(m_vBoxDimensions.y != 0.0f)
		return m_vBoxDimensions.y * fPercent;

	CalculateGlyphInfos();
	return m_fUsedPixelHeight * fPercent;
}

template<typename NODETYPE, typename ENTTYPE>
float IHyText<NODETYPE, ENTTYPE>::GetLineBreakHeight(float fPercent /*= 1.0f*/)
{
	if(this->AcquireData() == nullptr)
	{
		HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetLineHeight invoked on null data");
		return 0.0f;
	}
	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	float fLineHeight = pData->GetLineHeight(this->m_uiState);
	return fLineHeight * fPercent;
}

template<typename NODETYPE, typename ENTTYPE>
float IHyText<NODETYPE, ENTTYPE>::GetLineAscender(float fPercent /*= 1.0f*/)
{
	if(this->AcquireData() == nullptr)
	{
		HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetAscender invoked on null data");
		return 0.0f;
	}
	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	float fAscender = pData->GetLineAscender(this->m_uiState);
	return fAscender * fPercent;
}

template<typename NODETYPE, typename ENTTYPE>
float IHyText<NODETYPE, ENTTYPE>::GetLineDescender(float fPercent /*= 1.0f*/)
{
	if(this->AcquireData() == nullptr)
	{
		HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetDescender invoked on null data");
		return 0.0f;
	}
	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	float fDescender = pData->GetLineDescender(this->m_uiState);
	return fDescender * fPercent;
}

template<typename NODETYPE, typename ENTTYPE>
glm::vec2 IHyText<NODETYPE, ENTTYPE>::GetGlyphOffset(uint32 uiCharIndex, uint32 uiLayerIndex)
{
	if(uiCharIndex >= m_Utf32CodeList.size())
		return GetCursorPos();

	CalculateGlyphInfos();

	if(this->AcquireData() == nullptr || m_pGlyphInfos == nullptr)
	{
		if(this->UncheckedGetData() == nullptr) {
			HyLogDebug("IHyText<NODETYPE, ENTTYPE>::TextGetGlyphOffset invoked on null data");
		}

		return glm::vec2(0);
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	uint32 uiNumLayers = pData->GetNumLayers(this->m_uiState);

	uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNumLayers, uiLayerIndex);
	HyAssert(uiGlyphOffsetIndex < m_uiNumReservedGlyphs, "IHyText<NODETYPE, ENTTYPE>::GetGlyphOffset() - HYTEXT2D_GlyphIndex returned index that is out of bounds of m_pGlyphInfos");

	return m_pGlyphInfos[uiGlyphOffsetIndex].vOffset;
}

template<typename NODETYPE, typename ENTTYPE>
glm::vec2 IHyText<NODETYPE, ENTTYPE>::GetGlyphSize(uint32 uiCharIndex, uint32 uiLayerIndex)
{
	CalculateGlyphInfos();

	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetGlyphSize invoked on null data");
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	HyAssert(uiCharIndex < m_Utf32CodeList.size(), "IHyText<NODETYPE, ENTTYPE>::GetGlyphSize() was passed invalid 'uiCharIndex'");
	const HyTextGlyph *pGlyphRef = pData->GetGlyph(this->m_uiState, uiLayerIndex, m_Utf32CodeList[uiCharIndex]);
	if(pGlyphRef == nullptr)
		return glm::vec2(0.0f);

	glm::vec2 vSize(pGlyphRef->uiWIDTH, pGlyphRef->uiHEIGHT);
	vSize *= m_fScaleBoxModifier;
	return vSize;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsCharacterAvailable(const std::string sUtf8Character)
{
	return IsCharacterAvailable(this->m_uiState, sUtf8Character);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsCharacterAvailable(uint32 uiStateIndex, const std::string sUtf8Character)
{
	if(this->AcquireData() == nullptr)
		return false;

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	uint32 uiUsedBytes = 0;
	const HyTextGlyph *pGlyphRef = pData->GetGlyph(this->m_uiState, 0, HyIO::Utf8_to_Utf32(sUtf8Character.c_str(), uiUsedBytes));

	return pGlyphRef != nullptr;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyText<NODETYPE, ENTTYPE>::GetNumCharacters() const
{
	return static_cast<uint32>(m_Utf32CodeList.size());
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyText<NODETYPE, ENTTYPE>::GetNumShownCharacters() const
{
	return m_uiNumValidCharacters;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyText<NODETYPE, ENTTYPE>::GetNumRenderQuads()
{
	CalculateGlyphInfos();
	return m_uiNumRenderQuads;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyText<NODETYPE, ENTTYPE>::GetCharacterCode(uint32 uiCharIndex) const
{
	HyAssert(uiCharIndex < m_Utf32CodeList.size(), "IHyText<NODETYPE, ENTTYPE>::GetCharacterCode() was passed invalid 'uiCharIndex'");
	return m_Utf32CodeList[uiCharIndex];
}

template<typename NODETYPE, typename ENTTYPE>
glm::vec2 IHyText<NODETYPE, ENTTYPE>::GetCharacterOffset(uint32 uiCharIndex)
{
	CalculateGlyphInfos();

	if(this->AcquireData() == nullptr || m_pGlyphInfos == nullptr || uiCharIndex >= m_Utf32CodeList.size())
	{
		if(uiCharIndex >= m_Utf32CodeList.size())
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::GetCharacterOffset() was passed invalid 'uiCharIndex'");
		else if(this->UncheckedGetData() == nullptr)
			HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetCharacterOffset invoked on null data");

		return glm::vec2(0.0f, 0.0f);
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(this->m_uiState);

	uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNUM_LAYERS, 0);
	HyAssert(uiGlyphOffsetIndex < m_uiNumReservedGlyphs, "IHyText<NODETYPE, ENTTYPE>::GetCharacterOffset() was passed invalid 'uiCharIndex'");

	return m_pGlyphInfos[uiGlyphOffsetIndex].vUserKerning;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetCharacterOffset(uint32 uiCharIndex, glm::vec2 vOffsetAmt)
{
	CalculateGlyphInfos();

	if(this->AcquireData() == nullptr || m_pGlyphInfos == nullptr || uiCharIndex >= m_Utf32CodeList.size())
	{
		if(uiCharIndex >= m_Utf32CodeList.size())
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::SetCharacterOffset() was passed invalid 'uiCharIndex'");
		else if(this->UncheckedGetData() == nullptr)
			HyLogDebug("IHyText<NODETYPE, ENTTYPE>::SetCharacterOffset invoked on null data");

		return;
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(this->m_uiState);

	for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
	{
		uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNUM_LAYERS, uiLayerIndex);
		HyAssert(uiGlyphOffsetIndex < m_uiNumReservedGlyphs, "IHyText<NODETYPE, ENTTYPE>::SetCharacterOffset() - HYTEXT2D_GlyphIndex returned index that is out of bounds of m_pGlyphInfos");
		m_pGlyphInfos[uiGlyphOffsetIndex].vUserKerning = vOffsetAmt;
	}
}

template<typename NODETYPE, typename ENTTYPE>
float IHyText<NODETYPE, ENTTYPE>::GetCharacterScale(uint32 uiCharIndex)
{
	CalculateGlyphInfos();

	if(this->AcquireData() == nullptr || m_pGlyphInfos == nullptr || uiCharIndex >= m_Utf32CodeList.size())
	{
		if(uiCharIndex >= m_Utf32CodeList.size())
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::GetCharacterScale() was passed invalid 'uiCharIndex'");
		else if(this->UncheckedGetData() == nullptr)
			HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetCharacterScale invoked on null data");

		return 0.0f;
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(this->m_uiState);

	uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNUM_LAYERS, 0);
	HyAssert(uiGlyphOffsetIndex < m_uiNumReservedGlyphs, "IHyText<NODETYPE, ENTTYPE>::GetCharacterScale() was passed invalid 'uiCharIndex'");
	
	return m_pGlyphInfos[uiGlyphOffsetIndex].fScale;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetCharacterScale(uint32 uiCharIndex, float fScale)
{
	CalculateGlyphInfos();

	if(this->AcquireData() == nullptr || m_pGlyphInfos == nullptr)
	{
		if(this->UncheckedGetData() == nullptr) {
			HyLogDebug("IHyText<NODETYPE, ENTTYPE>::SetCharacterScale invoked on null data");
		}

		return;
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(this->m_uiState);

	for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
	{
		uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNUM_LAYERS, uiLayerIndex);
		HyAssert(uiGlyphOffsetIndex < m_uiNumReservedGlyphs, "IHyText<NODETYPE, ENTTYPE>::SetGlyphAlpha() was passed invalid 'uiCharIndex'");
		m_pGlyphInfos[uiGlyphOffsetIndex].fScale = fScale;
	}

	CalculateGlyphScaleKerning();
}

template<typename NODETYPE, typename ENTTYPE>
float IHyText<NODETYPE, ENTTYPE>::GetCharacterAlpha(uint32 uiCharIndex)
{
	if(this->AcquireData() == nullptr || m_pGlyphInfos == nullptr)
	{
		if(this->UncheckedGetData() == nullptr) {
			HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetCharacterAlpha invoked on null data");
		}

		return 1.0f;
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(this->m_uiState);

	uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNUM_LAYERS, 0);
	HyAssert(uiGlyphOffsetIndex < m_uiNumReservedGlyphs, "IHyText<NODETYPE, ENTTYPE>::GetGlyphAlpha() was passed invalid 'uiCharIndex'");
	return m_pGlyphInfos[uiGlyphOffsetIndex].fAlpha;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetCharacterAlpha(uint32 uiCharIndex, float fAlpha)
{
	CalculateGlyphInfos();

	if(this->AcquireData() == nullptr || m_pGlyphInfos == nullptr)
	{
		if(this->UncheckedGetData() == nullptr) {
			HyLogDebug("IHyText<NODETYPE, ENTTYPE>::SetCharacterAlpha invoked on null data");
		}

		return;
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(this->m_uiState);

	for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
	{
		uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiCharIndex, uiNUM_LAYERS, uiLayerIndex);
		HyAssert(uiGlyphOffsetIndex < m_uiNumReservedGlyphs, "IHyText<NODETYPE, ENTTYPE>::SetCharacterAlpha() was passed invalid 'uiCharIndex'");
		m_pGlyphInfos[uiGlyphOffsetIndex].fAlpha = fAlpha;
	}
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyText<NODETYPE, ENTTYPE>::GetNumLayers()
{
	return this->GetNumLayers(this->m_uiState);
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyText<NODETYPE, ENTTYPE>::GetNumLayers(uint32 uiStateIndex)
{
	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetNumLayers invoked on null data");
		return 0;
	}

	return static_cast<const HyTextData *>(this->UncheckedGetData())->GetNumLayers(uiStateIndex);
}

template<typename NODETYPE, typename ENTTYPE>
std::pair<HyColor, HyColor> IHyText<NODETYPE, ENTTYPE>::GetLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex)
{
	if(this->AcquireData() == nullptr || uiStateIndex >= m_StateColors.size() || uiLayerIndex >= m_StateColors[uiStateIndex]->m_LayerColors.size())
	{
		if(this->AcquireData() == nullptr)
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::GetLayerColor invoked on null data");
		else
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::GetLayerColor() was passed invalid 'uiStateIndex' or 'uiLayerIndex'");
		return std::pair<HyColor, HyColor>(HyColor(0, 0, 0, 0), HyColor(0, 0, 0, 0));
	}

	return std::pair<HyColor, HyColor>(m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topClr, m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botClr);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor)
{
	if(this->AcquireData() == nullptr || uiStateIndex >= m_StateColors.size() || uiLayerIndex >= m_StateColors[uiStateIndex]->m_LayerColors.size())
	{
		if(this->AcquireData() == nullptr)
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::SetLayerColor invoked on null data");
		else
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::SetLayerColor() was passed invalid 'uiStateIndex' or 'uiLayerIndex'");
		return;
	}

	StopTweeningLayerColor(uiStateIndex, uiLayerIndex);
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topClr = topColor;
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botClr = botColor;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::TweenLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor, float fDuration, HyTweenFunc fpTween /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/)
{
	if(this->AcquireData() == nullptr || uiStateIndex >= m_StateColors.size() || uiLayerIndex >= m_StateColors[uiStateIndex]->m_LayerColors.size())
	{
		if(this->AcquireData() == nullptr)
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::TweenLayerColor invoked on null data");
		else
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::TweenLayerColor() was passed invalid 'uiStateIndex' or 'uiLayerIndex'");
		return;
	}

	m_uiTextAttributes |= TEXTATTRIB_IsTweeningLayerColor;

	delete m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->m_pActiveTweenData;
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->m_pActiveTweenData = HY_NEW StateColors::LayerColor::TweenLayerColorData(m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->topClr, m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->botClr, topColor, botColor, fDuration, fpTween, fDeferStart);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsTweeningLayerColor()
{
	return (m_uiTextAttributes & TEXTATTRIB_IsTweeningLayerColor) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsTweeningLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex)
{
	if(this->AcquireData() == nullptr || uiStateIndex >= m_StateColors.size() || uiLayerIndex >= m_StateColors[uiStateIndex]->m_LayerColors.size())
	{
		if(this->AcquireData() == nullptr)
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::IsTweeningLayerColor invoked on null data");
		else
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::IsTweeningLayerColor() was passed invalid 'uiStateIndex' or 'uiLayerIndex'");
		return false;
	}

	return m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->m_pActiveTweenData != nullptr;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::StopTweeningLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex)
{
	if(this->AcquireData() == nullptr || uiStateIndex >= m_StateColors.size() || uiLayerIndex >= m_StateColors[uiStateIndex]->m_LayerColors.size())
	{
		if(this->AcquireData() == nullptr)
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::StopTweeningLayerColor invoked on null data");
		else
			HyLogWarning("IHyText<NODETYPE, ENTTYPE>::StopTweeningLayerColor() was passed invalid 'uiStateIndex' or 'uiLayerIndex'");
		return;
	}

	delete m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->m_pActiveTweenData;
	m_StateColors[uiStateIndex]->m_LayerColors[uiLayerIndex]->m_pActiveTweenData = nullptr;
}

template<typename NODETYPE, typename ENTTYPE>
HyAlignment IHyText<NODETYPE, ENTTYPE>::GetAlignment() const
{
	return m_eAlignment;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetAlignment(HyAlignment eAlignment)
{
	if(m_eAlignment != eAlignment)
	{
		MarkAsDirty();
		m_eAlignment = eAlignment;
	}
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyText<NODETYPE, ENTTYPE>::GetTextIndent() const
{
	return m_uiIndent;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetTextIndent(uint32 uiIndentPixels)
{
	if(m_uiIndent != uiIndentPixels)
	{
		MarkAsDirty();
		m_uiIndent = uiIndentPixels;
	}
}

// The offset location past the last glyph. Essentially where the user input cursor in a command window would be, on the baseline
template<typename NODETYPE, typename ENTTYPE>
glm::vec2 IHyText<NODETYPE, ENTTYPE>::GetCursorPos()
{
	CalculateGlyphInfos();

	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetCursorPos invoked on null data");
		return glm::vec2();
	}

	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	if(m_uiNumValidCharacters > 0)
	{
		uint32 uiLastCharOffset = 1;
		uint32 uiUtf32Code = m_Utf32CodeList[m_uiNumValidCharacters - uiLastCharOffset];
		while(uiUtf32Code == 10) // 10 == '\n' character
		{
			if(m_uiNumValidCharacters >= uiLastCharOffset)
			{
				uiLastCharOffset++;
				uiUtf32Code = m_Utf32CodeList[m_uiNumValidCharacters - uiLastCharOffset];
			}
			else
				return glm::vec2();
		}
		const HyTextGlyph *pGlyph = pData->GetGlyph(this->m_uiState, 0, uiUtf32Code);
		if(pGlyph == nullptr)
			return glm::vec2();

		uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(m_uiNumValidCharacters - uiLastCharOffset, pData->GetNumLayers(this->m_uiState), 0);

		glm::vec2 ptCursorPos = m_pGlyphInfos[uiGlyphOffsetIndex].vOffset + glm::vec2(pGlyph->fADVANCE_X * m_fScaleBoxModifier, 0.0f);

		ptCursorPos.x += pData->GetLeftSideNudgeAmt(this->m_uiState); // HACK: Technically this should be GetRightSideNudgeAmt, but we don't store it and this is probably close enough
		ptCursorPos.y += (static_cast<int32>(pGlyph->uiHEIGHT) - pGlyph->iOFFSET_Y) * m_fScaleBoxModifier; // Find the baseline of this last glyph
		
		ptCursorPos.x *= this->scale.X();
		ptCursorPos.y *= this->scale.Y();
		return ptCursorPos;
	}

	return glm::vec2();
}

template<typename NODETYPE, typename ENTTYPE>
glm::vec2 IHyText<NODETYPE, ENTTYPE>::GetBottomLeft()
{
	HyTextType eTextType = GetTextType();

	if(eTextType == HYTEXT_Box || eTextType == HYTEXT_ScaleBox)
		return glm::vec2(0.0f, 0.0f);

	if(this->AcquireData() == nullptr) {
		HyLogDebug("IHyText<NODETYPE, ENTTYPE>::GetBottomLeft invoked on null data");
		return glm::vec2(0.0f, 0.0f);
	}

	CalculateGlyphInfos();
	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());

	float fX = 0.0f;
	if(eTextType != HYTEXT_Column)
	{
		switch(m_eAlignment)
		{
		case HYALIGN_Center:
			fX = GetWidth(-0.5f);
			break;

		case HYALIGN_Right:
			fX = GetWidth(-1.0f);
			break;

		default:
			break;
		}
	}

	return glm::vec2(fX, GetCursorPos().y);
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsMonospacedDigits() const
{
	return (m_uiTextAttributes & TEXTATTRIB_UseMonospacedDigits) != 0;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetMonospacedDigits(bool bSet)
{
	if(IsMonospacedDigits() != bSet)
		MarkAsDirty();
	
	if(bSet)
		m_uiTextAttributes |= TEXTATTRIB_UseMonospacedDigits;
	else
		m_uiTextAttributes &= ~TEXTATTRIB_UseMonospacedDigits;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsCenterVertically() const
{
	return m_uiTextAttributes & TEXTATTRIB_CenterVertically;
}

template<typename NODETYPE, typename ENTTYPE>
const glm::vec2 &IHyText<NODETYPE, ENTTYPE>::GetTextBoxDimensions() const
{
	return m_vBoxDimensions;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsLine() const
{
	return GetTextType() == HYTEXT_Line;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsColumn() const
{
	return GetTextType() == HYTEXT_Column;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsBox() const
{
	return GetTextType() == HYTEXT_Box;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsScaleBox() const
{
	return GetTextType() == HYTEXT_ScaleBox;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyText<NODETYPE, ENTTYPE>::IsVertical() const
{
	return GetTextType() == HYTEXT_Vertical;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetAsLine()
{
	m_uiTextAttributes &= ~HYTEXT_MASK;
	m_uiTextAttributes |= HYTEXT_Line;

	m_vBoxDimensions.x = 0.0f;
	m_vBoxDimensions.y = 0.0f;
	if(m_uiTextAttributes & TEXTATTRIB_BoxScissor)
	{
		this->ClearScissor(true);
		m_uiTextAttributes &= ~TEXTATTRIB_BoxScissor;
	}

	MarkAsDirty();
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetAsColumn(float fWidth)
{
	if(fWidth <= 0.0f)
	{
		HyLogWarning("IHyText<NODETYPE, ENTTYPE>::SetAsColumn() invoked with invalid width: " << fWidth);
		fWidth = HyMath::Max(1.0f, fWidth);
	}

	m_uiTextAttributes &= ~HYTEXT_MASK;
	m_uiTextAttributes |= HYTEXT_Column;

	m_vBoxDimensions.x = fWidth;
	m_vBoxDimensions.y = 0.0f;
	if(m_uiTextAttributes & TEXTATTRIB_BoxScissor)
	{
		this->ClearScissor(true);
		m_uiTextAttributes &= ~TEXTATTRIB_BoxScissor;
	}

	MarkAsDirty();
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetAsBox(float fWidth, float fHeight, bool bCenterVertically /*= false*/, bool bUseScissor /*= true*/)
{
	if(fWidth <= 0.0f || fHeight <= 0.0f)
	{
		HyLogWarning("IHyText<NODETYPE, ENTTYPE>::SetAsTextBox() invoked with invalid dimensions: " << fWidth << ", " << fHeight);
		fWidth = HyMath::Max(1.0f, fWidth);
		fHeight = HyMath::Max(1.0f, fHeight);
	}

	m_uiTextAttributes &= ~HYTEXT_MASK;
	m_uiTextAttributes |= HYTEXT_Box;

	if(bCenterVertically)
		m_uiTextAttributes |= TEXTATTRIB_CenterVertically;
	else
		m_uiTextAttributes &= ~TEXTATTRIB_CenterVertically;

	m_vBoxDimensions.x = fWidth;
	m_vBoxDimensions.y = fHeight;
	if(bUseScissor)
	{
		SetScissor(HyRect(fWidth, fHeight));
		m_uiTextAttributes |= TEXTATTRIB_BoxScissor;
	}
	else if(m_uiTextAttributes & TEXTATTRIB_BoxScissor)
	{
		this->ClearScissor(true);
		m_uiTextAttributes &= ~TEXTATTRIB_BoxScissor;
	}

	MarkAsDirty();
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically /*= true*/)
{
	if(fWidth <= 0.0f || fHeight <= 0.0f)
	{
		HyLogWarning("IHyText<NODETYPE, ENTTYPE>::SetAsScaleBox() invoked with invalid dimensions: " << fWidth << ", " << fHeight);
		fWidth = HyMath::Max(1.0f, fWidth);
		fHeight = HyMath::Max(1.0f, fHeight);
	}

	m_uiTextAttributes &= ~HYTEXT_MASK;
	m_uiTextAttributes |= HYTEXT_ScaleBox;

	if(bCenterVertically)
		m_uiTextAttributes |= TEXTATTRIB_CenterVertically;
	else
		m_uiTextAttributes &= ~TEXTATTRIB_CenterVertically;

	m_vBoxDimensions.x = fWidth;
	m_vBoxDimensions.y = fHeight;
	if(m_uiTextAttributes & TEXTATTRIB_BoxScissor)
	{
		this->ClearScissor(true);
		m_uiTextAttributes &= ~TEXTATTRIB_BoxScissor;
	}

	MarkAsDirty();
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::SetAsVertical()
{
	m_uiTextAttributes &= ~HYTEXT_MASK;
	m_uiTextAttributes |= HYTEXT_Vertical;

	m_vBoxDimensions.x = 0.0f;
	m_vBoxDimensions.y = 0.0f;
	if(m_uiTextAttributes & TEXTATTRIB_BoxScissor)
	{
		this->ClearScissor(true);
		m_uiTextAttributes &= ~TEXTATTRIB_BoxScissor;
	}

	MarkAsDirty();
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHyText<NODETYPE, ENTTYPE>::SetState(uint32 uiStateIndex) /*override*/
{
	if(this->m_uiState == uiStateIndex || IHyLoadable::SetState(uiStateIndex) == false)
		return this->m_uiState == uiStateIndex; // Return true if the state is already set, otherwise return false because IHyLoadable::SetState() failed

	MarkAsDirty();
	return true;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHyText<NODETYPE, ENTTYPE>::IsLoadDataValid() /*override*/
{
	const HyTextData *pData = static_cast<const HyTextData *>(this->AcquireData());
	return pData && pData->GetNumStates() != 0;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHyText<NODETYPE, ENTTYPE>::OnIsValidToRender() /*override*/
{
	// OnIsValidToRender() will always be invoked before this IHyText is sent off to render.
	// Esure CalculateGlyphInfos() will always have a chance to be invoked here to not skip any rendering frame
	CalculateGlyphInfos(); // sets 'm_uiNumValidCharacters' inside
	return m_uiNumValidCharacters > 0;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHyText<NODETYPE, ENTTYPE>::OnDataAcquired() /*override*/
{
	const HyTextData *pTextData = static_cast<const HyTextData *>(this->UncheckedGetData());

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
			m_StateColors[i]->m_LayerColors.push_back(HY_NEW typename StateColors::LayerColor());

			m_StateColors[i]->m_LayerColors[j]->topClr = pTextData->GetDefaultColor(i, j, true);
			m_StateColors[i]->m_LayerColors[j]->botClr = pTextData->GetDefaultColor(i, j, false);
		}
	}

	MarkAsDirty();
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHyText<NODETYPE, ENTTYPE>::OnLoaded() /*override*/
{
	NODETYPE::OnLoaded();

	const HyTextData *pTextData = static_cast<const HyTextData *>(this->UncheckedGetData());
	if(pTextData == nullptr)
		return;

	if(pTextData->GetAtlas())
		this->m_ShaderUniforms.SetTexHandle(0, pTextData->GetAtlas()->GetTextureHandle());

	MarkAsDirty();
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHyText<NODETYPE, ENTTYPE>::OnLoadedUpdate() /*override*/
{
	CalculateGlyphInfos();

	if(m_uiTextAttributes & TEXTATTRIB_IsTweeningLayerColor)
	{
		bool bContinueTweening = false;
		for(uint32 i = 0; i < m_StateColors.size(); ++i)
		{
			for(uint32 j = 0; j < m_StateColors[i]->m_LayerColors.size(); ++j)
			{
				if(m_StateColors[i]->m_LayerColors[j]->m_pActiveTweenData)
				{
					StateColors::LayerColor::TweenLayerColorData *pTweenData = m_StateColors[i]->m_LayerColors[j]->m_pActiveTweenData;

					if(pTweenData->m_fElapsedTime < 0.0f) // Handle deferred start
					{
						pTweenData->m_fElapsedTime = HyMath::Min(pTweenData->m_fElapsedTime + HyEngine::DeltaTime(), 0.0f);
						bContinueTweening = true;
					}
					else
					{
						pTweenData->m_fElapsedTime = HyMath::Clamp(pTweenData->m_fElapsedTime + HyEngine::DeltaTime(), 0.0f, pTweenData->m_fDuration);
						if(pTweenData->m_fElapsedTime == pTweenData->m_fDuration)
						{
							m_StateColors[i]->m_LayerColors[j]->topClr = pTweenData->m_TopTargetClr;
							m_StateColors[i]->m_LayerColors[j]->botClr = pTweenData->m_BotTargetClr;

							delete m_StateColors[i]->m_LayerColors[j]->m_pActiveTweenData;
							m_StateColors[i]->m_LayerColors[j]->m_pActiveTweenData = nullptr;
						}
						else
						{
							float fProgress = pTweenData->m_fpActiveTweenFunc(pTweenData->m_fElapsedTime / pTweenData->m_fDuration);

							glm::vec4 startClr(pTweenData->m_TopStartClr.GetRedF(), pTweenData->m_TopStartClr.GetGreenF(), pTweenData->m_TopStartClr.GetBlueF(), pTweenData->m_TopStartClr.GetAlphaF());
							glm::vec4 targetClr(pTweenData->m_TopTargetClr.GetRedF(), pTweenData->m_TopTargetClr.GetGreenF(), pTweenData->m_TopTargetClr.GetBlueF(), pTweenData->m_TopTargetClr.GetAlphaF());
							float fRed = startClr.r + (targetClr.r - startClr.r) * fProgress;
							float fGreen = startClr.g + (targetClr.g - startClr.g) * fProgress;
							float fBlue = startClr.b + (targetClr.b - startClr.b) * fProgress;
							float fAlpha = startClr.a + (targetClr.a - startClr.a) * fProgress;
							m_StateColors[i]->m_LayerColors[j]->topClr = HyColor(fRed, fGreen, fBlue, fAlpha);

							HySetVec(startClr, pTweenData->m_BotStartClr.GetRedF(), pTweenData->m_BotStartClr.GetGreenF(), pTweenData->m_BotStartClr.GetBlueF(), pTweenData->m_BotStartClr.GetAlphaF());
							HySetVec(targetClr, pTweenData->m_BotTargetClr.GetRedF(), pTweenData->m_BotTargetClr.GetGreenF(), pTweenData->m_BotTargetClr.GetBlueF(), pTweenData->m_BotTargetClr.GetAlphaF());
							fRed = startClr.r + (targetClr.r - startClr.r) * fProgress;
							fGreen = startClr.g + (targetClr.g - startClr.g) * fProgress;
							fBlue = startClr.b + (targetClr.b - startClr.b) * fProgress;
							fAlpha = startClr.a + (targetClr.a - startClr.a) * fProgress;
							m_StateColors[i]->m_LayerColors[j]->botClr = HyColor(fRed, fGreen, fBlue, fAlpha);

							bContinueTweening = true;
						}
					}
				}
			}
		}

		if(bContinueTweening == false)
			m_uiTextAttributes &= ~TEXTATTRIB_IsTweeningLayerColor;
	}
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::CalculateGlyphInfos()
{
	if((m_uiTextAttributes & TEXTATTRIB_IsDirty) == 0 || this->AcquireData() == nullptr)
		return;

	const HyTextType eTEXT_TYPE = GetTextType();
	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());

	m_uiNumValidCharacters = m_uiNumRenderQuads = 0;
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(this->m_uiState);
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
	if(IsMonospacedDigits())
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
				const HyTextGlyph *pGlyphRef = pData->GetGlyph(this->m_uiState, uiLayerIndex, iDigit);
				if(pGlyphRef)
				{
					if(pMonospaceWidths[uiLayerIndex] < pGlyphRef->fADVANCE_X)
						pMonospaceWidths[uiLayerIndex] = pGlyphRef->fADVANCE_X;

					if(pMonospaceAscender[uiLayerIndex] < pGlyphRef->iOFFSET_Y)
						pMonospaceAscender[uiLayerIndex] = static_cast<float>(pGlyphRef->iOFFSET_Y);

					if(pMonospaceDecender[uiLayerIndex] < (static_cast<int32>(pGlyphRef->uiHEIGHT) - pGlyphRef->iOFFSET_Y))
						pMonospaceDecender[uiLayerIndex] = static_cast<float>(static_cast<int32>(pGlyphRef->uiHEIGHT) - pGlyphRef->iOFFSET_Y);
				}
			}
		}
	}

offsetCalculation:

	for(uint32 i = 0; i < m_uiNumReservedGlyphs; ++i)
		m_pGlyphInfos[i].vOffset.x = m_pGlyphInfos[i].vOffset.y = 0.0f;

	memset(pWritePos, 0, sizeof(glm::vec2) * uiNUM_LAYERS);

	// Apply indent to each write position layer
	if(m_uiIndent != 0)
	{
		for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
			pWritePos[uiLayerIndex].x += m_uiIndent;// *m_fScaleBoxModifier;
	}

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
	float fLastSpacePosX = 0.0f;
	float fLastCharWidth = 0.0f;
	float fCurLineWidth = 0.0f;
	float fCurLineHeight = 0.0f;
	float fCurLineAscender = 0.0f;
	float fCurLineDecender = 0.0f;	// Stored as positive value

	int32 iLastSpaceIndex = -1;
	uint32 uiNewlineIndex = 0;
	uint32 uiNumUnprintableCharacters = 0;
	bool bFirstCharacterOnNewLine = true;
	float fFirstCharacterNudgeRightAmt = 0.0f;

	bool bFirstLine = true;

	for(uint32 uiStrIndex = 0; uiStrIndex < uiSTR_SIZE; ++uiStrIndex)
	{
		bool bDoNewline = false;
		bool bHandleNewlineCharacter = false;

		if(m_Utf32CodeList[uiStrIndex] == 32)	// 32 = ' ' character
		{
			iLastSpaceIndex = uiStrIndex;
			fLastSpacePosX = fCurLineWidth;
		}

		if(m_Utf32CodeList[uiStrIndex] == '\n')
		{
			++uiNumUnprintableCharacters;

			bDoNewline = true;
			++uiStrIndex;	// increment past the '\n' since the algorithm below assumes a regular character to be the uiNewlineIndex

			//uiLastSpaceIndex = uiNewlineIndex;	// Assigning uiLastSpaceIndex to be equal to uiNewlineIndex will "trick" the algorithm below to NOT split the line at the last ' ' character
			bHandleNewlineCharacter = true;

			fLastCharWidth = fCurLineWidth;	// Since we aren't technically splitting to the previous character, this will assign the proper line width to vNewlineInfo
		}
		else
		{
			// Handle every layer for this character
			for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
			{
				uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiStrIndex, uiNUM_LAYERS, uiLayerIndex);

				const HyTextGlyph *pGlyphRef = pData->GetGlyph(this->m_uiState, uiLayerIndex, m_Utf32CodeList[uiStrIndex]);
				if(pGlyphRef == nullptr)
				{
					uiNumUnprintableCharacters++;
					break;
				}

				float fKerning = 0.0f;
				if(bFirstCharacterOnNewLine)
				{
					if(pGlyphRef->iOFFSET_X < 0 && fFirstCharacterNudgeRightAmt < abs(pGlyphRef->iOFFSET_X))
						fFirstCharacterNudgeRightAmt = static_cast<float>(abs(pGlyphRef->iOFFSET_X));
				}
				else
				{
					// TODO: Apply kerning if it isn't the first character of a newline
					fKerning = 0.0f;
				}

				float fAdvanceAmtX = pGlyphRef->fADVANCE_X;
				float fAscender = static_cast<float>(pGlyphRef->iOFFSET_Y);
				float fDecender = HyMath::Clamp(static_cast<float>(static_cast<int32>(pGlyphRef->uiHEIGHT) - pGlyphRef->iOFFSET_Y), 0.0f, pData->GetLineHeight(this->m_uiState));
				float fOffsetX = static_cast<float>(pGlyphRef->iOFFSET_X);

				if(IsMonospacedDigits() && m_Utf32CodeList[uiStrIndex] >= 48 && m_Utf32CodeList[uiStrIndex] <= 57)
				{
					fAdvanceAmtX = pMonospaceWidths[uiLayerIndex];
					fAscender = pMonospaceAscender[uiLayerIndex];
					fDecender = pMonospaceDecender[uiLayerIndex];

					fOffsetX += (fAdvanceAmtX - pGlyphRef->fADVANCE_X) * 0.5f;	// This will center monospaced digits (horizontally) within their alloted space
				}

				m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.x = pWritePos[uiLayerIndex].x + ((fKerning + fOffsetX) * m_fScaleBoxModifier);
				m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.y = pWritePos[uiLayerIndex].y - ((static_cast<int32>(pGlyphRef->uiHEIGHT) - pGlyphRef->iOFFSET_Y) * m_fScaleBoxModifier);

				if(fLastCharWidth < pWritePos[uiLayerIndex].x)
					fLastCharWidth = pWritePos[uiLayerIndex].x;

				pWritePos[uiLayerIndex].x += (fAdvanceAmtX * m_fScaleBoxModifier);

				if(fCurLineWidth < pWritePos[uiLayerIndex].x)
					fCurLineWidth = pWritePos[uiLayerIndex].x;

				if(fCurLineHeight < static_cast<float>(pGlyphRef->uiHEIGHT))
					fCurLineHeight = static_cast<float>(pGlyphRef->uiHEIGHT);

				if(fCurLineAscender < (fAscender * m_fScaleBoxModifier))
					fCurLineAscender = (fAscender * m_fScaleBoxModifier);

				if(fCurLineDecender < (fDecender * m_fScaleBoxModifier))
					fCurLineDecender = (fDecender * m_fScaleBoxModifier);

				// If drawing text within a column or box, and we advance past our width, determine if we should newline
				if((eTEXT_TYPE == HYTEXT_Column || eTEXT_TYPE == HYTEXT_Box) && fCurLineWidth > m_vBoxDimensions.x)
				{
					// If splitting words is ok, continue. Otherwise ensure this isn't the only word on the line
					//if((m_uiTextAttributes & TEXTATTRIB_ColumnSplitWordsToFit) != 0 ||
					//	((m_uiTextAttributes & TEXTATTRIB_ColumnSplitWordsToFit) == 0 && uiNewlineIndex != uiLastSpaceIndex))
					{
						// Don't newline on ' ' characters
						if(uiStrIndex != iLastSpaceIndex)
						{
							bDoNewline = true;
							break;
						}
					}
				}
			}

			if(eTEXT_TYPE == HYTEXT_Vertical)
			{
				bDoNewline = true;
			}
			else if(bFirstCharacterOnNewLine)
			{
				// Handle whether any fancy glyph is hanging outside the bounds for every layer for this character
				for(uint32 uiLayerIndex = 0; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
				{
					uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiStrIndex, uiNUM_LAYERS, uiLayerIndex);
					m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.x += (fFirstCharacterNudgeRightAmt * m_fScaleBoxModifier);
					pWritePos[uiLayerIndex].x += (fFirstCharacterNudgeRightAmt * m_fScaleBoxModifier);

					if(fCurLineWidth < pWritePos[uiLayerIndex].x)
						fCurLineWidth = pWritePos[uiLayerIndex].x;
				}

				fFirstCharacterNudgeRightAmt = 0.0f;
				bFirstCharacterOnNewLine = false;
			}
		}

		// If this is the first line, and we're a Box or ScaleBox, then place text snug against the top of the bounds box
		if(bFirstLine && (bDoNewline || uiStrIndex == (uiSTR_SIZE - 1)))
		{
			if(eTEXT_TYPE == HYTEXT_Box || eTEXT_TYPE == HYTEXT_ScaleBox)
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
			float fNewLineOffset = (pData->GetLineHeight(this->m_uiState) * m_fScaleBoxModifier);

			if(eTEXT_TYPE == HYTEXT_Vertical)
				fNewLineOffset = fCurLineHeight;
			//else if(m_uiIndent == 0 && uiStrIndex == 0 && m_Utf32CodeList[uiStrIndex] != '\n') {
			//	HyLogWarning("Text box is too small to fit a single character");
			//}

			// Reset the write position onto a newline
			for(uint32 i = 0; i < uiNUM_LAYERS; ++i)
			{
				pWritePos[i].x = 0.0f;
				pWritePos[i].y -= fNewLineOffset;
			}

			if(eTEXT_TYPE != HYTEXT_Vertical)
			{
				// Restart calculation of glyph offsets at the beginning of this this word (on a newline)
				if(bHandleNewlineCharacter == false && (uiNewlineIndex != iLastSpaceIndex || iLastSpaceIndex == 0))
				{
					uiStrIndex = iLastSpaceIndex;
					fCurLineWidth = fLastSpacePosX;
				}
				else // Handling '\n' or we're splitting mid-word, go back one character to place on newline
				{
					--uiStrIndex;
					fCurLineWidth = fLastCharWidth;
				}
			}

			// Push back this line of text's info, and initialize for the next
			vNewlineInfo.push_back(LineInfo(fCurLineWidth, fNewLineOffset, uiNewlineIndex));
			fLastSpacePosX = 0.0f;
			fLastCharWidth = 0.0f;
			fCurLineWidth = 0.0f;
			fCurLineHeight = 0.0f;
			fCurLineAscender = 0.0f;
			fCurLineDecender = 0.0f;

			// The next for-loop iteration will increment uiStrIndex to the character after the ' '. Assign uiNewlineIndex and uiLastSpaceIndex a '+1' to compensate
			uiNewlineIndex = iLastSpaceIndex = uiStrIndex + 1;

			bFirstCharacterOnNewLine = true;
		}
	} // for(uiStrIndex < uiSTR_SIZE)

	// Push the final line (row)
	vNewlineInfo.push_back(LineInfo(fCurLineWidth, (fCurLineAscender + fCurLineDecender), uiNewlineIndex));

	m_uiNumValidCharacters = uiSTR_SIZE;
	m_uiNumRenderQuads = ((m_uiNumValidCharacters - uiNumUnprintableCharacters) * uiNUM_LAYERS);

	// Fix each text line to match proper alignment (by default, HYALIGN_Left is already set at this point)
	if(m_eAlignment != HYALIGN_Left)
	{
		for(uint32 i = 0; i < vNewlineInfo.size(); ++i)
		{
			// Don't bother with alignment on first line if there's an indent (will use HYALIGN_Left instead)
			if(i == 0 && m_uiIndent != 0)
				continue;

			float fNudgeAmt = (m_vBoxDimensions.x - vNewlineInfo[i].fUSED_WIDTH);
			fNudgeAmt *= (m_eAlignment == HYALIGN_Center) ? 0.5f : 1.0f; // else HYALIGN_Right|HYALIGN_Justify

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

				if(uiNumWords == 0)
					continue;

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

	if(eTEXT_TYPE == HYTEXT_Box || eTEXT_TYPE == HYTEXT_ScaleBox)
	{
		if(eTEXT_TYPE == HYTEXT_ScaleBox && bScaleBoxModiferIsSet == false)
		{
			float fScaleX = m_vBoxDimensions.x / m_fUsedPixelWidth;
			float fScaleY = m_vBoxDimensions.y / m_fUsedPixelHeight;

			m_fScaleBoxModifier = HyMath::Min(fScaleX, fScaleY);

			bScaleBoxModiferIsSet = true;
			goto offsetCalculation;
		}
		else if(0 != (m_uiTextAttributes & TEXTATTRIB_CenterVertically))
		{
			float fCenterNudgeAmt = (m_vBoxDimensions.y - m_fUsedPixelHeight) * 0.5f;
			for(uint32 i = 0; i < m_uiNumReservedGlyphs; ++i)
				m_pGlyphInfos[i].vOffset.y -= fCenterNudgeAmt;
		}
	}
	//else if(0 != (m_uiTextAttributes & TEXTATTRIB_IsColumn))	// Move column text to fit below its node position, which will extend downward from
	//{
	//	float fTopLineNudgeAmt = vNewlineInfo[0].fUSED_HEIGHT;
	//	for(uint32 i = 0; i < m_uiNumReservedGlyphs; ++i)
	//		m_pGlyphInfos[i].vOffset.y -= fTopLineNudgeAmt;
	//}

	delete[] pWritePos;
	delete[] pMonospaceWidths;
	delete[] pMonospaceAscender;
	delete[] pMonospaceDecender;

	CalculateGlyphScaleKerning();

	this->SetDirty(this->DIRTY_SceneAABB);
	m_uiTextAttributes &= ~TEXTATTRIB_IsDirty;

#ifdef HY_USE_TEXT_DEBUG_BOXES
	OnSetDebugBox();
#endif
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::CalculateGlyphScaleKerning()
{
	const HyTextData *pData = static_cast<const HyTextData *>(this->UncheckedGetData());
	const uint32 uiNUM_LAYERS = pData->GetNumLayers(this->m_uiState);
	if(uiNUM_LAYERS == 0)
		return;

	float fGlyphScaleKerningAccum = 0.0f;
	for(uint32 uiIndex = 0; uiIndex < m_uiNumValidCharacters; ++uiIndex)
	{
		uint32 uiGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiIndex, uiNUM_LAYERS, 0);

		if(GetTextType() == HYTEXT_Vertical)
		{
			if(uiIndex > 0)
			{
				uint32 uiPrevGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiIndex - 1, uiNUM_LAYERS, 0);

				fGlyphScaleKerningAccum += (m_pGlyphInfos[uiPrevGlyphOffsetIndex].vOffset.y - m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.y) * (1.0f - m_pGlyphInfos[uiGlyphOffsetIndex].fScale);
				m_pGlyphInfos[uiGlyphOffsetIndex].vScaleKerning.y = fGlyphScaleKerningAccum;
			}
			
			if(GetAlignment() == HYALIGN_Center)
				m_pGlyphInfos[uiGlyphOffsetIndex].vScaleKerning.x = (-1.0f * m_pGlyphInfos[uiGlyphOffsetIndex].vOffset.x) * (1.0f - m_pGlyphInfos[uiGlyphOffsetIndex].fScale);
		}
		else // Left-to-Right
		{
			const HyTextGlyph *pGlyphRef = pData->GetGlyph(m_uiState, 0, m_Utf32CodeList[uiIndex]);
			if(pGlyphRef)
			{
				fGlyphScaleKerningAccum -= pGlyphRef->iOFFSET_X * (1.0f - m_pGlyphInfos[uiGlyphOffsetIndex].fScale);
				m_pGlyphInfos[uiGlyphOffsetIndex].vScaleKerning.x = fGlyphScaleKerningAccum;

				fGlyphScaleKerningAccum -= pGlyphRef->uiWIDTH * (1.0f - m_pGlyphInfos[uiGlyphOffsetIndex].fScale);
			}
			else
				m_pGlyphInfos[uiGlyphOffsetIndex].vScaleKerning.x = fGlyphScaleKerningAccum;
		}

		// Apply this character's scale kerning to all layers (1 thru X)
		for(uint32 uiLayerIndex = 1; uiLayerIndex < uiNUM_LAYERS; ++uiLayerIndex)
		{
			uint32 uiLayerGlyphOffsetIndex = HYTEXT2D_GlyphIndex(uiIndex, uiNUM_LAYERS, uiLayerIndex);
			glm::vec2 vLayerDiff = (m_pGlyphInfos[uiGlyphOffsetIndex].vOffset - m_pGlyphInfos[uiLayerGlyphOffsetIndex].vOffset);

			m_pGlyphInfos[uiLayerGlyphOffsetIndex].vScaleKerning = m_pGlyphInfos[uiGlyphOffsetIndex].vScaleKerning + (vLayerDiff * (1.0f - m_pGlyphInfos[uiGlyphOffsetIndex].fScale));
		}
	}
}

template<typename NODETYPE, typename ENTTYPE>
void IHyText<NODETYPE, ENTTYPE>::MarkAsDirty()
{
	m_uiTextAttributes |= TEXTATTRIB_IsDirty;
}

template class IHyText<IHyDrawable2d, HyEntity2d>;
template class IHyText<IHyDrawable3d, HyEntity3d>;
