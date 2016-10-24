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

HyText2dData::FontState::FontState(std::string sName, Typeface *pTypefaces, jsonxx::Array layersArray) :	sNAME(sName),
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

HyText2dData::HyText2dData(const std::string &sPath, int32 iShaderId) : IHy2dData(HYINST_Text2d, sPath, iShaderId),
																		m_pTypefaces(NULL),
																		m_uiNumTypefaces(0),
																		m_pFontStates(NULL),
																		m_uiNumStates(0)
{
}

HyText2dData::~HyText2dData(void)
{
	for(uint32 i = 0; i < m_uiNumStates; ++i)
		m_pFontStates[i].~FontState();
	unsigned char *pFontStatesBuffer = reinterpret_cast<unsigned char *>(m_pFontStates);
	delete[] pFontStatesBuffer;

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

const HyText2dGlyphInfo &HyText2dData::GetGlyph(uint32 uiStateIndex, uint32 uiLayerIndex, uint32 uiCode)
{
	return *m_pFontStates[uiStateIndex].pLayers[uiLayerIndex].TYPEFACE_REF.at(uiCode);
}

const glm::vec3 &HyText2dData::GetDefaultColor(uint32 uiStateIndex, uint32 uiLayerIndex, bool bTop)
{
	if(bTop)
		return m_pFontStates[uiStateIndex].pLayers[uiLayerIndex].vDEFAULT_TOP_COLOR;
	else
		return m_pFontStates[uiStateIndex].pLayers[uiLayerIndex].vDEFAULT_BOT_COLOR;
}

/*virtual*/ void HyText2dData::DoFileLoad()
{
	std::string sFontFileContents;
	HyReadTextFile(GetPath().c_str(), sFontFileContents);

	jsonxx::Object textObject;
	textObject.parse(sFontFileContents);
	
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

			curTypeface[static_cast<uint32>(glyphObj.get<jsonxx::Number>("code"))] = HY_NEW HyText2dGlyphInfo(static_cast<uint32>(glyphObj.get<jsonxx::Number>("width")),
																											  static_cast<uint32>(glyphObj.get<jsonxx::Number>("height")),
																											  static_cast<uint32>(glyphObj.get<jsonxx::Number>("offset_x")),
																											  static_cast<uint32>(glyphObj.get<jsonxx::Number>("offset_y")),
																											  static_cast<float>(glyphObj.get<jsonxx::Number>("advance_x")),
																											  static_cast<float>(glyphObj.get<jsonxx::Number>("advance_y")),
																											  static_cast<float>(glyphObj.get<jsonxx::Number>("left")),
																											  static_cast<float>(glyphObj.get<jsonxx::Number>("top")),
																											  static_cast<float>(glyphObj.get<jsonxx::Number>("right")),
																											  static_cast<float>(glyphObj.get<jsonxx::Number>("bottom")),
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
		new (pStateWriteLocation)FontState(stateObj.get<jsonxx::String>("name"), m_pTypefaces, stateObj.get<jsonxx::Array>("layers"));
	}
}
