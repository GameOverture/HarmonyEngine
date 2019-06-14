/**************************************************************************
*	TextModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TextModel.h"
#include "Project.h"
#include "ExplorerModel.h"

#define FONTPROP_Dimensions "Dimensions"
#define FONTPROP_UsedPercent "Used Percent"
#define FONTPROP_09 "0-9"
#define FONTPROP_AZ "A-Z"
#define FONTPROP_az "a-z"
#define FONTPROP_Symbols "!\"#$%&'()*+,-./\\[]^_`{|}~:;<=>?@"
#define FONTPROP_AdditionalSyms "Additional glyphs"

TextStateData::TextStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj) :
	IStateData(iStateIndex, modelRef, stateObj["name"].toString())
{
}

/*virtual*/ TextStateData::~TextStateData()
{
}

/*virtual*/ int TextStateData::AddFrame(AtlasFrame *pFrame) /*override*/
{
	return 0;
}

/*virtual*/ void TextStateData::RelinquishFrame(AtlasFrame *pFrame) /*override*/
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TextModel::TextModel(ProjectItem &itemRef, QJsonObject fontObj) :
	IModel(itemRef)
{

}

/*virtual*/ TextModel::~TextModel()
{
}

/*virtual*/ void TextModel::OnSave() /*override*/
{
	//if(m_FontMetaDir.mkpath(".") == false)
	//	HyGuiLog("Could not create font meta directory", LOGTYPE_Error);
	//else
	//{
	//	// Copy font files into the font meta directory
	//	for(int i = 0; i < m_MasterLayerList.count(); ++i)
	//	{
	//		QFileInfo tmpFontFile(m_MasterLayerList[i]->pTextureFont->filename);
	//		QFileInfo metaFontFile(m_FontMetaDir.absoluteFilePath(tmpFontFile.fileName()));

	//		if(metaFontFile.exists() == false)
	//		{
	//			if(QFile::copy(tmpFontFile.absoluteFilePath(), metaFontFile.absoluteFilePath()) == false)
	//				HyGuiLog("Could not copy font file (" % tmpFontFile.filePath() % ") to the meta directory", LOGTYPE_Error);
	//		}
	//	}
	//}
}

/*virtual*/ QJsonObject TextModel::PopStateAt(uint32 uiIndex) /*override*/
{
	QJsonObject retObj;
	//static_cast<TextStateData *>(m_StateList[uiIndex])->GetStateInfo(retObj);

	return retObj;
}

