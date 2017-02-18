/**************************************************************************
 *	HyText2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyText2dData_h__
#define __HyText2dData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Data/HyDataDraw.h"
#include "Assets/HyNodeDataContainer.h"

#include <map>
using std::map;

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
struct HyText2dGlyphInfo
{
	const uint32				uiWIDTH;
	const uint32				uiHEIGHT;

	const int					iOFFSET_X;
	const int					iOFFSET_Y;

	const float					fADVANCE_X;
	const float					fADVANCE_Y;

	const HyRectangle<float>	rSRC_RECT;

	std::map<uint32, float>		kerningMap;

	HyText2dGlyphInfo(uint32 uiWidth,
					  uint32 uiHeight,
					  int iOffsetX,
					  int iOffsetY,
					  float fAdvanceX,
					  float fAdvanceY,
					  float fSrcLeft,
					  float fSrcTop,
					  float fSrcRight,
					  float fSrcBot,
					  jsonxx::Object &kerningObj) :	uiWIDTH(uiWidth),
													uiHEIGHT(uiHeight),
													iOFFSET_X(iOffsetX),
													iOFFSET_Y(iOffsetY),
													fADVANCE_X(fAdvanceX),
													fADVANCE_Y(fAdvanceY),
													rSRC_RECT(fSrcLeft, fSrcTop, fSrcRight, fSrcBot)
	{
		kerningMap;
	}
};

class HyText2dData : public HyDataDraw
{
	friend class HyNodeDataContainer<HyText2dData>;

	typedef std::map<uint32, HyText2dGlyphInfo *> Typeface;

	uint32							m_uiAtlasGroupTextureIndex;
	HyAtlasGroup *					m_pAtlasGroup;

	Typeface *						m_pTypefaces;
	uint32							m_uiNumTypefaces;

	struct FontState
	{
		const std::string			sNAME;
		const uint32				uiNUM_LAYERS;
		const float					fLINE_HEIGHT;
		const float					fLINE_ASCENDER;
		const float					fLINE_DESCENDER;
		const float					fLEFT_SIDE_NUDGE_AMT;

		struct Layer
		{
			const glm::vec3			vDEFAULT_TOP_COLOR;
			const glm::vec3			vDEFAULT_BOT_COLOR;
			const Typeface &		TYPEFACE_REF;

			Layer(float fTopR, float fTopG, float fTopB, float fBotR, float fBotG, float fBotB, Typeface &typefaceRef) :	vDEFAULT_TOP_COLOR(fTopR, fTopG, fTopB),
																															vDEFAULT_BOT_COLOR(fBotR, fBotG, fBotB),
																															TYPEFACE_REF(typefaceRef)
			{ }
		};
		Layer *						pLayers;

		FontState(std::string sName, Typeface *pTypefaces, float fLineGap, float fLineAcender, float fLineDescender, float fLeftSideNudgeAmt, jsonxx::Array layersArray);
		~FontState();
	};
	FontState *						m_pFontStates;
	uint32							m_uiNumStates;

	// Only allow HyNodeDataContainer instantiate
	HyText2dData(const std::string &sPath, int32 iShaderId);

public:
	virtual ~HyText2dData();

	uint32 GetNumStates();
	uint32 GetNumLayers(uint32 uiStateIndex);
	const HyText2dGlyphInfo &GetGlyph(uint32 uiStateIndex, uint32 uiLayerIndex, uint32 uiCode);
	const glm::vec3 &GetDefaultColor(uint32 uiStateIndex, uint32 uiLayerIndex, bool bTop);
	HyAtlasGroup *GetAtlasGroup();
	uint32 GetAtlasGroupTextureIndex();
	float GetLineHeight(uint32 uiStateIndex);
	float GetLineAscender(uint32 uiStateIndex);
	float GetLineDescender(uint32 uiStateIndex);
	float GetLeftSideNudgeAmt(uint32 uiStateIndex);

	virtual void DoFileLoad() override;
};

#endif /* __HyText2dData_h__ */
