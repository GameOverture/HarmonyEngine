/**************************************************************************
 *	IHyText.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyText_h__
#define IHyText_h__

#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimVec3.h"
#include "Utilities/HyMath.h"

#if defined(HY_DEBUG) && !defined(HY_PLATFORM_GUI)
	#define HY_USE_TEXT_DEBUG_BOXES
#endif

#define HYTEXT2D_GlyphIndex(uiCharIndex, uiNumLayers, uiLayerIndex) static_cast<uint32>(uiCharIndex + (m_Utf32CodeList.size() * ((uiNumLayers - 1) - uiLayerIndex)))

// Convenience function macro
#define HySetText(textNode, msg) textNode.SetText((std::stringstream() << msg))

template<typename NODETYPE, typename ENTTYPE>
class IHyText : public NODETYPE
{
protected:
	enum TextAttributes
	{
		TEXTATTRIB_IsDirty					= 1 << 0,
		TEXTATTRIB_IsColumn					= 1 << 1,
		TEXTATTRIB_IsVertical				= 1 << 2,
		TEXTATTRIB_ColumnSplitWordsToFit	= 1 << 3,
		TEXTATTRIB_IsScaleBox				= 1 << 4,
		TEXTATTRIB_ScaleBoxCenterVertically	= 1 << 5,
		TEXTATTRIB_UseMonospacedDigits		= 1 << 6,
		TEXTATTRIB_IsTweeningLayerColor		= 1 << 7,
	};
	uint32								m_uiTextAttributes;

	std::string							m_sRawString;
	std::vector<uint32>					m_Utf32CodeList;

	struct StateColors
	{
		struct LayerColor
		{
			HyColor 					topClr;
			HyColor 					botClr;

			struct TweenLayerColorData
			{
				HyColor					m_TopStartClr;
				HyColor					m_BotStartClr;
				HyColor					m_TopTargetClr;
				HyColor					m_BotTargetClr;
				float					m_fDuration;
				float					m_fElapsedTime;
				HyTweenFunc				m_fpActiveTweenFunc;

				TweenLayerColorData(HyColor topStartColor, HyColor botStartColor, HyColor topTargetColor, HyColor botTargetColor, float fDuration, HyTweenFunc fpTween, float fDeferStart) :
					m_TopStartClr(topStartColor),
					m_BotStartClr(botStartColor),
					m_TopTargetClr(topTargetColor),
					m_BotTargetClr(botTargetColor),
					m_fDuration(fDuration),
					m_fElapsedTime(fabs(fDeferStart) * -1.0f),
					m_fpActiveTweenFunc(fpTween)
				{ }
			};
			TweenLayerColorData *		m_pActiveTweenData = nullptr;
		};
		std::vector<LayerColor *>		m_LayerColors;
	};
	std::vector<StateColors *>			m_StateColors;

	glm::vec2							m_vBoxDimensions;
	float								m_fScaleBoxModifier;

	HyAlignment							m_eAlignment;
	uint32								m_uiIndent;

	struct GlyphInfo
	{
		glm::vec2						vOffset;
		float							fAlpha;

		GlyphInfo() :
			vOffset(0.0f),
			fAlpha(1.0f)
		{ }
	};
	GlyphInfo *							m_pGlyphInfos;
	uint32								m_uiNumReservedGlyphs;		// Essentially NUM_LAYERS * NUM_UTF32_CHARACTERS

	uint32								m_uiNumValidCharacters;		// How many characters (with their effects) were rendered
	uint32								m_uiNumRenderQuads;

	float								m_fUsedPixelWidth;
	float								m_fUsedPixelHeight;

public:
	IHyText(std::string sPrefix, std::string sName, ENTTYPE *pParent);
	IHyText(const IHyText &copyRef);
	virtual ~IHyText(void);

	const IHyText &operator=(const IHyText &rhs);

	bool IsGlyphAvailable(const std::string sUtf8Character); // Pass a single utf8 character, returns whether that character exists in the font

	const std::string &GetUtf8String() const;

	// Assumes UTF-8 encoding. Accepts newline characters '\n'
	void SetText(const std::string &sUtf8Text);
	void SetText(const std::stringstream &ssUtf8Text);

	float GetWidth(float fPercent = 1.0f);		// Returns the maximum pixel width of all lines of text (ignores any scaling)
	float GetHeight(float fPercent = 1.0f);		// Returns the maximum pixel height of all lines of text (ignores any scaling)
	
	float GetLineBreakHeight(float fPercent = 1.0f);
	float GetLineDescender(float fPercent = 1.0f);

	uint32 GetNumCharacters() const;
	uint32 GetNumShownCharacters() const;
	uint32 GetNumRenderQuads();

	uint32 GetCharacterCode(uint32 uiCharIndex) const;
	glm::vec2 GetGlyphOffset(uint32 uiCharIndex, uint32 uiLayerIndex);
	glm::vec2 GetGlyphSize(uint32 uiCharIndex, uint32 uiLayerIndex);
	float GetGlyphAlpha(uint32 uiCharIndex);
	void SetGlyphAlpha(uint32 uiCharIndex, float fAlpha);

	uint32 GetNumLayers();
	uint32 GetNumLayers(uint32 uiStateIndex);

	std::pair<HyColor, HyColor> GetLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex);
	void SetLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor);
	void TweenLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor, float fDuration, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f);
	bool IsTweeningLayerColor(); // If any layer is tweening
	bool IsTweeningLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex);
	void StopTweeningLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex);

	HyAlignment GetTextAlignment() const;
	void SetTextAlignment(HyAlignment eAlignment);

	uint32 GetTextIndent() const;
	void SetTextIndent(uint32 uiIndentPixels);

	// The offset location past the last glyph. Essentially where the user input cursor in a command window would be, on the baseline
	glm::vec2 GetTextCursorPos();

	// The offset location from 'pos' to the bottom left of all the written glyphs. Affected by things like alignment and SetAs.
	glm::vec2 GetTextBottomLeft();

	bool IsMonospacedDigits() const;
	void SetMonospacedDigits(bool bSet);

	const glm::vec2 &GetTextBoxDimensions() const;

	bool IsLine() const;
	bool IsColumn() const;
	bool IsScaleBox() const;
	bool IsScaleBoxCenterVertically() const;
	bool IsVertical() const;

	void SetAsLine();
	void SetAsColumn(float fWidth, bool bSplitWordsToFit = false);
	void SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically = true);
	void SetAsVertical();

	virtual bool SetState(uint32 uiStateIndex) override;
	virtual bool IsLoadDataValid() override;

protected:
#ifdef HY_USE_TEXT_DEBUG_BOXES
	virtual void OnSetDebugBox() = 0;
#endif
	virtual bool OnIsValidToRender() override;
	virtual void OnDataAcquired() override;
	virtual void OnLoaded() override;
	virtual void OnLoadedUpdate() override;

	void CalculateGlyphInfos();

private:
	void MarkAsDirty();

	// Converts a given UTF-8 encoded character (array) to its UTF-32 LE equivalent
	uint32 HyUtf8_to_Utf32(const char *pChar, uint32 &uiNumBytesUsedRef);
};

#endif /* HyText2d_h__ */
