/**************************************************************************
 *	HyTextData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/Objects/HyTextData.h"
#include "Assets/Files/HyFileAtlas.h"
#include "Renderer/IHyRenderer.h"
#include "Diagnostics/Console/IHyConsole.h"

HyTextData::FontState::FontState(Typeface *pTypefaces, float fLineHeight, float fLineAcender, float fLineDescender, float fLeftSideNudgeAmt, HyJsonArray layersArray) :
	m_fLINE_HEIGHT(fLineHeight),
	m_fLINE_ASCENDER(fLineAcender),
	m_fLINE_DESCENDER(fLineDescender),
	m_fLEFT_SIDE_NUDGE_AMT(fLeftSideNudgeAmt),
	m_uiNUM_LAYERS(layersArray.Size()),
	m_pLayers(nullptr)
{
	m_pLayers = reinterpret_cast<Layer *>(HY_NEW unsigned char[sizeof(Layer) * m_uiNUM_LAYERS]);
	Layer *pLayerWriteLocation = m_pLayers;

	for(uint32 i = 0; i < m_uiNUM_LAYERS; ++i, ++pLayerWriteLocation)
	{
		HyJsonObj layerObj = layersArray[i].GetObject();
		new (pLayerWriteLocation)Layer(layerObj["topR"].GetFloat(),
									   layerObj["topG"].GetFloat(),
									   layerObj["topB"].GetFloat(),
									   layerObj["botR"].GetFloat(),
									   layerObj["botG"].GetFloat(),
									   layerObj["botB"].GetFloat(),
									   pTypefaces[layerObj["typefaceIndex"].GetUint()]);
	}
}

HyTextData::FontState::~FontState()
{
	unsigned char *pLayerBuffer = reinterpret_cast<unsigned char *>(m_pLayers);
	delete[] pLayerBuffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HyTextData::HyTextData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(nodePath),
	m_pTypefaces(nullptr),
	m_uiNumTypefaces(0),
	m_pFontStates(nullptr)
{
	HyUvCoord rSubAtlasUVRect;
	uint64 uiCropMask = 0;

#ifdef HY_PLATFORM_GUI
	if(nodePath.GetPath() == HY_GUI_DATAOVERRIDE)
		m_pAtlas = nullptr;
	else
		m_pAtlas = assetsRef.GetAtlas(itemDataObj["checksum"].GetUint(), itemDataObj["bankId"].GetUint(), rSubAtlasUVRect, uiCropMask);
#else
	m_pAtlas = assetsRef.GetAtlas(itemDataObj["checksum"].GetUint(), itemDataObj["bankId"].GetUint(), rSubAtlasUVRect, uiCropMask);
#endif
	
	// NOTE: m_pAtlas may be nullptr if this is a preview generated by the Editor Tool
	//
	uint32 uiFullAtlasWidth = 0;
	uint32 uiFullAtlasHeight = 0;
	if(m_pAtlas)
	{
		m_RequiredFiles[HYFILE_Atlas].Set(m_pAtlas->GetManifestIndex());

		uiFullAtlasWidth = m_pAtlas->GetWidth();
		uiFullAtlasHeight = m_pAtlas->GetHeight();
	}
	else
	{
		rSubAtlasUVRect.left = rSubAtlasUVRect.top = 0.0f;
		rSubAtlasUVRect.right = rSubAtlasUVRect.bottom = 1.0f;

		if(itemDataObj.HasMember("subAtlasWidth") && itemDataObj.HasMember("subAtlasHeight"))
		{
			uiFullAtlasWidth = static_cast<uint32>(HyMath::Max(0, itemDataObj["subAtlasWidth"].GetInt()));
			uiFullAtlasHeight = static_cast<uint32>(HyMath::Max(0, itemDataObj["subAtlasHeight"].GetInt()));
		}
	}
	
	float fSubAtlasWidth = 0.0f;
	float fSubAtlasHeight = 0.0f;
	if(itemDataObj.HasMember("subAtlasWidth") && itemDataObj.HasMember("subAtlasHeight"))
	{
		fSubAtlasWidth = static_cast<float>(itemDataObj["subAtlasWidth"].GetInt());
		fSubAtlasHeight = static_cast<float>(itemDataObj["subAtlasHeight"].GetInt());
	}
	if(fSubAtlasWidth == 0.0f || fSubAtlasHeight == 0.0f)
	{
		// NOTE: I don't think this should ever happen, but it's a fall-back that should work (if the sub-atlas is not cropped)
		fSubAtlasWidth = uiFullAtlasWidth * (rSubAtlasUVRect.right - rSubAtlasUVRect.left);
		fSubAtlasHeight = uiFullAtlasHeight * (rSubAtlasUVRect.bottom - rSubAtlasUVRect.top);
	}
	
	HyJsonArray fontArray = itemDataObj["fontArray"].GetArray();
	m_uiNumTypefaces = fontArray.Size();
	
	m_pTypefaces = HY_NEW Typeface[m_uiNumTypefaces];
	for (uint32 i = 0; i < m_uiNumTypefaces; ++i)
	{
		Typeface &curTypeface = m_pTypefaces[i];

		HyJsonObj typefaceObj = fontArray[i].GetObject();
		HyJsonArray glyphsArray = typefaceObj["glyphs"].GetArray();
		
		uint32 uiNumGlyphs = glyphsArray.Size();
		for (uint32 j = 0; j < uiNumGlyphs; ++j)
		{
			HyJsonObj glyphObj = glyphsArray[j].GetObject();

			float fLeftUv = (uiFullAtlasWidth * rSubAtlasUVRect.left) + (fSubAtlasWidth * glyphObj["left"].GetFloat());
			fLeftUv /= uiFullAtlasWidth;
			float fTopUv = (uiFullAtlasHeight * rSubAtlasUVRect.top) + (fSubAtlasHeight * glyphObj["top"].GetFloat());
			fTopUv /= uiFullAtlasHeight;
			float fRightUv = (uiFullAtlasWidth * rSubAtlasUVRect.left) + (fSubAtlasWidth * glyphObj["right"].GetFloat());
			fRightUv /= uiFullAtlasWidth;
			float fBottomUv = (uiFullAtlasHeight * rSubAtlasUVRect.top) + (fSubAtlasHeight * glyphObj["bottom"].GetFloat());
			fBottomUv /= uiFullAtlasHeight;

			uint32 uiCode = glyphObj["code"].GetUint();
			//HyAssert(curTypeface.find(uiCode) == curTypeface.end(), "Duplicate glyph codes found - fix in HyEditor");
			curTypeface[uiCode] = HY_NEW HyTextGlyph(
				glyphObj["width"].GetUint(),
				glyphObj["height"].GetUint(),
				glyphObj["offset_x"].GetInt(),
				glyphObj["offset_y"].GetInt(),
				glyphObj["advance_x"].GetFloat(),
				glyphObj["advance_y"].GetFloat(),
				fLeftUv,
				fTopUv,
				fRightUv,
				fBottomUv);
		}
	}

	HyJsonArray stateArray = itemDataObj["stateArray"].GetArray();
	m_uiNumStates = stateArray.Size();

	m_pFontStates = reinterpret_cast<FontState *>(HY_NEW unsigned char[sizeof(FontState) * m_uiNumStates]);
	FontState *pStateWriteLocation = m_pFontStates;
	for (uint32 i = 0; i < stateArray.Size(); ++i, ++pStateWriteLocation)
	{
		HyJsonObj stateObj = stateArray[i].GetObject();
		new (pStateWriteLocation)FontState(m_pTypefaces,
										   stateObj["lineHeight"].GetFloat(),
										   stateObj["lineAscender"].GetFloat(),
										   stateObj["lineDescender"].GetFloat(),
										   stateObj["leftSideNudgeAmt"].GetFloat(),
										   stateObj["layers"].GetArray());
	}
}

HyTextData::~HyTextData(void)
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

uint32 HyTextData::GetNumLayers(uint32 uiStateIndex) const
{
	return m_pFontStates[uiStateIndex].m_uiNUM_LAYERS;
}

const HyTextGlyph *HyTextData::GetGlyph(uint32 uiStateIndex, uint32 uiLayerIndex, uint32 uiUtf32Code) const
{
	if(uiStateIndex >= m_uiNumStates || m_pFontStates[uiStateIndex].m_pLayers == nullptr || uiLayerIndex >= m_pFontStates[uiStateIndex].m_uiNUM_LAYERS)
		return nullptr;

	// Special case: No-Break Space CodePoint: 160
	if(uiUtf32Code == 160)
		uiUtf32Code = 32; // Use standard space (no breaks should occur because it should act as a regular character from this point)

	auto iter = m_pFontStates[uiStateIndex].m_pLayers[uiLayerIndex].m_TYPEFACE_REF.find(uiUtf32Code);
	if(iter == m_pFontStates[uiStateIndex].m_pLayers[uiLayerIndex].m_TYPEFACE_REF.end())
	{
		HyLogDebug("Missing glyph code " << uiUtf32Code << " in Text instance: " << GetPath());

		// Instead return Unicode Character 'REPLACEMENT CHARACTER' (U+FFFD) which should always be available
		iter = m_pFontStates[uiStateIndex].m_pLayers[uiLayerIndex].m_TYPEFACE_REF.find(65533);
		if(iter == m_pFontStates[uiStateIndex].m_pLayers[uiLayerIndex].m_TYPEFACE_REF.end())
		{
//#ifndef HY_PLATFORM_GUI
//			HyError("Could not retrieve Unicode Character 'REPLACEMENT CHARACTER' (U+FFFD) which should always be available");
//#endif
			return nullptr;
		}
	}

	return iter->second;
}

HyColor HyTextData::GetDefaultColor(uint32 uiStateIndex, uint32 uiLayerIndex, bool bTop) const
{
	if(bTop)
		return m_pFontStates[uiStateIndex].m_pLayers[uiLayerIndex].m_DEFAULT_TOP_COLOR;
	else
		return m_pFontStates[uiStateIndex].m_pLayers[uiLayerIndex].m_DEFAULT_BOT_COLOR;
}

HyFileAtlas *HyTextData::GetAtlas() const
{
	return m_pAtlas;
}

float HyTextData::GetLineHeight(uint32 uiStateIndex) const
{
	return m_pFontStates[uiStateIndex].m_fLINE_HEIGHT;
}

float HyTextData::GetLineAscender(uint32 uiStateIndex) const
{
	return m_pFontStates[uiStateIndex].m_fLINE_ASCENDER;
}

float HyTextData::GetLineDescender(uint32 uiStateIndex) const
{
	return m_pFontStates[uiStateIndex].m_fLINE_DESCENDER;
}

float HyTextData::GetLeftSideNudgeAmt(uint32 uiStateIndex) const
{
	if(m_pFontStates)
		return m_pFontStates[uiStateIndex].m_fLEFT_SIDE_NUDGE_AMT;

	return 0.0f;
}
