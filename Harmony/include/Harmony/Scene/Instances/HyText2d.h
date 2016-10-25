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

#include "Scene/Instances/IHyInst2d.h"

#include "Assets/Data/HyText2dData.h"

class HyText2d : public IHyInst2d
{
protected:
	bool				m_bIsDirty;
	std::string			m_sString;

	uint32				m_uiCurFontState;

	struct StateColors
	{
		struct LayerColor
		{
			HyAnimVec3 		topColor;
			HyAnimVec3 		botColor;
		};
		vector<LayerColor *>	m_LayerColors;
	};
	vector<StateColors *>	m_StateColors;

	enum eBoxAttributes
	{
		BOXATTRIB_IsUsed			= 1 << 0,
		BOXATTRIB_CenterVertically	= 1 << 1,
		BOXATTRIB_ExtendingBottom	= 1 << 2,
		BOXATTRIB_SplitWordsToFit	= 1 << 3,
		BOXATTRIB_SingleLineScale	= 1 << 4
	};
	uint32				m_uiBoxAttributes;
	glm::vec2			m_vBoxDimensions;

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

	std::pair<HyAnimVec3 &, HyAnimVec3 &> TextGetLayerColor(uint32 uiLayerIndex);
	std::pair<HyAnimVec3 &, HyAnimVec3 &> TextGetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex);
	void TextSetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);
	void TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fR, float fG, float fB);
	void TextSetLayerColor(uint32 uiLayerIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB);
	void TextSetLayerColor(uint32 uiLayerIndex, uint32 uiStateIndex, float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB);

	HyAlign TextGetAlignment();
	void TextSetAlignment(HyAlign eAlignment);

	const glm::vec2 &TextGetBox();

	void SetAsLine();
	void SetAsBox(float fWidth, float fHeight, bool bCenterVertically = false, bool bExtendingBottom = false, bool bDontSplitWordsToFit = false);
	void SetAsScaleBox();

private:
	virtual void OnDataLoaded();

	virtual void OnInstUpdate();

	virtual void OnUpdateUniforms(HyShaderUniforms *pShaderUniformsRef);
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos);
};

#endif /* __HyText2d_h__ */
