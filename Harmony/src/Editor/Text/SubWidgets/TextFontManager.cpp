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
#include "TextModel.h"
#include "Project.h"

#define TEXTFONTMANAGER_AtlasSize 2048, 2048

TextLayerHandle TextFontManager::sm_hHandleCount = 0;

TextFontManager::TextFontManager(ProjectItem &itemRef, QJsonObject availGlyphsObj, QJsonArray fontArray) :
	m_FontArray(fontArray),
	m_GlyphsModel(itemRef, 0, 0, nullptr),
	m_pPreviewAtlas(nullptr)
{
	bool b09 = true;
	bool bAZ = true;
	bool baz = true;
	bool bSymbols = true;
	QString sAdditional = "";

	// If item's init value is defined, parse and initialize with it, otherwise make default empty font
	if(availGlyphsObj.empty() == false)
	{
		b09 = availGlyphsObj[TEXTPROP_09].toBool();
		bAZ = availGlyphsObj[TEXTPROP_AZ].toBool();
		baz = availGlyphsObj[TEXTPROP_az].toBool();
		bSymbols = availGlyphsObj["symbols"].toBool();
		sAdditional = availGlyphsObj["additional"].toString();
	}

	m_GlyphsModel.AppendCategory("Uses Glyphs", HyGlobal::ItemColor(ITEM_Prefix));
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_09, PROPERTIESTYPE_bool, QVariant(b09 ? Qt::Checked : Qt::Unchecked), "Include numerical glyphs 0-9");
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_AZ, PROPERTIESTYPE_bool, QVariant(bAZ ? Qt::Checked : Qt::Unchecked), "Include capital letter glyphs A-Z");
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_az, PROPERTIESTYPE_bool, QVariant(baz ? Qt::Checked : Qt::Unchecked), "Include lowercase letter glyphs a-z");
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_Symbols, PROPERTIESTYPE_bool, QVariant(bSymbols ? Qt::Checked : Qt::Unchecked), "Include common punctuation and symbol glyphs");
	m_GlyphsModel.AppendProperty("Uses Glyphs", TEXTPROP_AdditionalSyms, PROPERTIESTYPE_LineEdit, QVariant(sAdditional), "Include specified glyphs");
	m_GlyphsModel.AppendCategory("Atlas Info");
	m_GlyphsModel.AppendProperty("Atlas Info", TEXTPROP_Dimensions, PROPERTIESTYPE_ivec2, QPoint(0, 0), "The required portion size needed to fit on an atlas", true);
	m_GlyphsModel.AppendProperty("Atlas Info", TEXTPROP_UsedPercent, PROPERTIESTYPE_double, 0.0, "Percentage of the maximum size dimensions used", true);
}

TextFontManager::~TextFontManager()
{
	for(auto iter = m_LayerMap.begin(); iter != m_LayerMap.end(); ++iter)
		delete iter.value();

	for(int i = 0; i < m_PreviewFontList.size(); ++i)
		delete m_PreviewFontList[i];

	if(m_pPreviewAtlas != nullptr)
		texture_atlas_delete(m_pPreviewAtlas);
}

PropertiesTreeModel *TextFontManager::GetGlyphsModel()
{
	return &m_GlyphsModel;
}

const PropertiesTreeModel *TextFontManager::GetGlyphsModel() const
{
	return &m_GlyphsModel;
}

QList<TextLayerHandle> TextFontManager::RegisterLayers(QJsonArray layerArray)
{
	QList<TextLayerHandle> retLayerHandleList;
	for(int i = 0; i < layerArray.size(); ++i)
	{
		QJsonObject layerObj = layerArray[i].toObject();

		QColor botColor;
		botColor.setRedF(layerObj["botR"].toDouble());
		botColor.setGreenF(layerObj["botG"].toDouble());
		botColor.setBlueF(layerObj["botB"].toDouble());
		QColor topColor;
		topColor.setRedF(layerObj["topR"].toDouble());
		topColor.setGreenF(layerObj["topG"].toDouble());
		topColor.setBlueF(layerObj["topB"].toDouble());

		uint32 uiFontIndex = layerObj["typefaceIndex"].toInt();

		TextLayerHandle hNewLayer = ++sm_hHandleCount;
		m_LayerMap[hNewLayer] = new Layer(hNewLayer, uiFontIndex, botColor, topColor);

		retLayerHandleList.append(hNewLayer);
	}

	return retLayerHandleList;
}

QJsonArray TextFontManager::GetFontArray() const
{
	return m_FontArray;
}

