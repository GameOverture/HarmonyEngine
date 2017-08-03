/**************************************************************************
 *	HyText2dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Nodes/HyText2dData.h"

#include "Renderer/IHyRenderer.h"

HyText2dData::FontState::FontState(std::string sName, Typeface *pTypefaces, float fLineHeight, float fLineAcender, float fLineDescender, float fLeftSideNudgeAmt, jsonxx::Array layersArray) :	sNAME(sName),
																																																fLINE_HEIGHT(fLineHeight),
																																																fLINE_ASCENDER(fLineAcender),
																																																fLINE_DESCENDER(fLineDescender),
																																																fLEFT_SIDE_NUDGE_AMT(fLeftSideNudgeAmt),
																																																uiNUM_LAYERS(static_cast<uint32>(layersArray.size()))
{
	pLayers = reinterpret_cast<Layer *>(HY_NEW unsigned char[sizeof(Layer) * uiNUM_LAYERS]);
	Layer *pLayerWriteLocation = pLayers;

	for(uint32 i = 0; i < uiNUM_LAYERS; ++i, ++pLayerWriteLocation)
	{
		jsonxx::Object layerObj = layersArray.get<jsonxx::Object>(i);
		new (pLayerWriteLocation)Layer(static_cast<float>(layerObj.get<jsonxx::Number>("topR")),
									   static_cast<float>(layerObj.get<jsonxx::Number>("topG")),
									   static_cast<float>(layerObj.get<jsonxx::Number>("topB")),
									   static_cast<float>(layerObj.get<jsonxx::Number>("botR")),
									   static_cast<float>(layerObj.get<jsonxx::Number>("botG")),
									   static_cast<float>(layerObj.get<jsonxx::Number>("botB")),
									   pTypefaces[static_cast<uint32>(layerObj.get<jsonxx::Number>("typefaceIndex"))]);
	}
}

HyText2dData::FontState::~FontState()
{
	unsigned char *pLayerBuffer = reinterpret_cast<unsigned char *>(pLayers);
	delete[] pLayerBuffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyText2dData::HyText2dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef) :	IHyNodeData(HYTYPE_Text2d, sPath),
																												m_pTypefaces(NULL),
																												m_uiNumTypefaces(0),
																												m_pFontStates(NULL),
																												m_uiNumStates(0)
{
	jsonxx::Object textObject = dataValueRef.get<jsonxx::Object>();

	HyRectangle<float> rSubAtlasUVRect;
	m_pAtlas = assetsRef.GetAtlas(static_cast<uint32>(textObject.get<jsonxx::Number>("checksum")), rSubAtlasUVRect);

	if(m_pAtlas == nullptr)
		return;

	m_RequiredAtlasIndices.Set(m_pAtlas->GetMasterIndex());

	float fSubAtlasWidth = m_pAtlas->GetWidth() * (rSubAtlasUVRect.right - rSubAtlasUVRect.left);
	float fSubAtlasHeight = m_pAtlas->GetHeight() * (rSubAtlasUVRect.bottom - rSubAtlasUVRect.top);
	
	jsonxx::Array typefaceArray = textObject.get<jsonxx::Array>("typefaceArray");
	m_uiNumTypefaces = static_cast<uint32>(typefaceArray.size());
	
	m_pTypefaces = HY_NEW Typeface[m_uiNumTypefaces];
	for (uint32 i = 0; i < m_uiNumTypefaces; ++i)
	{
		Typeface &curTypeface = m_pTypefaces[i];

		jsonxx::Object typefaceObj = typefaceArray.get<jsonxx::Object>(i);
		jsonxx::Array glyphsArray = typefaceObj.get<jsonxx::Array>("glyphs");
		
		uint32 uiNumGlyphs = static_cast<uint32>(glyphsArray.size());
		for (uint32 j = 0; j < uiNumGlyphs; ++j)
		{
			jsonxx::Object glyphObj = glyphsArray.get<jsonxx::Object>(j);

			float fLeftUv = (m_pAtlas->GetWidth() * rSubAtlasUVRect.left) + (fSubAtlasWidth * static_cast<float>(glyphObj.get<jsonxx::Number>("left")));
			fLeftUv /= m_pAtlas->GetWidth();
			float fTopUv = (m_pAtlas->GetHeight() * rSubAtlasUVRect.top) + (fSubAtlasHeight * static_cast<float>(glyphObj.get<jsonxx::Number>("top")));
			fTopUv /= m_pAtlas->GetHeight();
			float fRightUv = (m_pAtlas->GetWidth() * rSubAtlasUVRect.left) + (fSubAtlasWidth * static_cast<float>(glyphObj.get<jsonxx::Number>("right")));
			fRightUv /= m_pAtlas->GetWidth();
			float fBottomUv = (m_pAtlas->GetHeight() * rSubAtlasUVRect.top) + (fSubAtlasHeight * static_cast<float>(glyphObj.get<jsonxx::Number>("bottom")));
			fBottomUv /= m_pAtlas->GetHeight();

			curTypeface[static_cast<uint32>(glyphObj.get<jsonxx::Number>("code"))] = HY_NEW HyText2dGlyphInfo(static_cast<uint32>(glyphObj.get<jsonxx::Number>("width")),
																											  static_cast<uint32>(glyphObj.get<jsonxx::Number>("height")),
																											  static_cast<uint32>(glyphObj.get<jsonxx::Number>("offset_x")),
																											  static_cast<uint32>(glyphObj.get<jsonxx::Number>("offset_y")),
																											  static_cast<float>(glyphObj.get<jsonxx::Number>("advance_x")),
																											  static_cast<float>(glyphObj.get<jsonxx::Number>("advance_y")),
																											  fLeftUv,
																											  fTopUv,
																											  fRightUv,
																											  fBottomUv,
																											  glyphObj.get<jsonxx::Object>("kerning"));
		}
	}

	jsonxx::Array stateArray = textObject.get<jsonxx::Array>("stateArray");
	m_uiNumStates = static_cast<uint32>(stateArray.size());

	m_pFontStates = reinterpret_cast<FontState *>(HY_NEW unsigned char[sizeof(FontState) * m_uiNumStates]);
	FontState *pStateWriteLocation = m_pFontStates;
	for (uint32 i = 0; i < stateArray.size(); ++i, ++pStateWriteLocation)
	{
		jsonxx::Object stateObj = stateArray.get<jsonxx::Object>(i);
		new (pStateWriteLocation)FontState(stateObj.get<jsonxx::String>("name"),
										   m_pTypefaces,
										   static_cast<float>(stateObj.get<jsonxx::Number>("lineHeight")),
										   static_cast<float>(stateObj.get<jsonxx::Number>("lineAscender")),
										   static_cast<float>(stateObj.get<jsonxx::Number>("lineDescender")),
										   static_cast<float>(stateObj.get<jsonxx::Number>("leftSideNudgeAmt")),
										   stateObj.get<jsonxx::Array>("layers"));
	}
}

HyText2dData::~HyText2dData(void)
{
	for(uint32 i = 0; i < m_uiNumStates; ++i)
		m_pFontStates[i].~FontState();
	unsigned char *pFontStatesBuffer = reinterpret_cast<unsigned char *>(m_pFontStates);
	delete[] pFontStatesBuffer;

	for(uint32 i = 0; i < m_uiNumTypefaces; ++i)
	{
		for(auto iter = m_pTypefaces[i].begin(); iter != m_pTypefaces[i].end(); ++iter)
			delete iter->second;
	}
	delete[] m_pTypefaces;
}

uint32 HyText2dData::GetNumStates()
{
	return m_uiNumStates;
}

uint32 HyText2dData::GetNumLayers(uint32 uiStateIndex)
{
	return m_pFontStates[uiStateIndex].uiNUM_LAYERS;
}

const HyText2dGlyphInfo &HyText2dData::GetGlyph(uint32 uiStateIndex, uint32 uiLayerIndex, uint32 uiUtf32Code)
{
	HyAssert(m_pFontStates[uiStateIndex].pLayers[uiLayerIndex].TYPEFACE_REF.find(uiUtf32Code) != m_pFontStates[uiStateIndex].pLayers[uiLayerIndex].TYPEFACE_REF.end(), "HyText2d tried to draw a glyph (UTF-32: " << uiUtf32Code << ") that wasn't exported");
	return *m_pFontStates[uiStateIndex].pLayers[uiLayerIndex].TYPEFACE_REF.at(uiUtf32Code);
}

const glm::vec3 &HyText2dData::GetDefaultColor(uint32 uiStateIndex, uint32 uiLayerIndex, bool bTop)
{
	if(bTop)
		return m_pFontStates[uiStateIndex].pLayers[uiLayerIndex].vDEFAULT_TOP_COLOR;
	else
		return m_pFontStates[uiStateIndex].pLayers[uiLayerIndex].vDEFAULT_BOT_COLOR;
}

HyAtlas *HyText2dData::GetAtlas()
{
	return m_pAtlas;
}

float HyText2dData::GetLineHeight(uint32 uiStateIndex)
{
	return m_pFontStates[uiStateIndex].fLINE_HEIGHT;
}

float HyText2dData::GetLineAscender(uint32 uiStateIndex)
{
	return m_pFontStates[uiStateIndex].fLINE_ASCENDER;
}

float HyText2dData::GetLineDescender(uint32 uiStateIndex)
{
	return m_pFontStates[uiStateIndex].fLINE_DESCENDER;
}

float HyText2dData::GetLeftSideNudgeAmt(uint32 uiStateIndex)
{
	if(m_pFontStates)
		return m_pFontStates[uiStateIndex].fLEFT_SIDE_NUDGE_AMT;

	return 0.0f;
}
