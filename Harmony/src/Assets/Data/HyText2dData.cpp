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

			curTypeface[static_cast<uint32>(glyphObj.get<jsonxx::Number>("code"))] = HY_NEW GlyphInfo(static_cast<uint32>(glyphObj.get<jsonxx::Number>("width")),
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
	for (int i = 0; i < stateArray.size(); ++i, ++pStateWriteLocation)
	{
		jsonxx::Object stateObj = stateArray.get<jsonxx::Object>(i);
		new (pStateWriteLocation)FontState(pAtlasGroup,
			uiTextureIndex,
			rUVRect.left, rUVRect.top, rUVRect.right, rUVRect.bottom,
			glm::ivec2(static_cast<int32>(frameObj.get<jsonxx::Number>("offsetX")), static_cast<int32>(frameObj.get<jsonxx::Number>("offsetY"))),
			rUVRect.iTag == 0 ? false : true,
			static_cast<float>(frameObj.get<jsonxx::Number>("duration")));
	}

	//m_uiNumStates = static_cast<uint32>(spriteStateArray.size());
	//m_pAnimStates = reinterpret_cast<AnimState *>(HY_NEW unsigned char[sizeof(AnimState) * m_uiNumStates]);
	//AnimState *pAnimStateWriteLocation = m_pAnimStates;

	//for(uint32 i = 0; i < m_uiNumStates; ++i, ++pAnimStateWriteLocation)
	//{
	//	jsonxx::Object spriteStateObj = spriteStateArray.get<jsonxx::Object>(i);

	//	new (pAnimStateWriteLocation)AnimState(spriteStateObj.get<jsonxx::String>("name"),
	//		spriteStateObj.get<jsonxx::Boolean>("loop"),
	//		spriteStateObj.get<jsonxx::Boolean>("reverse"),
	//		spriteStateObj.get<jsonxx::Boolean>("bounce"),
	//		spriteStateObj.get<jsonxx::Array>("frames"),
	//		*this);
	//}
}