uint TextFontManager::GetFontIndex(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetFontIndex passed an invalid handle", LOGTYPE_Error);
		return 0;
	}

	return iter.value()->m_uiFontIndex;
}

QString TextFontManager::GetFontName(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetFontName passed an invalid handle", LOGTYPE_Error);
		return QString();
	}

	QJsonObject fontObj = m_FontArray.at(iter.value()->m_uiFontIndex).toObject();
	return fontObj["font"].toString();
}

rendermode_t TextFontManager::GetRenderMode(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetRenderMode passed an invalid handle", LOGTYPE_Error);
		return RENDER_NORMAL;
	}

	QJsonObject fontObj = m_FontArray.at(iter.value()->m_uiFontIndex).toObject();
	return static_cast<rendermode_t>(fontObj["mode"].toInt());
}

float TextFontManager::GetOutlineThickness(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetOutlineThickness passed an invalid handle", LOGTYPE_Error);
		return 0.0f;
	}

	QJsonObject fontObj = m_FontArray.at(iter.value()->m_uiFontIndex).toObject();
	return static_cast<float>(fontObj["outlineThickness"].toDouble());
}

float TextFontManager::GetSize(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetSize passed an invalid handle", LOGTYPE_Error);
		return 0.0f;
	}

	QJsonObject fontObj = m_FontArray.at(iter.value()->m_uiFontIndex).toObject();
	return static_cast<float>(fontObj["size"].toDouble());
}

void TextFontManager::GetColor(TextLayerHandle hLayer, QColor &topColorOut, QColor &botColorOut) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetColor passed an invalid handle", LOGTYPE_Error);
		return;
	}

	topColorOut = iter.value()->m_TopColor;
	botColorOut = iter.value()->m_BotColor;
}

TextLayerHandle TextFontManager::AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize)
{
	TextLayerHandle hNewLayer = HY_UNUSED_HANDLE;

	// If font already exists, don't generate preview
	int iFontIndex = DoesFontExist(sFontName, eRenderMode, fOutlineThickness, fSize);
	if(iFontIndex >= 0)
	{
		hNewLayer = ++sm_hHandleCount;
		m_LayerMap[hNewLayer] = new Layer(hNewLayer, iFontIndex, QColor(Qt::black), QColor(Qt::black));

		return hNewLayer;
	}

	PrepPreview();
	{
		int iNewFontIndex = CreatePreviewFont(sFontName, eRenderMode, fOutlineThickness, fSize, true);
		if(iNewFontIndex >= 0)
		{
			hNewLayer = ++sm_hHandleCount;
			m_LayerMap[hNewLayer] = new Layer(hNewLayer, iNewFontIndex, QColor(Qt::black), QColor(Qt::black));
		}
	}
	RegenFontArray();

	return hNewLayer;
}

void TextFontManager::SetRenderMode(TextLayerHandle hLayer, rendermode_t eMode)
{
	// If font already exists, don't generate preview
	int iFontIndex = DoesFontExist(GetFontName(hLayer), eMode, GetOutlineThickness(hLayer), GetSize(hLayer));
	if(iFontIndex >= 0)
	{
		m_LayerMap[hLayer]->m_uiFontIndex = iFontIndex;
		return;
	}

	PrepPreview();
	{
		int iNewFontIndex = CreatePreviewFont(GetFontName(hLayer), eMode, GetOutlineThickness(hLayer), GetSize(hLayer), true);
		if(iNewFontIndex >= 0)
			m_LayerMap[hLayer]->m_uiFontIndex = iNewFontIndex;
	}
	RegenFontArray();
}

void TextFontManager::SetOutlineThickness(TextLayerHandle hLayer, float fThickness)
{
	// If font already exists, don't generate preview
	int iFontIndex = DoesFontExist(GetFontName(hLayer), GetRenderMode(hLayer), fThickness, GetSize(hLayer));
	if(iFontIndex >= 0)
	{
		m_LayerMap[hLayer]->m_uiFontIndex = iFontIndex;
		return;
	}

	PrepPreview();
	{
		int iNewFontIndex = CreatePreviewFont(GetFontName(hLayer), GetRenderMode(hLayer), fThickness, GetSize(hLayer), true);
		if(iNewFontIndex >= 0)
			m_LayerMap[hLayer]->m_uiFontIndex = iNewFontIndex;
	}
	RegenFontArray();
}

void TextFontManager::SetColors(TextLayerHandle hLayer, const QColor &topColor, const QColor &botColor)
{
	m_LayerMap[hLayer]->m_TopColor = topColor;
	m_LayerMap[hLayer]->m_BotColor = botColor;
}

