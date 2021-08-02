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
#include "AtlasModel.h"

#define TEXTFONTERROR_Success 0
#define TEXTFONTERROR_Unknown -1
#define TEXTFONTERROR_AtlasFull -2
#define TEXTFONTERROR_FontNotFound -3

#define FONTMANAGER_PreviewGrowSize 1024
#define FONTMANAGER_OptimizedGrowSize 25

TextLayerHandle TextFontManager::sm_hHandleCount = 0;

TextFontManager::TextFontManager(ProjectItemData &itemRef, QJsonObject availGlyphsObj, QJsonArray fontArray) :
	m_FontArray(fontArray),
	m_GlyphsModel(itemRef, 0, TEXTGLYPHS_SubStateId, nullptr),
	m_pPreviewAtlas(nullptr),
	m_pPreviewAtlasPixelData(nullptr),
	m_uiPreviewAtlasBufferSize(0),
	m_uiPreviewAtlasGrowSize(FONTMANAGER_PreviewGrowSize),
	m_uiPreviewAtlasDimension(m_uiPreviewAtlasGrowSize),
	m_bPreviewAtlasPixelDataInitialized(false)
{
	// Default empty font settings
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
	m_GlyphsModel.AppendProperty("Atlas Info", TEXTPROP_AtlasGroup, PROPERTIESTYPE_LineEdit, "", "The atlas group the font sub atlas will reside in", true);
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

QList<TextLayerHandle> TextFontManager::RegisterLayers(QJsonObject stateObj)
{
	QList<TextLayerHandle> retLayerHandleList;

	QJsonArray layerArray = stateObj["layers"].toArray();
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

		m_LayerMap[hNewLayer] = new Layer(hNewLayer,
										  uiFontIndex,
										  botColor,
										  topColor,
										  stateObj["leftSideNudgeAmt"].toDouble(),
										  stateObj["lineAscender"].toDouble(),
										  stateObj["lineDescender"].toDouble(),
										  stateObj["lineHeight"].toDouble());

		retLayerHandleList.append(hNewLayer);
	}

	return retLayerHandleList;
}

QJsonArray TextFontManager::GetFontArray() const
{
	return m_FontArray;
}

int TextFontManager::GetFontIndex(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetFontIndex passed an invalid handle", LOGTYPE_Error);
		return 0;
	}

	int iIndex = iter.value()->m_iFontIndex;
	return iIndex;
}

QString TextFontManager::GetFontName(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetFontName passed an invalid handle", LOGTYPE_Error);
		return QString();
	}

	QJsonObject fontObj = m_FontArray.at(iter.value()->m_iFontIndex).toObject();
	QString sFontName = fontObj["font"].toString();
	return sFontName;
}

QString TextFontManager::GetFontPath(TextLayerHandle hLayer) const
{
	int iFontIndex = GetFontIndex(hLayer);

	if(iFontIndex >= m_PreviewFontList.size())
	{
		HyGuiLog("TextFontManager::GetFontPath failed because 'm_PreviewFontList' has not been generated", LOGTYPE_Error);
		return QString();
	}

	return QString(m_PreviewFontList[iFontIndex]->GetTextureFont()->filename);
}

rendermode_t TextFontManager::GetRenderMode(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetRenderMode passed an invalid handle", LOGTYPE_Error);
		return RENDER_NORMAL;
	}

	QJsonObject fontObj = m_FontArray.at(iter.value()->m_iFontIndex).toObject();
	rendermode_t eMode = static_cast<rendermode_t>(fontObj["mode"].toInt());
	return eMode;
}

float TextFontManager::GetOutlineThickness(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetOutlineThickness passed an invalid handle", LOGTYPE_Error);
		return 0.0f;
	}

	QJsonObject fontObj = m_FontArray.at(iter.value()->m_iFontIndex).toObject();
	float fThickness = static_cast<float>(fontObj["outlineThickness"].toDouble());
	return fThickness;
}

float TextFontManager::GetSize(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetSize passed an invalid handle", LOGTYPE_Error);
		return 0.0f;
	}

	QJsonObject fontObj = m_FontArray.at(iter.value()->m_iFontIndex).toObject();
	float fSize = static_cast<float>(fontObj["size"].toDouble());
	return fSize;
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

float TextFontManager::GetLineHeight(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetLineHeight passed an invalid handle", LOGTYPE_Error);
		return 0.0f;
	}

	return iter.value()->m_fLineHeight;
}

