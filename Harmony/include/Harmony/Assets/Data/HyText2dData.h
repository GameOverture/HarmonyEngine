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

#include "Assets/Data/IHy2dData.h"
#include "Assets/HyFactory.h"

#include <map>
using std::map;

class HyText2d;

class HyText2dData : public IHy2dData
{
	friend class HyFactory<HyText2dData>;

	struct GlyphInfo
	{
		const uint32				uiWIDTH;
		const uint32				uiHEIGHT;

		const int					iOFFSET_X;
		const int					iOFFSET_Y;

		const float					fADVANCE_X;
		const float					fADVANCE_Y;

		const HyRectangle<float>	rSRC_RECT;

		std::map<uint32, float>		kerningMap;

		GlyphInfo(uint32 uiWidth,
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
	typedef std::map<uint32, GlyphInfo *> Typeface;

	Typeface *						m_pTypefaces;
	uint32							m_uiNumTypefaces;

	struct FontState
	{
		const std::string			m_sNAME;

		struct Layer
		{
			const glm::vec4			m_vDEFAULT_TOP_COLOR;
			const glm::vec4			m_vDEFAULT_BOT_COLOR;
			const Typeface &		m_TYPEFACE_REF;
		};
		Layer *						m_pLayers;
		uint32						m_uiNumLayers;

		FontState(std::string sName) : m_sNAME(sName)
		{ }
	};
	FontState *						m_pFontStates;
	uint32							m_uiNumStates;

	// Only allow HyFactory instantiate
	HyText2dData(const std::string &sPath, int32 iShaderId);

public:
	virtual ~HyText2dData();

	virtual void DoFileLoad();
};

#endif /* __HyText2dData_h__ */
