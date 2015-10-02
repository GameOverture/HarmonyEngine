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

#include "FileIO/Data/IHyData.h"

#include <map>
using std::map;

struct _texture_font_t;
struct _texture_glyph_t;

class HyText2d;

class HyText2dData : public IHyData
{
	friend class HyFactory<HyText2dData>;

	vector<_texture_font_t *>	m_vFonts;
	HyTexture *					m_pTexture;

	// Only allow HyFactory instantiate
	HyText2dData(const std::string &sPath);

public:
	virtual ~HyText2dData(void);

	HyTexture *GetTexturePtr()								{ return m_pTexture; }

	_texture_glyph_t *GetGlyph(uint32 uiFontIndex, wchar_t charcode);

	virtual void DoFileLoad();
	virtual void OnGfxLoad(IHyRenderer &gfxApi);
	virtual void OnGfxRemove(IHyRenderer &gfxApi);
};

#endif /* __HyText2dData_h__ */
