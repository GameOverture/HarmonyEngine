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

typedef int TextFontHandle;
#define TEXTFONTHANDLE_NotUsed -1

class TextFontManager
{
	PropertiesTreeModel		m_GlyphsModel;
	QJsonArray				m_FontArray;

	texture_atlas_t *		m_pAtlas;

	class TextFont
	{
		QFileInfo			m_FontFileInfo;
		texture_font_t *	m_pTextureFont;
		size_t				m_uiMissedGlyphs;

	public:
		TextFont(texture_atlas_t *pFtglAtlas, QString sGlyphs, QString sFontFilePath, float fSize, float fThickness, rendermode_t eMode) :
			m_FontFileInfo(sFontFilePath),
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
		~TextFont()
		{
			texture_font_delete(m_pTextureFont);
		}

		//bool (const TextFont &rightRef)
		//{
		//	return m_FontFileInfo.baseName().compare(rightRef.m_FontFileInfo.baseName(), Qt::CaseInsensitive) == 0 &&
		//		m_pTextureFont->size == rightRef.m_pTextureFont->size &&
		//		m_pTextureFont->outline_thickness == rightRef.m_pTextureFont->outline_thickness &&
		//		m_pTextureFont->rendermode == rightRef.m_pTextureFont->rendermode;
		//}
	};
	QList<TextFont *>		m_FontList;

public:
	TextFontManager(ProjectItem &itemRef, QJsonObject availableGlyphsObj, QJsonArray fontArray);
	~TextFontManager();

	QJsonObject GetAvailableGlyphsObject() const;
	QJsonArray GetFontArray() const;

	TextFontHandle AcquireFont(QString sFontName, rendermode_t eRenderMode, float fSize, float fOutlineThickness);

	PropertiesTreeModel *GetGlyphsModel();

private:
	void PrepPreview();
	void RegenFontArray();
	QString GetAvailableTypefaceGlyphs() const;
};

#endif // TEXTFONTMANAGER_H