int TextFontManager::DoesFontExist(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize) const
{
	for(int i = 0; i < m_FontArray.size(); ++i)
	{
		QJsonObject fontObj = m_FontArray[i].toObject();

		if(fontObj["font"].toString().compare(sFontName, Qt::CaseInsensitive) == 0 &&
		   fontObj["mode"].toInt() == eRenderMode &&
		   HyCompareFloat(fontObj["outlineThickness"].toDouble(), static_cast<double>(fOutlineThickness)) &&
		   HyCompareFloat(fontObj["size"].toDouble(), static_cast<double>(fSize)))
		{
			return i;
		}
	}

	return -1;
}

void TextFontManager::PrepPreview()
{
	if(m_pPreviewAtlas == nullptr)
	{
		m_pPreviewAtlas = texture_atlas_new(TEXTFONTMANAGER_AtlasSize, 1);
		if(m_PreviewFontList.isEmpty() == false)
		{
			HyGuiLog("PrepPreview has uncleared fonts in 'm_PreviewFontList'", LOGTYPE_Error);
			m_PreviewFontList.clear();
		}
	}
	
	if(m_pPreviewAtlas->used == 0.0f)
	{
		for(int i = 0; i < m_FontArray.size(); ++i)
		{
			QJsonObject fontObj = m_FontArray[i].toObject();
			CreatePreviewFont(fontObj["font"].toString(), static_cast<rendermode_t>(fontObj["mode"].toInt()), fontObj["outlineThickness"].toDouble(), fontObj["size"].toDouble(), false);
		}
	}
}

int TextFontManager::CreatePreviewFont(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize, bool bAllowRepack)
{
	QStandardItemModel *pProjectFontsModel = m_GlyphsModel.GetOwner().GetProject().GetFontListModel();
	int iNumFonts = pProjectFontsModel->rowCount();
	for(int i = 0; i < iNumFonts; ++i)
	{
		QString sTest = pProjectFontsModel->item(i)->text();
		if(pProjectFontsModel->item(i)->text().compare(sFontName, Qt::CaseInsensitive) == 0)
		{
			PreviewFont *pNewPreviewFont = new PreviewFont(m_pPreviewAtlas,
														   GetAvailableTypefaceGlyphs(),
														   pProjectFontsModel->item(i)->data().toString(),
														   fSize,
														   fOutlineThickness,
														   eRenderMode);
			if(pNewPreviewFont->GetMissedGlyphs() != 0)
			{
				if(bAllowRepack)
				{
					HyGuiLog("Font preview could not fit '" % QString::number(pNewPreviewFont->GetMissedGlyphs()) % "' glyphs on atlas. Removing unused fonts from preview atlas and regenerating.", LOGTYPE_Info);
					delete pNewPreviewFont;

					for(int i = 0; i < m_PreviewFontList.size(); ++i)
						delete m_PreviewFontList[i];
					m_PreviewFontList.clear();

					texture_atlas_clear(m_pPreviewAtlas);
					PrepPreview();

					// Attempt two
					pNewPreviewFont = new PreviewFont(m_pPreviewAtlas,
													  GetAvailableTypefaceGlyphs(),
													  pProjectFontsModel->item(i)->data().toString(),
													  fSize,
													  fOutlineThickness,
													  eRenderMode);

					if(pNewPreviewFont->GetMissedGlyphs() != 0)
					{
						HyGuiLog("TextFontManager::CreatePreviewFont could not fit all required font layers on atlas, even after repacking", LOGTYPE_Error);
						return -1;
					}
				}
				else
				{
					HyGuiLog("TextFontManager::CreatePreviewFont could not fit all required font layers on atlas", LOGTYPE_Error);
					return -1;
				}
			}

			m_PreviewFontList.append(pNewPreviewFont);
			return m_PreviewFontList.size() - 1;
		}
	}

	HyGuiLog("TextFontManager::CreatePreviewFont could not find font '" % sFontName % "'", LOGTYPE_Error);
	return -1;
}