float TextFontManager::GetLineAscender(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetLineAscender passed an invalid handle", LOGTYPE_Error);
		return 0.0f;
	}

	return iter.value()->m_fLineAscender;
}

float TextFontManager::GetLineDescender(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetLineDescender passed an invalid handle", LOGTYPE_Error);
		return 0.0f;
	}

	return iter.value()->m_fLineDescender;
}

float TextFontManager::GetLeftSideNudgeAmt(TextLayerHandle hLayer) const
{
	auto iter = m_LayerMap.find(hLayer);
	if(iter == m_LayerMap.end())
	{
		HyGuiLog("TextFontManager::GetLeftSideNudgeAmt passed an invalid handle", LOGTYPE_Error);
		return 0.0f;
	}

	return iter.value()->m_fLeftSideNudgeAmt;
}

TextLayerHandle TextFontManager::AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize)
{
	TextLayerHandle hNewLayer = HY_UNUSED_HANDLE;

	int iFontIndex = DoesFontExist(sFontName, eRenderMode, fOutlineThickness, fSize);
	if(iFontIndex < 0)
	{
		iFontIndex = CreatePreviewFont(sFontName, eRenderMode, fOutlineThickness, fSize);
		while(iFontIndex == TEXTFONTERROR_AtlasFull)
		{
			ClearAndEmbiggenAtlas();
			InitAtlas();
			iFontIndex = CreatePreviewFont(sFontName, eRenderMode, fOutlineThickness, fSize);
		}
	}
	if(iFontIndex < 0)
	{
		HyGuiLog("TextFontManager::AddNewLayer failed to create preview font. Error code: " % QString::number(iFontIndex), LOGTYPE_Error);
		return hNewLayer;
	}
	
	hNewLayer = ++sm_hHandleCount;
	m_LayerMap[hNewLayer] = new Layer(hNewLayer, iFontIndex, QColor(Qt::black), QColor(Qt::black), 0.0f, 0.0f, 0.0f, 0.0f);

	RegenFontArray();
	return hNewLayer;
}

void TextFontManager::SetAtlasGroup(quint32 uiBankId)
{
	AtlasModel &atlasModelRef = m_GlyphsModel.GetOwner().GetProject().GetAtlasModel();
	m_GlyphsModel.SetPropertyValue("Atlas Info", TEXTPROP_AtlasGroup, atlasModelRef.GetBankName(atlasModelRef.GetBankIndexFromBankId(uiBankId)));
}

void TextFontManager::SetFont(TextLayerHandle hLayer, QString sFontName)
{
	int iFontIndex = DoesFontExist(sFontName, GetRenderMode(hLayer), GetOutlineThickness(hLayer), GetSize(hLayer));
	if(iFontIndex < 0)
	{
		iFontIndex = CreatePreviewFont(sFontName, GetRenderMode(hLayer), GetOutlineThickness(hLayer), GetSize(hLayer));
		while(iFontIndex == TEXTFONTERROR_AtlasFull)
		{
			ClearAndEmbiggenAtlas();
			InitAtlas();
			iFontIndex = CreatePreviewFont(sFontName, GetRenderMode(hLayer), GetOutlineThickness(hLayer), GetSize(hLayer));
		}
	}
	if(iFontIndex < 0)
	{
		HyGuiLog("TextFontManager::SetFont failed to create preview font. Error code: " % QString::number(iFontIndex), LOGTYPE_Error);
		return;
	}

	m_LayerMap[hLayer]->m_iFontIndex = iFontIndex;
	RegenFontArray();
}

void TextFontManager::SetFontSize(TextLayerHandle hLayer, float fSize)
{
	int iFontIndex = DoesFontExist(GetFontName(hLayer), GetRenderMode(hLayer), GetOutlineThickness(hLayer), fSize);
	if(iFontIndex < 0)
	{
		iFontIndex = CreatePreviewFont(GetFontName(hLayer), GetRenderMode(hLayer), GetOutlineThickness(hLayer), fSize);
		while(iFontIndex == TEXTFONTERROR_AtlasFull)
		{
			ClearAndEmbiggenAtlas();
			InitAtlas();
			iFontIndex = CreatePreviewFont(GetFontName(hLayer), GetRenderMode(hLayer), GetOutlineThickness(hLayer), fSize);
		}
	}
	if(iFontIndex < 0)
	{
		HyGuiLog("TextFontManager::SetFontSize failed to create preview font. Error code: " % QString::number(iFontIndex), LOGTYPE_Error);
		return;
	}

	m_LayerMap[hLayer]->m_iFontIndex = iFontIndex;
	RegenFontArray();
}