/*virtual*/ QJsonValue TextModel::GetJson() const /*override*/
{
	QJsonObject textObj;

	//textObj.insert("checksum", m_pTrueAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_pTrueAtlasFrame->GetImageChecksum())));
	//textObj.insert("id", m_pTrueAtlasFrame == nullptr ? 0 : QJsonValue(static_cast<qint64>(m_pTrueAtlasFrame->GetId())));

	//textObj.insert("subAtlasWidth", m_pTrueAtlasFrame == nullptr ? 0 : QJsonValue(m_pTrueAtlasFrame->GetSize().width()));
	//textObj.insert("subAtlasHeight", m_pTrueAtlasFrame == nullptr ? 0 : QJsonValue(m_pTrueAtlasFrame->GetSize().height()));

	//QJsonObject availableGlyphsObj;
	//QVariant propValue;

	//propValue = m_TypeFacePropertiesModel.FindPropertyValue("Uses Glyphs", FONTPROP_09);
	//availableGlyphsObj.insert("0-9", static_cast<bool>(propValue.toInt() == Qt::Checked));

	//propValue = m_TypeFacePropertiesModel.FindPropertyValue("Uses Glyphs", FONTPROP_az);
	//availableGlyphsObj.insert("a-z", static_cast<bool>(propValue.toInt() == Qt::Checked));

	//propValue = m_TypeFacePropertiesModel.FindPropertyValue("Uses Glyphs", FONTPROP_AZ);
	//availableGlyphsObj.insert("A-Z", static_cast<bool>(propValue.toInt() == Qt::Checked));

	//propValue = m_TypeFacePropertiesModel.FindPropertyValue("Uses Glyphs", FONTPROP_Symbols);
	//availableGlyphsObj.insert("symbols", static_cast<bool>(propValue.toInt() == Qt::Checked));

	//propValue = m_TypeFacePropertiesModel.FindPropertyValue("Uses Glyphs", FONTPROP_AdditionalSyms);
	//availableGlyphsObj.insert("additional", propValue.toString());

	//textObj.insert("availableGlyphs", availableGlyphsObj);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//QString sAvailableTypefaceGlyphs = GetAvailableTypefaceGlyphs();
	//QJsonArray typefaceArray;
	//for(int i = 0; i < m_MasterLayerList.count(); ++i)
	//{
	//	QJsonObject stageObj;
	//	QFileInfo fontFileInfo(m_MasterLayerList[i]->pTextureFont->filename);

	//	stageObj.insert("font", fontFileInfo.fileName());
	//	stageObj.insert("size", m_MasterLayerList[i]->fSize);
	//	stageObj.insert("mode", m_MasterLayerList[i]->eMode);
	//	stageObj.insert("outlineThickness", m_MasterLayerList[i]->fOutlineThickness);

	//	QJsonArray glyphsArray;
	//	for(int j = 0; j < sAvailableTypefaceGlyphs.count(); ++j)
	//	{
	//		// NOTE: Assumes LITTLE ENDIAN
	//		QString sSingleChar = sAvailableTypefaceGlyphs[j];
	//		texture_glyph_t *pGlyph = texture_font_get_glyph(m_MasterLayerList[i]->pTextureFont, sSingleChar.toUtf8().data());

	//		QJsonObject glyphInfoObj;
	//		if(pGlyph == nullptr)
	//		{
	//			HyGuiLog("Could not find glyph: '" % sSingleChar % "'\nPlace a breakpoint and walk into texture_font_get_glyph() below before continuing", LOGTYPE_Error);

	//			pGlyph = texture_font_get_glyph(m_MasterLayerList[i]->pTextureFont, sSingleChar.toUtf8().data());
	//		}
	//		else
	//		{
	//			glyphInfoObj.insert("code", QJsonValue(static_cast<qint64>(pGlyph->codepoint)));
	//			glyphInfoObj.insert("advance_x", pGlyph->advance_x);
	//			glyphInfoObj.insert("advance_y", pGlyph->advance_y);
	//			glyphInfoObj.insert("width", static_cast<int>(pGlyph->width));
	//			glyphInfoObj.insert("height", static_cast<int>(pGlyph->height));
	//			glyphInfoObj.insert("offset_x", pGlyph->offset_x);
	//			glyphInfoObj.insert("offset_y", pGlyph->offset_y);
	//			glyphInfoObj.insert("left", pGlyph->s0);
	//			glyphInfoObj.insert("top", pGlyph->t0);
	//			glyphInfoObj.insert("right", pGlyph->s1);
	//			glyphInfoObj.insert("bottom", pGlyph->t1);
	//		}

	//		QJsonObject kerningInfoObj;
	//		for(int k = 0; k < sAvailableTypefaceGlyphs.count(); ++k)
	//		{
	//			char cTmpChar = sAvailableTypefaceGlyphs.toStdString().c_str()[k];
	//			float fKerningAmt = texture_glyph_get_kerning(pGlyph, &cTmpChar);

	//			if(fKerningAmt != 0.0f)
	//				kerningInfoObj.insert(QString(sAvailableTypefaceGlyphs[k]), fKerningAmt);
	//		}
	//		glyphInfoObj.insert("kerning", kerningInfoObj);

	//		glyphsArray.append(glyphInfoObj);
	//	}
	//	stageObj.insert("glyphs", glyphsArray);

	//	typefaceArray.append(QJsonValue(stageObj));
	//}
	//textObj.insert("typefaceArray", typefaceArray);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//QJsonArray stateArray;
	//for(int i = 0; i < m_StateList.size(); ++i)
	//{
	//	QJsonObject stateObj;
	//	static_cast<TextStateData *>(m_StateList[i])->GetStateInfo(stateObj);

	//	stateArray.append(stateObj);
	//}
	//textObj.insert("stateArray", stateArray);

	return textObj;
}

/*virtual*/ QList<AtlasFrame *> TextModel::GetAtlasFrames() const /*override*/
{
	QList<AtlasFrame *> retAtlasFrameList;
	//if(m_pTrueAtlasFrame)
	//	retAtlasFrameList.push_back(m_pTrueAtlasFrame);

	return retAtlasFrameList;
}

/*virtual*/ QStringList TextModel::GetFontUrls() const /*override*/
{
	QStringList fontUrlList;
	//for(int i = 0; i < m_StateList.size(); ++i)
	//{
	//	FontStateData *pState = static_cast<FontStateData *>(m_StateList[i]);
	//	fontUrlList.append(pState->GetFontFilePath());
	//}

	//fontUrlList.removeDuplicates();
	return fontUrlList;
}

/*virtual*/ void TextModel::Refresh() /*override*/
{
}