void TextFontManager::RegenFontArray()
{
	QString sAvailGlyphs = GetAvailableTypefaceGlyphs();

	CleanUnusedFonts();

	QJsonArray fontArray;
	for(int i = 0; i < m_PreviewFontList.count(); ++i)
	{
		texture_font_t *pFtglFont = m_PreviewFontList[i]->GetTextureFont();

		QJsonObject stageObj;
		QFileInfo fontFileInfo(pFtglFont->filename);

		stageObj.insert("font", fontFileInfo.fileName());
		stageObj.insert("size", pFtglFont->size);
		stageObj.insert("mode", pFtglFont->rendermode);
		stageObj.insert("outlineThickness", pFtglFont->outline_thickness);

		QJsonArray glyphsArray;
		for(int j = 0; j < sAvailGlyphs.count(); ++j)
		{
			// NOTE: Assumes LITTLE ENDIAN
			QString sSingleChar = sAvailGlyphs[j];
			texture_glyph_t *pGlyph = texture_font_get_glyph(pFtglFont, sSingleChar.toUtf8().data());

			QJsonObject glyphInfoObj;
			if(pGlyph == nullptr)
			{
				HyGuiLog("Could not find glyph: '" % sSingleChar % "'\nPlace a breakpoint and walk into texture_font_get_glyph() below before continuing", LOGTYPE_Error);
				pGlyph = texture_font_get_glyph(pFtglFont, sSingleChar.toUtf8().data());
			}
			else
			{
				glyphInfoObj.insert("code", QJsonValue(static_cast<qint64>(pGlyph->codepoint)));
				glyphInfoObj.insert("advance_x", pGlyph->advance_x);
				glyphInfoObj.insert("advance_y", pGlyph->advance_y);
				glyphInfoObj.insert("width", static_cast<int>(pGlyph->width));
				glyphInfoObj.insert("height", static_cast<int>(pGlyph->height));
				glyphInfoObj.insert("offset_x", pGlyph->offset_x);
				glyphInfoObj.insert("offset_y", pGlyph->offset_y);
				glyphInfoObj.insert("left", pGlyph->s0);
				glyphInfoObj.insert("top", pGlyph->t0);
				glyphInfoObj.insert("right", pGlyph->s1);
				glyphInfoObj.insert("bottom", pGlyph->t1);
			}

#if 0		// Not using kerning data
			QJsonObject kerningInfoObj;
			for(int k = 0; k < sAvailableTypefaceGlyphs.count(); ++k)
			{
				char cTmpChar = sAvailableTypefaceGlyphs.toStdString().c_str()[k];
				float fKerningAmt = texture_glyph_get_kerning(pGlyph, &cTmpChar);

				if(fKerningAmt != 0.0f)
					kerningInfoObj.insert(QString(sAvailableTypefaceGlyphs[k]), fKerningAmt);
			}
			glyphInfoObj.insert("kerning", kerningInfoObj);
#endif
			glyphsArray.append(glyphInfoObj);
		}
		stageObj.insert("glyphs", glyphsArray);

		fontArray.append(QJsonValue(stageObj));
	}

	m_FontArray = fontArray;
}

void TextFontManager::CleanUnusedFonts()
{
	int iFontIndex = 0;
	for(auto iter = m_PreviewFontList.begin(); iter != m_PreviewFontList.end(); ++iter, ++iFontIndex)
	{
		bool bFontUsed = false;
		for(auto iterLayer = m_LayerMap.begin(); iterLayer != m_LayerMap.end(); ++iterLayer)
		{
			if(iterLayer.value()->m_uiFontIndex == iFontIndex)
			{
				bFontUsed = true;
				break;
			}
		}

		if(bFontUsed == false)
		{
			iter = m_PreviewFontList.erase(iter);

			// If a font gets erased, layer font indexes may become offset. Fix below
			for(auto iterLayer = m_LayerMap.begin(); iterLayer != m_LayerMap.end(); ++iterLayer)
			{
				if(iterLayer.value()->m_uiFontIndex > iFontIndex)
					iterLayer.value()->m_uiFontIndex--;
			}

			iFontIndex--; // Subtract one here to account for the erased font
		}
	}
}

QString TextFontManager::GetAvailableTypefaceGlyphs() const
{
	// Assemble glyph set
	QString sAvailableTypefaceGlyphs;
	sAvailableTypefaceGlyphs.clear();
	sAvailableTypefaceGlyphs += ' ';

	QVariant propValue;

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_09);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "0123456789";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_az);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "abcdefghijklmnopqrstuvwxyz";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_AZ);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_Symbols);
	if(propValue.toInt() == Qt::Checked)
		sAvailableTypefaceGlyphs += TEXTPROP_Symbols;

	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_AdditionalSyms);
	sAvailableTypefaceGlyphs += propValue.toString(); // May contain duplicates as stated in freetype-gl documentation

	return sAvailableTypefaceGlyphs;
}
