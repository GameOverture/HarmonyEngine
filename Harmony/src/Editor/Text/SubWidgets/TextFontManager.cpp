/**************************************************************************
*	TextFontManager.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TextFontManager.h"
#include "Project.h"

#define FONTPROP_Dimensions "Dimensions"
#define FONTPROP_UsedPercent "Used Percent"
#define FONTPROP_09 "0-9"
#define FONTPROP_AZ "A-Z"
#define FONTPROP_az "a-z"
#define FONTPROP_Symbols "!\"#$%&'()*+,-./\\[]^_`{|}~:;<=>?@"
#define FONTPROP_AdditionalSyms "Additional glyphs"
#define TEXTFONTMANAGER_AtlasSize 2048, 2048

TextFontManager::TextFontManager(ProjectItem &itemRef, QJsonObject availGlyphsObj, QJsonArray fontArray) :
	m_GlyphsModel(itemRef, 0, 0, nullptr),
	m_FontArray(fontArray),
	m_pAtlas(nullptr)
{
	bool b09 = true;
	bool bAZ = true;
	bool baz = true;
	bool bSymbols = true;
	QString sAdditional = "";

	// If item's init value is defined, parse and initialize with it, otherwise make default empty font
	if(availGlyphsObj.empty() == false)
	{
		b09 = availGlyphsObj[FONTPROP_09].toBool();
		bAZ = availGlyphsObj[FONTPROP_AZ].toBool();
		baz = availGlyphsObj[FONTPROP_az].toBool();
		bSymbols = availGlyphsObj["symbols"].toBool();
		sAdditional = availGlyphsObj["additional"].toString();
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

TextFontManager::~TextFontManager()
{
	for(int i = 0; i < m_FontList.size(); ++i)
		delete m_FontList[i];

	texture_atlas_delete(m_pAtlas);
}

QJsonObject TextFontManager::GetAvailableGlyphsObject() const
{
	QJsonObject availableGlyphsObj;
	QVariant propValue;

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_09);
	availableGlyphsObj.insert("0-9", static_cast<bool>(propValue.toInt() == Qt::Checked));

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_az);
	availableGlyphsObj.insert("a-z", static_cast<bool>(propValue.toInt() == Qt::Checked));

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_AZ);
	availableGlyphsObj.insert("A-Z", static_cast<bool>(propValue.toInt() == Qt::Checked));

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_Symbols);
	availableGlyphsObj.insert("symbols", static_cast<bool>(propValue.toInt() == Qt::Checked));

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_AdditionalSyms);
	availableGlyphsObj.insert("additional", propValue.toString());

	return availableGlyphsObj;
}

QJsonArray TextFontManager::GetFontArray() const
{
	return m_FontArray;
}

TextFontHandle TextFontManager::AcquireFont(QString sFontName, rendermode_t eRenderMode, float fSize, float fOutlineThickness)
{
	for(int i = 0; i < m_FontList.size(); ++i)
	{
		//m_FontList[i]
	}
}

PropertiesTreeModel *TextFontManager::GetGlyphsModel()
{
	return &m_GlyphsModel;
}

void TextFontManager::PrepPreview()
{
	QStandardItemModel *pFontListModel = m_GlyphsModel.GetOwner().GetProject().GetFontListModel();
	QString sAvailGlyphs = GetAvailableTypefaceGlyphs();
	m_pAtlas = texture_atlas_new(TEXTFONTMANAGER_AtlasSize, 1);
	for(int i = 0; i < m_FontArray.size(); ++i)
	{
		QJsonObject fontObj = m_FontArray[i].toObject();

		QList<QStandardItem *> foundFontList = pFontListModel->findItems(fontObj["font"].toString());
		if(foundFontList.size() == 1)
			m_FontList.append(new TextFont(m_pAtlas,
				sAvailGlyphs,
				foundFontList[0]->data().toString(),
				fontObj["size"].toDouble(),
				fontObj["outlineThickness"].toDouble(),
				static_cast<rendermode_t>(fontObj["mode"].toInt())));
	}
}

void TextFontManager::RegenFontArray()
{
	QJsonArray fontArray;
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

	//	fontArray.append(QJsonValue(stageObj));
	//}

	m_FontArray = fontArray;
}

QString TextFontManager::GetAvailableTypefaceGlyphs() const
{
	// Assemble glyph set
	QString sAvailableTypefaceGlyphs;
	sAvailableTypefaceGlyphs.clear();
	sAvailableTypefaceGlyphs += ' ';

	QVariant propValue;

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_09);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "0123456789";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_az);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "abcdefghijklmnopqrstuvwxyz";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_AZ);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_Symbols);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += FONTPROP_Symbols;

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", FONTPROP_AdditionalSyms);
	sAvailableTypefaceGlyphs += propValue.toString(); // May contain duplicates as stated in freetype-gl documentation

	return sAvailableTypefaceGlyphs;
}