void TextFontManager::SetRenderMode(TextLayerHandle hLayer, rendermode_t eMode)
{
	int iFontIndex = DoesFontExist(GetFontName(hLayer), eMode, GetOutlineThickness(hLayer), GetSize(hLayer));
	if(iFontIndex < 0)
	{
		iFontIndex = CreatePreviewFont(GetFontName(hLayer), eMode, GetOutlineThickness(hLayer), GetSize(hLayer));
		while(iFontIndex == TEXTFONTERROR_AtlasFull)
		{
			ClearAndEmbiggenAtlas();
			InitAtlas();
			iFontIndex = CreatePreviewFont(GetFontName(hLayer), eMode, GetOutlineThickness(hLayer), GetSize(hLayer));
		}
	}
	if(iFontIndex < 0)
	{
		HyGuiLog("TextFontManager::SetRenderMode failed to create preview font. Error code: " % QString::number(iFontIndex), LOGTYPE_Error);
		return;
	}

	m_LayerMap[hLayer]->m_iFontIndex = iFontIndex;
	RegenFontArray();
}

void TextFontManager::SetOutlineThickness(TextLayerHandle hLayer, float fThickness)
{
	int iFontIndex = DoesFontExist(GetFontName(hLayer), GetRenderMode(hLayer), fThickness, GetSize(hLayer));
	if(iFontIndex < 0)
	{
		iFontIndex = CreatePreviewFont(GetFontName(hLayer), GetRenderMode(hLayer), fThickness, GetSize(hLayer));
		while(iFontIndex == TEXTFONTERROR_AtlasFull)
		{
			ClearAndEmbiggenAtlas();
			InitAtlas();
			iFontIndex = CreatePreviewFont(GetFontName(hLayer), GetRenderMode(hLayer), fThickness, GetSize(hLayer));
		}
	}
	if(iFontIndex < 0)
	{
		HyGuiLog("TextFontManager::SetOutlineThickness failed to create preview font. Error code: " % QString::number(iFontIndex), LOGTYPE_Error);
		return;
	}
	
	m_LayerMap[hLayer]->m_iFontIndex = iFontIndex;
	RegenFontArray();
}

void TextFontManager::SetColors(TextLayerHandle hLayer, const QColor &topColor, const QColor &botColor)
{
	m_LayerMap[hLayer]->m_TopColor = topColor;
	m_LayerMap[hLayer]->m_BotColor = botColor;
}

void TextFontManager::RegenAtlas()
{
	RegenFontArray();

	m_uiPreviewAtlasGrowSize = 0;
	ClearAndEmbiggenAtlas();

	m_uiPreviewAtlasGrowSize = FONTMANAGER_PreviewGrowSize;
	InitAtlas();

	RegenFontArray();
}

void TextFontManager::CleanupLayers(const QList<IStateData *> &statesListRef)
{
	for(auto iter = m_LayerMap.begin(); iter != m_LayerMap.end();)
	{
		bool bFound = false;
		for(int i = 0; i < statesListRef.size(); ++i)
		{
			TextLayersModel &layerModelRef = static_cast<TextStateData *>(statesListRef[i])->GetLayersModel();
			if(layerModelRef.HasHandle(iter.value()->m_hUNIQUE_ID))
			{
				bFound = true;
				break;
			}
		}

		if(bFound == false)
			iter = m_LayerMap.erase(iter);
		else
			++iter;
	}
}

void TextFontManager::GenerateOptimizedAtlas()
{
	// Get pristine 'm_pPreviewAtlas' and calculate approx 'm_uiPreviewAtlasDimension'
	if(m_pPreviewAtlas)
	{
		RegenFontArray();
		m_uiPreviewAtlasGrowSize = 0;
		ClearAndEmbiggenAtlas();
	}
	InitAtlas();
	m_uiPreviewAtlasDimension = sqrt(static_cast<double>(m_pPreviewAtlas->used));

	// Now regen with approx dimensions
	m_uiPreviewAtlasGrowSize = 0;
	ClearAndEmbiggenAtlas();
	m_uiPreviewAtlasGrowSize = FONTMANAGER_OptimizedGrowSize;
	InitAtlas();

	// Set control variables back to their defaults
	m_bPreviewAtlasPixelDataInitialized = false;
	m_uiPreviewAtlasGrowSize = FONTMANAGER_PreviewGrowSize;
}

