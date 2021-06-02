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

#ifdef HY_DEBUG
	#define HY_USE_TEXT_DEBUG_BOXES
#endif

#define HYTEXT2D_GlyphIndex(uiCharIndex, uiNumLayers, uiLayerIndex) static_cast<uint32>(uiCharIndex + (m_Utf32CodeList.size() * ((uiNumLayers - 1) - uiLayerIndex)))

// Convienence function macro
#define HySetText(textNode, msg) textNode.SetText((std::stringstream() << msg).str())

template<typename NODETYPE, typename ENTTYPE>
class IHyText : public NODETYPE
{
protected:
	bool							m_bIsDirty;
	std::string						m_sRawString;
	std::vector<uint32>				m_Utf32CodeList;

	struct StateColors
	{
		struct LayerColor
		{
			HyAnimVec3 		topColor;
			HyAnimVec3 		botColor;

			LayerColor(NODETYPE &colorOwner) :	topColor(colorOwner, 0),
												botColor(colorOwner, 0)
			{ }
		};
		std::vector<LayerColor *>	m_LayerColors;
	};
	std::vector<StateColors *>		m_StateColors;

	enum BoxAttributes
	{
		BOXATTRIB_IsColumn					= 1 << 0,
		BOXATTRIB_IsVertical				= 1 << 1,
		BOXATTRIB_ColumnSplitWordsToFit		= 1 << 2,
		BOXATTRIB_IsScaleBox				= 1 << 3,
		BOXATTRIB_ScaleBoxCenterVertically	= 1 << 4,
	};
	uint32				m_uiBoxAttributes;
	glm::vec2			m_vBoxDimensions;
	float				m_fScaleBoxModifier;

	HyAlignment			m_eAlignment;
	bool				m_bMonospacedDigits;

	struct GlyphInfo
	{
		glm::vec2			vOffset;
		float				fAlpha;

		GlyphInfo() :	vOffset(0.0f),
						fAlpha(1.0f)
		{ }
	};
	GlyphInfo *			m_pGlyphInfos;
	uint32				m_uiNumReservedGlyphs;		// Essentially NUM_LAYERS * NUM_UTF32_CHARACTERS

	uint32				m_uiNumValidCharacters;		// How many characters (with their effects) were rendered
	uint32				m_uiNumRenderQuads;

	float				m_fUsedPixelWidth;
	float				m_fUsedPixelHeight;

public:
	IHyText(std::string sPrefix, std::string sName, ENTTYPE *pParent);
	IHyText(const IHyText &copyRef);
	virtual ~IHyText(void);

	const IHyText &operator=(const IHyText &rhs);

	// Assumes UTF-8 encoding. Accepts newline characters '\n'
	void SetText(const std::stringstream sText);
	void SetText(const std::string sText);
	const std::string &GetText() const;

	float GetTextWidth(bool bIncludeScaling = true);
	float GetTextHeight(bool bIncludeScaling = true);

	uint32 GetNumGlyphs() const;
	uint32 GetNumShownGlyphs() const;
	uint32 GetNumRenderQuads();
	//float TextGetScaleBoxModifer();

	glm::vec2 GetGlyphOffset(uint32 uiCharIndex, uint32 uiLayerIndex);
	glm::vec2 GetGlyphSize(uint32 uiCharIndex, uint32 uiLayerIndex);
	float GetGlyphAlpha(uint32 uiCharIndex);
	void SetGlyphAlpha(uint32 uiCharIndex, float fAlpha);

	uint32 GetNumLayers();
	uint32 GetNumLayers(uint32 uiStateIndex);

	std::pair<HyAnimVec3 &, HyAnimVec3 &> GetLayerColor(uint32 uiLayerIndex);
	std::pair<HyAnimVec3 &, HyAnimVec3 &> GetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex);
	void SetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);
	void SetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fR, float fG, float fB);
	void SetLayerColor(uint32 uiLayerIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB);
	void SetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB);
	void SetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, uint32 uiRgbHex);

	HyAlignment GetTextAlignment();
	void SetTextAlignment(HyAlignment eAlignment);

	bool IsMonospacedDigits();
	void SetMonospacedDigits(bool bSet);

	const glm::vec2 &GetTextBox();

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
	virtual void OnLoadedUpdate() override = 0;

	void CalculateGlyphInfos();

private:
	void MarkAsDirty();

	// Converts a given UTF-8 encoded character (array) to its UTF-32 LE equivalent
	uint32 HyUtf8_to_Utf32(const char *pChar, uint32 &uiNumBytesUsedRef);
};

#endif /* HyText2d_h__ */
