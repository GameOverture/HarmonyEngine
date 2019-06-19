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
#include "freetype-gl/freetype-gl.h"

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

		uint						m_uiFontIndex;
		glm::vec3					m_vBotColor;
		glm::vec3					m_vTopColor;

		Layer(TextLayerHandle m_hUniqueId, uint uiFontIndex, glm::vec3 vBotColor, glm::vec3 vTopColor) :
			m_hUNIQUE_ID(m_hUniqueId),
			m_uiFontIndex(uiFontIndex),
			m_vBotColor(vBotColor),
			m_vTopColor(vTopColor)
		{ }
	};
	QMap<TextLayerHandle, Layer *>	m_LayerMap;

	texture_atlas_t *				m_pPreviewAtlas;

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
				HyGuiLog("Could not create freetype font from: " % sFontFilePath, LOGTYPE_Error);
				return;
			}

			m_pTextureFont->size = fSize;
			m_pTextureFont->rendermode = eMode;
			m_pTextureFont->outline_thickness = fThickness;

			m_uiMissedGlyphs = texture_font_load_glyphs(m_pTextureFont, sGlyphs.toUtf8().data());
		}
		~PreviewFont()
		{
			texture_font_delete(m_pTextureFont);
		}

		texture_font_t *GetTextureFont() {
			return m_pTextureFont;
		}

		size_t GetMissedGlyphs() {
			return m_uiMissedGlyphs;
		}

		//bool (const PreviewFont &rightRef)
		//{
		//	return m_FontFileInfo.baseName().compare(rightRef.m_FontFileInfo.baseName(), Qt::CaseInsensitive) == 0 &&
		//		m_pTextureFont->size == rightRef.m_pTextureFont->size &&
		//		m_pTextureFont->outline_thickness == rightRef.m_pTextureFont->outline_thickness &&
		//		m_pTextureFont->rendermode == rightRef.m_pTextureFont->rendermode;
		//}
	};
	QList<PreviewFont *>		m_PreviewFontList;

public:
	TextFontManager(ProjectItem &itemRef, QJsonObject availableGlyphsObj, QJsonArray fontArray);
	~TextFontManager();

	PropertiesTreeModel *GetGlyphsModel();
	const PropertiesTreeModel *GetGlyphsModel() const;

	QList<TextLayerHandle> RegisterLayers(QJsonArray layerArray);
	QJsonArray GetFontArray() const;

	rendermode_t GetRenderMode(TextLayerHandle hLayer);
	float GetOutlineThickness(TextLayerHandle hLayer);
	void GetColor(TextLayerHandle hLayer, glm::vec3 &vTopColorOut, glm::vec3 &vBotColorOut);

	TextLayerHandle AddNewLayer(QString sFontName, rendermode_t eRenderMode, float fOutlineThickness, float fSize);

private:
	void PrepPreview();
	void RegenFontArray();
	QString GetAvailableTypefaceGlyphs() const;
};

#endif // TEXTFONTMANAGER_H
