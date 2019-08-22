/**************************************************************************
 *	HyText2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyText2d_h__
#define HyText2d_h__

#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable2d.h"
#include "Assets/Nodes/HyText2dData.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/Objects/HyPrimitive2d.h"

class HyText2d : public IHyDrawable2d
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
			HyAnimVec3 		topColor;
			HyAnimVec3 		botColor;

			LayerColor(IHyNode2d &colorOwner) :	topColor(colorOwner, 0),
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
#ifdef HY_DEBUG
	HyPrimitive2d		m_DebugBox;
#endif

	HyTextAlign				m_eAlignment;
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
	HyText2d(const char *szPrefix, const char *szName, HyEntity2d *pParent);
	HyText2d(const HyText2d &copyRef);
	virtual ~HyText2d(void);

	const HyText2d &operator=(const HyText2d &rhs);
	virtual HyText2d *Clone() const;

	// Assumes UTF-8 encoding. Accepts newline characters '\n'
	void TextSet(const std::string sText);
	const std::string &TextGet() const;

	float TextGetPixelWidth(bool bIncludeScaling = true);
	float TextGetPixelHeight(bool bIncludeScaling = true);

	uint32 TextGetNumCharacters() const;
	uint32 TextGetNumShownCharacters() const;
	uint32 GetNumRenderQuads() const;
	float TextGetScaleBoxModifer();

	glm::vec2 TextGetGlyphOffset(uint32 uiCharIndex, uint32 uiLayerIndex);
	glm::vec2 TextGetGlyphSize(uint32 uiCharIndex, uint32 uiLayerIndex);
	float TextGetGlyphAlpha(uint32 uiCharIndex);
	void TextSetGlyphAlpha(uint32 uiCharIndex, float fAlpha);

	uint32 TextGetState();
	void TextSetState(uint32 uiStateIndex);

	uint32 TextGetNumLayers();
	uint32 TextGetNumLayers(uint32 uiStateIndex);

	std::pair<HyAnimVec3 &, HyAnimVec3 &> TextGetLayerColor(uint32 uiLayerIndex);
	std::pair<HyAnimVec3 &, HyAnimVec3 &> TextGetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex);
	void TextSetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);
	void TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fR, float fG, float fB);
	void TextSetLayerColor(uint32 uiLayerIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB);
	void TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB);
	void TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, uint32 uiRgbHex);

	HyTextAlign TextGetAlignment();
	void TextSetAlignment(HyTextAlign eAlignment);

	bool TextIsMonospacedDigits();
	void TextSetMonospacedDigits(bool bSet);

	const glm::vec2 &TextGetBox();

	void SetAsLine();
	void SetAsColumn(float fWidth, bool bSplitWordsToFit = false);
	void SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically = true);
	void SetAsVertical();

	virtual bool IsLoadDataValid() override;

protected:
	virtual bool OnIsValid() override;
	virtual void OnCalcBoundingVolume() override;
	virtual void OnDataAcquired() override;
	virtual void OnLoaded() override;
	virtual void OnLoadedUpdate() override;
	virtual void OnWriteVertexData(HyVertexBuffer &vertexBufferRef) override;

private:
	void MarkAsDirty();
	void CalculateGlyphInfos();
};

#endif /* HyText2d_h__ */