unsigned char *TextFontManager::GetAtlasInfo(uint &uiAtlasPixelDataSizeOut, QSize &atlasDimensionsOut)
{
	InitAtlas();

	if(m_bPreviewAtlasPixelDataInitialized == false)
	{
		// Make a fully white texture in 'pBuffer', then using the single channel from 'texture_atlas_t', overwrite the alpha channel
		memset(m_pPreviewAtlasPixelData, 0xFF, m_uiPreviewAtlasBufferSize);

		// Overwriting alpha channel
		uint uiNumPixels = static_cast<uint>(m_uiPreviewAtlasDimension * m_uiPreviewAtlasDimension);
		for(uint i = 0; i < uiNumPixels; ++i)
			m_pPreviewAtlasPixelData[i*4+3] = m_pPreviewAtlas->data[i];

		RegenFontArray();
		m_bPreviewAtlasPixelDataInitialized = true;
	}

	uiAtlasPixelDataSizeOut = m_uiPreviewAtlasBufferSize;
	atlasDimensionsOut.setWidth(m_uiPreviewAtlasDimension);
	atlasDimensionsOut.setHeight(m_uiPreviewAtlasDimension);

	return m_pPreviewAtlasPixelData;
}

int TextFontManager::DoesFontExist(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize)
{
	// Checks for existing font in JSON array (m_FontArray)
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

int TextFontManager::CreatePreviewFont(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize)
{
	m_bPreviewAtlasPixelDataInitialized = false;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Get font path
	QStandardItemModel *pProjectFontsModel = m_GlyphsModel.GetOwner().GetProject().GetFontListModel();
	int iNumFonts = pProjectFontsModel->rowCount();
	QString sFontPath;
	for(int i = 0; i < iNumFonts; ++i)
	{
		QString sTest = pProjectFontsModel->item(i)->text();
		if(pProjectFontsModel->item(i)->text().compare(sFontName, Qt::CaseInsensitive) == 0)
			sFontPath = pProjectFontsModel->item(i)->data().toString();
	}
	if(sFontPath.isEmpty())
	{
		HyGuiLog("TextFontManager::CreatePreviewFont could not find font: " % sFontName, LOGTYPE_Error);
		return TEXTFONTERROR_FontNotFound;
	}

	QString sGlyphList = GetAvailableTypefaceGlyphs();

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Allocate new font onto atlas
	PreviewFont *pNewPreviewFont = new PreviewFont(m_pPreviewAtlas,
		sGlyphList,
		sFontPath,
		fSize,
		fOutlineThickness,
		eRenderMode);

	if(pNewPreviewFont->GetMissedGlyphs() > 0)
	{
		HyGuiLog("Font preview could not fit '" % QString::number(pNewPreviewFont->GetMissedGlyphs()) % "' glyphs on atlas.", LOGTYPE_Info);
		delete pNewPreviewFont;

		return TEXTFONTERROR_AtlasFull;
	}

	m_PreviewFontList.append(pNewPreviewFont);

	int iFontIndex = m_PreviewFontList.size() - 1;
	for(auto iter = m_LayerMap.begin(); iter != m_LayerMap.end(); ++iter)
	{
		if(iter.value()->m_iFontIndex == iFontIndex)
		{
			iter.value()->m_fLineHeight = pNewPreviewFont->GetTextureFont()->height;
			iter.value()->m_fLineAscender = pNewPreviewFont->GetTextureFont()->ascender;
			iter.value()->m_fLineDescender = pNewPreviewFont->GetTextureFont()->descender;

			for(int j = 0; j < sGlyphList.count(); ++j)
			{
				// NOTE: Assumes LITTLE ENDIAN
				QString sSingleChar = sGlyphList[j];
				texture_glyph_t *pGlyph = texture_font_get_glyph(pNewPreviewFont->GetTextureFont(), sSingleChar.toUtf8().data());

				// Only keep track of negative offset_x's
				if(pGlyph->offset_x < 0 && iter.value()->m_fLeftSideNudgeAmt < abs(pGlyph->offset_x))
					iter.value()->m_fLeftSideNudgeAmt = abs(pGlyph->offset_x);
			}
			break;
		}
	}

	return iFontIndex;
}

int TextFontManager::InitAtlas()
{
	if(m_pPreviewAtlasPixelData != nullptr)
		return TEXTFONTERROR_Success;

	if(m_pPreviewAtlas) // This shouldn't happen
		delete m_pPreviewAtlas;
	
	m_pPreviewAtlas = texture_atlas_new(m_uiPreviewAtlasDimension, m_uiPreviewAtlasDimension, 1);
	m_uiPreviewAtlasBufferSize = static_cast<uint>(m_uiPreviewAtlasDimension * m_uiPreviewAtlasDimension * 4);
	m_pPreviewAtlasPixelData = new unsigned char[m_uiPreviewAtlasBufferSize];

	// This creates the preview fonts of the already existing fonts in 'm_FontArray'
	int iNumFonts = m_FontArray.size();
	for(int i = 0; i < iNumFonts; ++i)
	{
		QJsonObject fontObj = m_FontArray[i].toObject();
		switch(CreatePreviewFont(fontObj["font"].toString(), static_cast<rendermode_t>(fontObj["mode"].toInt()), fontObj["outlineThickness"].toDouble(), fontObj["size"].toDouble()))
		{
		case TEXTFONTERROR_AtlasFull:
			ClearAndEmbiggenAtlas();
			
			m_pPreviewAtlas = texture_atlas_new(m_uiPreviewAtlasDimension, m_uiPreviewAtlasDimension, 1);
			m_uiPreviewAtlasBufferSize = static_cast<uint>(m_uiPreviewAtlasDimension * m_uiPreviewAtlasDimension * 4);
			m_pPreviewAtlasPixelData = new unsigned char[m_uiPreviewAtlasBufferSize];

			i = -1; // Atlas has been enlarged, start loop over at '0', which will also recreate the atlas
			break;

		case TEXTFONTERROR_FontNotFound:
			return TEXTFONTERROR_FontNotFound;
		}
	}

	return TEXTFONTERROR_Success;
}

void TextFontManager::ClearAndEmbiggenAtlas()
{
	for(int i = 0; i < m_PreviewFontList.size(); ++i)
		delete m_PreviewFontList[i];
	m_PreviewFontList.clear();

	texture_atlas_delete(m_pPreviewAtlas);
	m_pPreviewAtlas = nullptr;

	delete [] m_pPreviewAtlasPixelData;
	m_pPreviewAtlasPixelData = nullptr;
	m_uiPreviewAtlasBufferSize = 0;

	m_uiPreviewAtlasDimension += m_uiPreviewAtlasGrowSize;
	HyGuiLog("Preview font's atlas has been embiggened to: " % QString::number(m_uiPreviewAtlasDimension), LOGTYPE_Info);
}

void TextFontManager::RegenFontArray()
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Cleanup unused fonts
	int iFontIndex = 0;
	for(auto iter = m_PreviewFontList.begin(); iter != m_PreviewFontList.end(); ++iFontIndex)
	{
		bool bFontUsed = false;
		for(auto iterLayer = m_LayerMap.begin(); iterLayer != m_LayerMap.end(); ++iterLayer)
		{
			if(iterLayer.value()->m_iFontIndex == iFontIndex)
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
				if(iterLayer.value()->m_iFontIndex > iFontIndex)
					iterLayer.value()->m_iFontIndex--;
			}

			iFontIndex--; // Subtract one here to account for the erased font
		}
		else
			++iter;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Create JSON Array to replace 'm_FontArray'
	QString sAvailGlyphs = GetAvailableTypefaceGlyphs();

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

	// Always append Unicode Character 'REPLACEMENT CHARACTER' (U+FFFD) to be used for invalid glyphs
	sAvailableTypefaceGlyphs.append(QChar(65533));

	// Finally append the "Additional glyphs" specified, discarding any duplicates
	propValue = m_GlyphsModel.FindPropertyValue("Uses Glyphs", TEXTPROP_AdditionalSyms);
	QString sAdditionalGlyphs = propValue.toString();
	for(int i = 0; i < sAdditionalGlyphs.size(); ++i)
	{
		if(sAvailableTypefaceGlyphs.contains(sAdditionalGlyphs[i]) == false)
			sAvailableTypefaceGlyphs += sAdditionalGlyphs[i];
	}

	return sAvailableTypefaceGlyphs;
}
