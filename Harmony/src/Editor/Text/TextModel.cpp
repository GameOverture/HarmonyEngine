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
	IStateData(iStateIndex, modelRef, stateObj["name"].toString()),
	m_LayersModel(&m_ModelRef)
{
}

/*virtual*/ TextStateData::~TextStateData()
{
}

TextLayersModel &TextStateData::GetLayersModel()
{
	return m_LayersModel;
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
	IModel(itemRef),
	m_GlyphsModel(itemRef, 0, 0, this),
	m_pAtlasFrame(nullptr)
{
	bool b09 = true;
	bool bAZ = true;
	bool baz = true;
	bool bSymbols = true;
	QString sAdditional = "";

	// If item's init value is defined, parse and initialize with it, otherwise make default empty font
	if(fontObj.empty() == false)
	{
		QJsonObject availGlyphsObj = fontObj["availableGlyphs"].toObject();
		b09 = availGlyphsObj[FONTPROP_09].toBool();
		bAZ = availGlyphsObj[FONTPROP_AZ].toBool();
		baz = availGlyphsObj[FONTPROP_az].toBool();
		bSymbols = availGlyphsObj["symbols"].toBool();
		sAdditional = availGlyphsObj["additional"].toString();

		int iAffectedFrameIndex = 0;
		QList<quint32> idRequestList;
		idRequestList.append(JSONOBJ_TOINT(fontObj, "id"));
		QList<AtlasFrame *> pRequestedList = RequestFramesById(nullptr, idRequestList, iAffectedFrameIndex);
		if(pRequestedList.size() == 1)
			m_pAtlasFrame = pRequestedList[0];
		else
			HyGuiLog("More than one frame returned for a font", LOGTYPE_Error);

		// Must set 'm_TypefaceArray' before any AppendState() call
		m_TypefaceArray = fontObj["typefaceArray"].toArray();

		QJsonArray stateArray = fontObj["stateArray"].toArray();
		for(int i = 0; i < stateArray.size(); ++i)
		{
			QJsonObject stateObj = stateArray[i].toObject();
			
			AppendState<TextStateData>(stateObj);
			if(stateObj.empty() == false)
			{
				QJsonArray layerArray = stateObj["layers"].toArray();
				for(int j = 0; j < layerArray.size(); ++j)
				{
					QJsonObject layerObj = layerArray.at(j).toObject();
					QJsonObject typefaceObj = m_TypefaceArray.at(layerObj["typefaceIndex"].toInt()).toObject();

					//if(j == 0) // Only need to set the state's font and size once
					//{
					//	m_pCmbMapper_Fonts->SetIndex(typefaceObj["font"].toString());
					//	m_pSbMapper_Size->SetValue(typefaceObj["size"].toDouble());
					//}

					QColor topColor, botColor;
					topColor.setRgbF(layerObj["topR"].toDouble(), layerObj["topG"].toDouble(), layerObj["topB"].toDouble());
					botColor.setRgbF(layerObj["botR"].toDouble(), layerObj["botG"].toDouble(), layerObj["botB"].toDouble());

					TextLayersModel &layersModelRef = static_cast<TextStateData *>(m_StateList[m_StateList.size() - 1])->GetLayersModel();
					int iLayerId = layersModelRef.AddNewLayer(static_cast<rendermode_t>(typefaceObj["mode"].toInt()), typefaceObj["size"].toInt(), typefaceObj["outlineThickness"].toDouble());
					layersModelRef.SetLayerColors(iLayerId, topColor, botColor);
				}
			}
		}
	}
	else
	{
		AppendState<TextStateData>(QJsonObject());
	}

	m_GlyphsModel.AppendCategory("Uses Glyphs", HyGlobal::ItemColor(ITEM_Prefix));
	m_GlyphsModel.AppendProperty("Uses Glyphs", FONTPROP_09, PROPERTIESTYPE_bool, QVariant(b09 ? Qt::Checked : Qt::Unchecked), "Include numerical glyphs 0-9");
	m_GlyphsModel.AppendProperty("Uses Glyphs", FONTPROP_AZ, PROPERTIESTYPE_bool, QVariant(bAZ ? Qt::Checked : Qt::Unchecked), "Include capital letter glyphs A-Z");
	m_GlyphsModel.AppendProperty("Uses Glyphs", FONTPROP_az, PROPERTIESTYPE_bool, QVariant(baz ? Qt::Checked : Qt::Unchecked), "Include lowercase letter glyphs a-z");
	m_GlyphsModel.AppendProperty("Uses Glyphs", FONTPROP_Symbols, PROPERTIESTYPE_bool, QVariant(bSymbols ? Qt::Checked : Qt::Unchecked), "Include common punctuation and symbol glyphs");
	m_GlyphsModel.AppendProperty("Uses Glyphs", FONTPROP_AdditionalSyms, PROPERTIESTYPE_LineEdit, QVariant(sAdditional), "Include specified glyphs");
	m_GlyphsModel.AppendCategory("Atlas Info");
	m_GlyphsModel.AppendProperty("Atlas Info", FONTPROP_Dimensions, PROPERTIESTYPE_ivec2, QPoint(0, 0), "The required portion size needed to fit on an atlas", true);
	m_GlyphsModel.AppendProperty("Atlas Info", FONTPROP_UsedPercent, PROPERTIESTYPE_double, 0.0, "Percentage of the maximum size dimensions used", true);
}

/*virtual*/ TextModel::~TextModel()
{
}

TextLayersModel *TextModel::GetLayersModel(uint uiIndex)
{
	if(uiIndex < m_StateList.size())
		return &static_cast<TextStateData *>(m_StateList[uiIndex])->GetLayersModel();

	return nullptr;
}

PropertiesTreeModel *TextModel::GetGlyphsModel()
{
	return &m_GlyphsModel;
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
