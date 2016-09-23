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

#include "freetype-gl/freetype-gl.h"

#include <map>
using std::map;

class HyText2d;

class HyText2dData : public IHy2dData
{
	friend class HyFactory<HyText2dData>;

	vector<ftgl::texture_font_t *>	m_vFonts;

	// Only allow HyFactory instantiate
	HyText2dData(const std::string &sPath, int32 iShaderId);

public:
	virtual ~HyText2dData();


	ftgl::texture_glyph_t *GetGlyph(uint32 uiFontIndex, const char *pCodePoint);

	virtual void DoFileLoad();
};

#endif /* __HyText2dData_h__ */
