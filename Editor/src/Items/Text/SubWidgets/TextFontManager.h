/**************************************************************************
*	TextFontManager.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef TEXTFONTMANAGER_H
#define TEXTFONTMANAGER_H

#include "PropertiesTreeModel.h"
#include "vendor/freetype-gl/freetype-gl.h"

#include <QFileInfo>

typedef int TextLayerHandle;

class TextFontManager
{
	static TextLayerHandle			sm_hHandleCount;
	PropertiesTreeModel				m_GlyphsModel;

	// NOTE: 'm_FontArray' must always be kept up to date with any modifications. All layer-queried accessors utilize 'm_FontArray' when
	//       returning data. If any data changes then the generated preview 'texture_atlas_t'/'texture_font_t' must be used to create a new 'm_FontArray'
	QJsonArray						m_FontArray;

	struct Layer
	{
		const TextLayerHandle		m_hUNIQUE_ID;

		int							m_iFontIndex;
		QColor						m_BotColor;
		QColor						m_TopColor;

		float						m_fLeftSideNudgeAmt;
		float						m_fLineAscender;
		float						m_fLineDescender;
		float						m_fLineHeight;

		Layer(TextLayerHandle m_hUniqueId, int iFontIndex, QColor botColor, QColor topColor, float fLeftSideNudgeAmt, float fLineAscender, float fLineDescender, float fLineHeight) :
			m_hUNIQUE_ID(m_hUniqueId),
			m_iFontIndex(iFontIndex),
			m_BotColor(botColor),
			m_TopColor(topColor),
			m_fLeftSideNudgeAmt(fLeftSideNudgeAmt),
			m_fLineAscender(fLineAscender),
			m_fLineDescender(fLineDescender),
			m_fLineHeight(fLineHeight)
		{ }
	};
	QMap<TextLayerHandle, Layer *>	m_LayerMap;

	class PreviewFont
	{
		texture_font_t *			m_pTextureFont;
		size_t						m_uiMissedGlyphs;

	public:
		PreviewFont(texture_atlas_t *pFtglAtlas, QString sGlyphs, QString sFontFilePath, float fSize, float fThickness, rendermode_t eMode) :
			m_pTextureFont(nullptr),
			m_uiMissedGlyphs(0)
		{
			m_pTextureFont = texture_font_new_from_file(pFtglAtlas, fSize, sFontFilePath.toStdString().c_str());
			if(m_pTextureFont == nullptr)
			{
				HyGuiLog("Could not create freetype font from: " % sFontFilePath, LOGTYPE_Warning);
				return;
			}

			m_pTextureFont->size = fSize;
			m_pTextureFont->rendermode = eMode;
			m_pTextureFont->outline_thickness = fThickness;
			m_pTextureFont->padding = 1;

			m_uiMissedGlyphs = texture_font_load_glyphs(m_pTextureFont, sGlyphs.toUtf8().data());
		}
		~PreviewFont() {
			texture_font_delete(m_pTextureFont);
		}
		texture_font_t *GetTextureFont() {
			return m_pTextureFont;
		}
		size_t GetMissedGlyphs() {
			return m_uiMissedGlyphs;
		}
	};
	QList<PreviewFont *>			m_PreviewFontList;

	texture_atlas_t *				m_pPreviewAtlas;
	unsigned char *					m_pPreviewAtlasPixelData;
	uint							m_uiPreviewAtlasBufferSize;
	uint							m_uiPreviewAtlasGrowSize;
	uint							m_uiPreviewAtlasDimension;
	bool							m_bPreviewAtlasPixelDataInitialized;

public:
	TextFontManager(ProjectItemData &itemRef, QJsonObject availableGlyphsObj, QJsonArray fontArray);
	~TextFontManager();

	PropertiesTreeModel *GetGlyphsModel();
	const PropertiesTreeModel *GetGlyphsModel() const;

	QList<TextLayerHandle> RegisterLayers(QJsonObject stateObj);
	QJsonArray GetFontArray() const;

	int GetFontIndex(TextLayerHandle hLayer) const;
	QString GetFontName(TextLayerHandle hLayer) const;
	QString GetFontPath(TextLayerHandle hLayer);
	rendermode_t GetRenderMode(TextLayerHandle hLayer) const;
	float GetOutlineThickness(TextLayerHandle hLayer) const;
	float GetSize(TextLayerHandle hLayer) const;
	void GetColor(TextLayerHandle hLayer, QColor &topColorOut, QColor &botColorOut) const;

	float GetLineHeight(TextLayerHandle hLayer) const;
	float GetLineAscender(TextLayerHandle hLayer) const;
	float GetLineDescender(TextLayerHandle hLayer) const;
	float GetLeftSideNudgeAmt(TextLayerHandle hLayer) const;

	TextLayerHandle AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize);
	void SetAtlasGroup(quint32 uiAtlasGroupId);
	void SetFont(TextLayerHandle hLayer, QString sFontName);
	void SetFontSize(TextLayerHandle hLayer, float fSize);
	void SetRenderMode(TextLayerHandle hLayer, rendermode_t eMode);
	void SetOutlineThickness(TextLayerHandle hLayer, float fThickness);
	void SetColors(TextLayerHandle hLayer, const QColor &topColor, const QColor &botColor);

	void RegenAtlas();
	void CleanupLayers(const QList<IStateData *> &statesListRef);
	void GenerateOptimizedAtlas();
	unsigned char *GetAtlasInfo(uint &uiAtlasPixelDataSizeOut, QSize &atlasDimensionsOut);

private:
	int DoesFontExist(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize);
	QString GetFontPathFromName(QString sFontName);
	int CreatePreviewFont(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize);

	int InitAtlas();
	void ClearAndEmbiggenAtlas();

	void RegenFontArray();

	QString GetAvailableTypefaceGlyphs() const;
};

#endif // TEXTFONTMANAGER_H
