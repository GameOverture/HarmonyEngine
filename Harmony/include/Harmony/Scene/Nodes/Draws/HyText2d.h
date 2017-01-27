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

#include "Scene/Nodes/Draws/IHyDraw2d.h"

#include "Assets/Data/HyText2dData.h"

#include <vector>

class HyText2d : public IHyDraw2d
{
protected:
	bool							m_bIsDirty;
	std::string						m_sCurrentString;
	std::string						m_sNewString;

	uint32							m_uiCurFontState;

	struct StateColors
	{
		struct LayerColor
		{
			HyTweenVec3 		topColor;
			HyTweenVec3 		botColor;

			LayerColor(IHyNode &colorOwner) :	topColor(colorOwner),
														botColor(colorOwner)
			{ }
		};
		std::vector<LayerColor *>	m_LayerColors;
	};
	std::vector<StateColors *>		m_StateColors;

	enum eBoxAttributes
	{
		BOXATTRIB_IsUsed			= 1 << 0,
		BOXATTRIB_CenterVertically	= 1 << 1,
		BOXATTRIB_ExtendingBottom	= 1 << 2,
		BOXATTRIB_SplitWordsToFit	= 1 << 3,
		BOXATTRIB_ScaleBox			= 1 << 4
	};
	uint32				m_uiBoxAttributes;
	glm::vec2			m_vBoxDimensions;
	float				m_fScaleBoxModifier;

	HyAlign				m_eAlignment;

	glm::vec2 *			m_pGlyphOffsets;
	uint32				m_uiNumReservedGlyphOffsets;

	uint32				m_uiNumValidCharacters;

public:
	HyText2d(const char *szPrefix, const char *szName);
	virtual ~HyText2d(void);

	// Accepts newline characters '\n'
	void TextSet(std::string sText);
	std::string TextGet();

	uint32 TextGetLength();

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
	void SetAsColumn(float fWidth, bool bSplitWordsToFit = false);
	void SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically = true);

protected:
	virtual void OnDataLoaded() override;

	virtual void OnUpdate() override;

	virtual void OnUpdateUniforms() override;
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) override;
};

#endif /* __HyText2d_h__ */
