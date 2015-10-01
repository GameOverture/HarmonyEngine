/**************************************************************************
 *	HyText2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Data/HyText2dData.h"
#include "Renderer/IHyRenderer.h"

#include "freetype-gl/freetype-gl.h"

HyText2dData::HyText2dData(const std::string &sPath) :	IHyData(HYINST_Text2d, sPath)
{
}

HyText2dData::~HyText2dData(void)
{
	for(uint32 i = 0; i < m_vFonts.size(); ++i)
		texture_font_delete(m_vFonts[i]);

	m_vFonts.clear();
}

/*virtual*/ void HyText2dData::DoFileLoad()
{
	// If path is empty, then use default engine font
	if(m_ksPath.empty())
	{
		std::string sFontName = "./data/fonts/Vera.ttf";
		uint32 uiMaxSize = 20;
		uint32 uiMinSize = 16;
		const wchar_t *szCharCodes = L" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

		/* Texture atlas to store individual glyphs */
		texture_atlas_t *atlas = texture_atlas_new(512, 512, 1);
		uint32 uiNumFonts = uiMaxSize - uiMinSize;

		if(uiNumFonts == 0)
			uiNumFonts = 1;
		size_t missed = 0;

		for(uint32 i = 0; i < uiNumFonts; ++i)
		{
			/* Build a new texture font from its description and size */
			m_vFonts.push_back(texture_font_new_from_file(atlas, static_cast<float>(i + uiMinSize), sFontName.c_str()));
			m_vFonts[m_vFonts.size() - 1]->hinting = 0;
			m_vFonts[m_vFonts.size() - 1]->outline_type = 1;
			m_vFonts[m_vFonts.size() - 1]->outline_thickness = 2;
			missed += texture_font_load_glyphs(m_vFonts[m_vFonts.size() - 1], szCharCodes);
		}
		HyAssert(missed == 0, "Default engine font does not fit on texture");

		std::ostringstream stringStream;
		stringStream << "[" << uiMinSize << "-" << uiMaxSize << "]";
		sFontName += stringStream.str();

		HyError("Text2dData's DoFileLoad() is not implemented");
		//m_pTexture = LoadTexture(sFontName.c_str(), static_cast<int32>(atlas->width), static_cast<int32>(atlas->height), static_cast<int32>(atlas->depth), atlas->data);

		printf( "Matched pFont               : %s\n", sFontName.c_str() );
		printf( "Number of fonts            : %ld\n", uiNumFonts );
		printf( "Number of glyphs per pFont  : %ld\n", wcslen(szCharCodes) );
		printf( "Number of missed glyphs    : %ld\n", missed );
		printf( "Total number of glyphs     : %ld/%ld\n",
			wcslen(szCharCodes)*uiNumFonts - missed, wcslen(szCharCodes)*uiNumFonts );
		printf( "Texture size               : %ldx%ld\n", atlas->width, atlas->height );
		printf( "Texture occupancy          : %.2f%%\n", 
			100.0*atlas->used/(float)(atlas->width*atlas->height) );

			/* Where to start printing on screen */
		//	vec2 pen = {0,0};
		//vec4 black = {0,0,0,1};

		/* Add text tothe buffer (see demo-pFont.c for the add_text code) */
		//add_text( buffer, pFont, "Hello World !", text, &black, &pen );
	}
}

_texture_glyph_t *HyText2dData::GetGlyph(uint32 uiFontIndex, wchar_t charcode)
{
	HyAssert(uiFontIndex >= 0 && uiFontIndex < m_vFonts.size(), "HyText2dData::GetGlyph() passed an invalid font index");

	return texture_font_get_glyph(m_vFonts[uiFontIndex], charcode);
}

/*virtual*/ void HyText2dData::OnGfxLoad(IHyRenderer &gfxApi)
{
	m_pTexture->Upload(gfxApi);
}

/*virtual*/ void HyText2dData::OnGfxRemove(IHyRenderer &gfxApi)
{
	gfxApi.DeleteTexture(*m_pTexture);
}
