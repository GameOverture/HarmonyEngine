/**************************************************************************
 *	HyText2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyText2d_h__
#define __HyText2d_h__

#include "Scene/Nodes/Leafs/IHyLeafDraw2d.h"
#include "Assets/Nodes/HyText2dData.h"

#include <vector>

class HyText2d : public IHyLeafDraw2d
{
protected:
	bool							m_bIsDirty;
	std::string						m_sCurrentString;

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

	enum eBoxAttributes
	{
		BOXATTRIB_IsColumn				= 1 << 0,
		BOXATTRIB_CenterVertically		= 1 << 1,
		BOXATTRIB_ExtendingBottom		= 1 << 2,
		BOXATTRIB_SplitWordsToFit		= 1 << 3,
		BOXATTRIB_IsScaleBox			= 1 << 4,
		BOXATTRIB_FitWithinBounds		= 1 << 5,	// Whether any fancy glyph is allowed to hang outside the bounds
		BOXATTRIB_PreserveGlyphRatios	= 1 << 6
		//BOXATTRIB_DigitBox
	};
	uint32				m_uiBoxAttributes;
	glm::vec2			m_vBoxDimensions;
	float				m_fScaleBoxModifier;

	HyAlign				m_eAlignment;

	glm::vec2 *			m_pGlyphOffsets;
	uint32				m_uiNumReservedGlyphOffsets;

	uint32				m_uiNumValidCharacters;

	float				m_fUsedPixelWidth;

public:
	HyText2d(const char *szPrefix, const char *szName, HyEntity2d *pParent = nullptr);
	virtual ~HyText2d(void);

	// Assumes UTF-8 encoding. Accepts newline characters '\n'
	void TextSet(const std::string &sTextRef);
	const std::string &TextGet() const;

	uint32 TextGetStrLength();
	float TextGetScaleBoxModifer();

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

	const glm::vec2 &TextGetBox();

	void SetAsLine();
	void SetAsColumn(float fWidth, bool bMustFitWithinColumn, bool bSplitWordsToFit = false);
	void SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically = true, bool bPreserveGlyphRatios = false);

protected:
	virtual void DrawUpdate() override;
	virtual void OnDataAcquired() override;
	virtual void OnLoaded() override;
	virtual void OnUpdateUniforms() override;
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) override;
};

#endif /* __HyText2d_h__ */
