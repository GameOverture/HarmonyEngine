/**************************************************************************
 *	HyText2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Instances/HyText2d.h"

#include "Utilities/HyStrManip.h"
#include "freetype-gl/freetype-gl.h"

#include <iostream>

/*static*/ HyChar HyText2d::sm_pTempTextBuffer[HY_TEMP_TEXTBUFFER_SIZE] = { 0 };

HyText2d::HyText2d(const char *szPrefix, const char *szName) :	IObjInst2d(HYINST_Text2d, szPrefix, szName),
																m_pVertexBuffer(NULL),
																m_uiCurFontIndex(0)
{
	m_RenderState.Enable(HyRenderState::DRAWMODE_TRIANGLESTRIP | HyRenderState::SHADER_QUADBATCH);
}

HyText2d::~HyText2d(void)
{
}

void HyText2d::SetString(const HyChar *szString, ...)
{
	va_list vl;
	va_start(vl, szString);
#ifdef HY_UNICODE
	vswprintf(sm_pTempTextBuffer, HY_TEMP_TEXTBUFFER_SIZE, szString, vl);
#else
	vsprintf(sm_pTempTextBuffer, szString, vl);
#endif
	va_end(vl);

	if(m_sString == sm_pTempTextBuffer)
		return;
	m_sString = sm_pTempTextBuffer;

	CalcVertexBuffer();
}

void HyText2d::CalcVertexBuffer()
{
	if(m_eLoadState != HYLOADSTATE_Loaded)
		return;

	if(m_pVertexBuffer)
		delete [] m_pVertexBuffer;

	vec4 ptVerts[4];
	vec2 vUVs[4];
	vec2 ptPenPos(0.0f);

	size_t uiNumCharacters = m_sString.size();
	m_uiBufferSizeBytes = uiNumCharacters * (4 * (sizeof(vec4) + sizeof(vec2)));
	m_pVertexBuffer = new unsigned char[m_uiBufferSizeBytes];

	HyText2dData *pTextData = reinterpret_cast<HyText2dData *>(m_pDataPtr);

	unsigned char *pCurVertexWritePos = m_pVertexBuffer;
	for(size_t i = 0; i < uiNumCharacters; ++i)
	{
		texture_glyph_t *glyph = pTextData->GetGlyph(m_uiCurFontIndex, m_sString[i]);
		if( glyph != NULL )
		{
			float fKerning = 0;
			if( i > 0)
				fKerning = texture_glyph_get_kerning(glyph, m_sString[i-1]);

			ptPenPos.x += fKerning;

			float x0  = ( ptPenPos.x + glyph->offset_x );
			float y0  = ( ptPenPos.y + glyph->offset_y );
			float x1  = ( x0 + glyph->width );
			float y1  = ( y0 - glyph->height );
			float s0 = glyph->s0;
			float t0 = glyph->t0;
			float s1 = glyph->s1;
			float t1 = glyph->t1;

			ptVerts[0].x = x0;	ptVerts[0].y = y1;		ptVerts[0].z = 0.0f;	ptVerts[0].w = 1.0f;
			ptVerts[1].x = x0;	ptVerts[1].y = y0;		ptVerts[1].z = 0.0f;	ptVerts[1].w = 1.0f;
			ptVerts[2].x = x1;	ptVerts[2].y = y1;		ptVerts[2].z = 0.0f;	ptVerts[2].w = 1.0f;
			ptVerts[3].x = x1;	ptVerts[3].y = y0;		ptVerts[3].z = 0.0f;	ptVerts[3].w = 1.0f;

			vUVs[0].x = s0;		vUVs[0].y = t1;
			vUVs[1].x = s0;		vUVs[1].y = t0;
			vUVs[2].x = s1;		vUVs[2].y = t1;
			vUVs[3].x = s1;		vUVs[3].y = t0;

			for(int j = 0; j < 4; ++j)
			{
				memcpy(pCurVertexWritePos, &ptVerts[j], sizeof(vec4));
				pCurVertexWritePos += sizeof(vec4);
				memcpy(pCurVertexWritePos, &vUVs[j], sizeof(vec2));
				pCurVertexWritePos += sizeof(vec2);
			}

			ptPenPos.x += glyph->advance_x;
		}
	}
}

/*virtual*/ void HyText2d::Update()
{

}

/*virtual*/ void HyText2d::OnDataLoaded()
{
	HyText2dData *pTextData = reinterpret_cast<HyText2dData *>(m_pDataPtr);
	//m_uiCurFontSize = pTextData->GetMinFontSize();

	CalcVertexBuffer();
}

/*virtual*/ void HyText2d::WriteDrawBufferData(char *&pRefDataWritePos)
{

}
