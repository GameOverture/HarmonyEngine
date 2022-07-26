/**************************************************************************
 *	HyTextData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTextData_h__
#define HyTextData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

/*
* Glyph metrics:
* --------------
*
*                       xmin                     xmax
*                        |                         |
*                        |<-------- width -------->|
*                        |                         |
*              |         +-------------------------+----------------- ymax
*              |         |    ggggggggg   ggggg    |     ^        ^
*              |         |   g:::::::::ggg::::g    |     |        |
*              |         |  g:::::::::::::::::g    |     |        |
*              |         | g::::::ggggg::::::gg    |     |        |
*              |         | g:::::g     g:::::g     |     |        |
*    offset_x -|-------->| g:::::g     g:::::g     |  offset_y    |
*              |         | g:::::g     g:::::g     |     |        |
*              |         | g::::::g    g:::::g     |     |        |
*              |         | g:::::::ggggg:::::g     |     |        |
*              |         |  g::::::::::::::::g     |     |      height
*              |         |   gg::::::::::::::g     |     |        |
*  baseline ---*---------|---- gggggggg::::::g-----*--------      |
*            / |         |             g:::::g     |              |
*     origin   |         | gggggg      g:::::g     |              |
*              |         | g:::::gg   gg:::::g     |              |
*              |         |  g::::::ggg:::::::g     |              |
*              |         |   gg:::::::::::::g      |              |
*              |         |     ggg::::::ggg        |              |
*              |         |         gggggg          |              v
*              |         +-------------------------+----------------- ymin
*              |                                   |
*              |------------- advance_x ---------->|
*/
struct HyTextGlyph
{
	const uint32				uiWIDTH;
	const uint32				uiHEIGHT;

	const int					iOFFSET_X;
	const int					iOFFSET_Y;

	const float					fADVANCE_X;
	const float					fADVANCE_Y;

	const HyRectangle<float>	rSRC_RECT;

	HyTextGlyph(uint32 uiWidth,
				uint32 uiHeight,
				int iOffsetX,
				int iOffsetY,
				float fAdvanceX,
				float fAdvanceY,
				float fSrcLeft,
				float fSrcTop,
				float fSrcRight,
				float fSrcBot) :
		uiWIDTH(uiWidth),
		uiHEIGHT(uiHeight),
		iOFFSET_X(iOffsetX),
		iOFFSET_Y(iOffsetY),
		fADVANCE_X(fAdvanceX),
		fADVANCE_Y(fAdvanceY),
		rSRC_RECT(fSrcLeft, fSrcBot, fSrcRight, fSrcTop)
	{
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HyTextData : public IHyNodeData
{
	typedef std::map<uint32, HyTextGlyph *> Typeface;

	HyFileAtlas *					m_pAtlas;

	Typeface *						m_pTypefaces;
	uint32							m_uiNumTypefaces;

	struct FontState
	{
		const uint32				uiNUM_LAYERS;
		const float					fLINE_HEIGHT;
		const float					fLINE_ASCENDER;
		const float					fLINE_DESCENDER;
		const float					fLEFT_SIDE_NUDGE_AMT;

		struct Layer
		{
			const HyColor			DEFAULT_TOP_COLOR;
			const HyColor			DEFAULT_BOT_COLOR;
			const Typeface &		TYPEFACE_REF;

			Layer(float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB, Typeface &typefaceRef) :
				DEFAULT_TOP_COLOR(fTopR, fTopG, fTopB),
				DEFAULT_BOT_COLOR(fBotR, fBotG, fBotB),
				TYPEFACE_REF(typefaceRef)
			{ }
		};
		Layer *						pLayers;

		FontState(Typeface *pTypefaces, float fLineGap, float fLineAcender, float fLineDescender, float fLeftSideNudgeAmt, HyJsonArray layersArray);
		~FontState();
	};
	FontState *						m_pFontStates;

public:
	HyTextData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef);
	virtual ~HyTextData();

	uint32 GetNumLayers(uint32 uiStateIndex) const;
	const HyTextGlyph *GetGlyph(uint32 uiStateIndex, uint32 uiLayerIndex, uint32 uiUtf32Code) const;
	HyColor GetDefaultColor(uint32 uiStateIndex, uint32 uiLayerIndex, bool bTop) const;
	HyFileAtlas *GetAtlas() const;
	float GetLineHeight(uint32 uiStateIndex) const;
	float GetLineAscender(uint32 uiStateIndex) const;
	float GetLineDescender(uint32 uiStateIndex) const;
	float GetLeftSideNudgeAmt(uint32 uiStateIndex) const;
};

#endif /* HyTextData_h__ */
