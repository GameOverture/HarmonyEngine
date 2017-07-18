/**************************************************************************
 *	HyText2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyText2d_h__
#define HyText2d_h__

#include "Scene/Nodes/Leafs/IHyLeafDraw2d.h"
#include "Assets/Nodes/HyText2dData.h"

#include <vector>

class HyText2d : public IHyLeafDraw2d
{
protected:
	bool							m_bIsDirty;
	std::string						m_sRawString;
	std::vector<uint32>				m_Utf32CodeList;

	uint32							m_uiCurFontState;

	struct StateColors
	{
		struct LayerColor
		{
			HyTweenVec3 		topColor;
			HyTweenVec3 		botColor;

			LayerColor(IHyNode2d &colorOwner) :	topColor(colorOwner, HYNODEDIRTY_DontCare),
												botColor(colorOwner, HYNODEDIRTY_DontCare)
			{ }
		};
		std::vector<LayerColor *>	m_LayerColors;
	};
	std::vector<StateColors *>		m_StateColors;

	enum BoxAttributes
	{
		BOXATTRIB_IsColumn				= 1 << 0,
		BOXATTRIB_CenterVertically		= 1 << 1,
		BOXATTRIB_ExtendingBottom		= 1 << 2,
		BOXATTRIB_SplitWordsToFit		= 1 << 3,
		BOXATTRIB_IsScaleBox			= 1 << 4,
		BOXATTRIB_FitWithinBounds		= 1 << 5,	// Whether any fancy glyph is allowed to hang outside the bounds
	};
	uint32				m_uiBoxAttributes;
	glm::vec2			m_vBoxDimensions;
	float				m_fScaleBoxModifier;

	HyAlign				m_eAlignment;
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

	uint32				m_uiNumValidCharacters;		// How many graphically shown characters

	float				m_fUsedPixelWidth;

public:
	HyText2d(const char *szPrefix, const char *szName, HyEntity2d *pParent = nullptr);
	virtual ~HyText2d(void);

	// Assumes UTF-8 encoding. Accepts newline characters '\n'
	void TextSet(const std::string &sTextRef);
	const std::string &TextGet() const;

	uint32 TextGetNumCharacters();
	uint32 TextGetNumShownCharacters();
	float TextGetScaleBoxModifer();

	glm::vec2 TextGetGlyphOffset(uint32 uiCharIndex, uint32 uiLayerIndex);
	glm::vec2 TextGetGlyphSize(uint32 uiCharIndex, uint32 uiLayerIndex);
	float TextGetGlyphAlpha(uint32 uiCharIndex);
	void TextSetGlyphAlpha(uint32 uiCharIndex, float fAlpha);

	uint32 TextGetState();
	void TextSetState(uint32 uiStateIndex);

	uint32 TextGetNumLayers();
	uint32 TextGetNumLayers(uint32 uiStateIndex);

	std::pair<HyTweenVec3 &, HyTweenVec3 &> TextGetLayerColor(uint32 uiLayerIndex);
	std::pair<HyTweenVec3 &, HyTweenVec3 &> TextGetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex);
	void TextSetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);
	void TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fR, float fG, float fB);
	void TextSetLayerColor(uint32 uiLayerIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB);
	void TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB);

	HyAlign TextGetAlignment();
	void TextSetAlignment(HyAlign eAlignment);

	bool TextIsMonospacedDigits();
	void TextSetMonospacedDigits(bool bSet);

	const glm::vec2 &TextGetBox();

	void SetAsLine();
	void SetAsColumn(float fWidth, bool bMustFitWithinColumn, bool bSplitWordsToFit = false);
	void SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically = true);

protected:
	virtual void DrawUpdate() override;
	virtual void OnDataAcquired() override;
	virtual void OnLoaded() override;
	virtual void OnUpdateUniforms() override;
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) override;
};

#endif /* HyText2d_h__ */
