/**************************************************************************
 *	HyText2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Data/HyText2dData.h"

#include "Renderer/IHyRenderer.h"

HyText2dData::HyText2dData(const std::string &sPath, int32 iShaderId) : IHy2dData(HYINST_Text2d, sPath, iShaderId)
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
	texture_atlas_t *atlas = texture_atlas_new(512, 512, 1);
	const char *filename = "fonts/Vera.ttf";
	const char * cache = " !\"#$%&'()*+,-./0123456789:;<=>?"
		"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
		"`abcdefghijklmnopqrstuvwxyz{|}~";
	size_t minsize = 8, maxsize = 27;
	size_t count = maxsize - minsize;
	size_t i, missed = 0;

	for(i = minsize; i < maxsize; ++i)
	{
		texture_font_t * font = texture_font_new_from_file(atlas, static_cast<const float>(i), filename);
		missed += texture_font_load_glyphs(font, cache);
		texture_font_delete(font);
	}

	printf("Matched font               : %s\n", filename);
	printf("Number of fonts            : %ld\n", count);
	printf("Number of glyphs per font  : %ld\n", strlen(cache));
	printf("Number of missed glyphs    : %ld\n", missed);
	printf("Total number of glyphs     : %ld/%ld\n",
		strlen(cache)*count - missed, strlen(cache)*count);
	printf("Texture size               : %ldx%ld\n", atlas->width, atlas->height);
	printf("Texture occupancy          : %.2f%%\n",
		100.0*atlas->used / (float)(atlas->width*atlas->height));

	glClearColor(1, 1, 1, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenTextures(1, &atlas->id);
	glBindTexture(GL_TEXTURE_2D, atlas->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, static_cast<GLsizei>(atlas->width), static_cast<GLsizei>(atlas->height),
		0, GL_RED, GL_UNSIGNED_BYTE, atlas->data);

	//typedef struct { float x, y, z, u, v, r, g, b, a; } vertex_t;
	//vertex_t vertices[4] = {
	//	{ 0, 0, 0, 0, 1, 0, 0, 0, 1 },
	//	{ 0, 512, 0, 0, 0, 0, 0, 0, 1 },
	//	{ 512, 512, 0, 1, 0, 0, 0, 0, 1 },
	//	{ 512, 0, 0, 1, 1, 0, 0, 0, 1 } };
	//GLuint indices[6] = { 0, 1, 2, 0, 2, 3 };
	//vertex_buffer_t *buffer = vertex_buffer_new("vertex:3f,tex_coord:2f,color:4f");
	//vertex_buffer_push_back(buffer, vertices, 4, indices, 6);

	//shader = shader_load("shaders/v3f-t2f-c4f.vert",
	//	"shaders/v3f-t2f-c4f.frag");
	//mat4_set_identity(&projection);
	//mat4_set_identity(&model);
	//mat4_set_identity(&view);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//// If path is empty, then use default engine font
	//if(GetPath().empty())
	//{
	//	std::string sFontName = "./data/fonts/Vera.ttf";
	//	uint32 uiMaxSize = 20;
	//	uint32 uiMinSize = 16;
	//	const char *szCharCodes = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

	//	/* Texture atlas to store individual glyphs */
	//	texture_atlas_t *atlas = texture_atlas_new(512, 512, 1);
	//	uint32 uiNumFonts = uiMaxSize - uiMinSize;

	//	if(uiNumFonts == 0)
	//		uiNumFonts = 1;
	//	size_t missed = 0;

	//	for(uint32 i = 0; i < uiNumFonts; ++i)
	//	{
	//		/* Build a new texture font from its description and size */
	//		m_vFonts.push_back(texture_font_new_from_file(atlas, static_cast<float>(i + uiMinSize), sFontName.c_str()));
	//		m_vFonts[m_vFonts.size() - 1]->hinting = 0;
	//		m_vFonts[m_vFonts.size() - 1]->outline_type = 1;
	//		m_vFonts[m_vFonts.size() - 1]->outline_thickness = 2;
	//		missed += texture_font_load_glyphs(m_vFonts[m_vFonts.size() - 1], szCharCodes);
	//	}
	//	HyAssert(missed == 0, "Default engine font does not fit on texture");

	//	std::ostringstream stringStream;
	//	stringStream << "[" << uiMinSize << "-" << uiMaxSize << "]";
	//	sFontName += stringStream.str();

	//	HyError("Text2dData's DoFileLoad() is not implemented");
	//	//m_pTexture = LoadTexture(sFontName.c_str(), static_cast<int32>(atlas->width), static_cast<int32>(atlas->height), static_cast<int32>(atlas->depth), atlas->data);

	//	printf( "Matched pFont               : %s\n", sFontName.c_str() );
	//	printf( "Number of fonts            : %ld\n", uiNumFonts );
	//	printf( "Number of glyphs per pFont  : %ld\n", wcslen(szCharCodes) );
	//	printf( "Number of missed glyphs    : %ld\n", missed );
	//	printf( "Total number of glyphs     : %ld/%ld\n",
	//		wcslen(szCharCodes)*uiNumFonts - missed, wcslen(szCharCodes)*uiNumFonts );
	//	printf( "Texture size               : %ldx%ld\n", atlas->width, atlas->height );
	//	printf( "Texture occupancy          : %.2f%%\n", 
	//		100.0*atlas->used/(float)(atlas->width*atlas->height) );

	//		/* Where to start printing on screen */
	//	//	vec2 pen = {0,0};
	//	//vec4 black = {0,0,0,1};

	//	/* Add text tothe buffer (see demo-pFont.c for the add_text code) */
	//	//add_text( buffer, pFont, "Hello World !", text, &black, &pen );
	//}
}

texture_glyph_t *HyText2dData::GetGlyph(uint32 uiFontIndex, const char *pCodePoint)
{
	HyAssert(uiFontIndex >= 0 && uiFontIndex < m_vFonts.size(), "HyText2dData::GetGlyph() passed an invalid font index");

	return texture_font_get_glyph(m_vFonts[uiFontIndex], pCodePoint);
}

