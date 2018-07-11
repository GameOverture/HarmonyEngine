/**************************************************************************
 *	FontModels.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Designer Tool License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTMODELS_H
#define FONTMODELS_H

#include "ProjectItem.h"
#include "FontModelView.h"
#include "IModel.h"
#include "AtlasFrame.h"
#include "GlobalWidgetMappers.h"
#include "PropertiesTreeModel.h"

#include <QJsonArray>

struct FontTypeface
{
	uint                uiReferenceCount;

	QString             sFontPath;
	texture_font_t *    pTextureFont;
	rendermode_t        eMode;
	float               fSize;
	float               fOutlineThickness;

	FontTypeface(QString sFontFilePath, rendermode_t eRenderMode, float fSize, float fOutlineThickness) :   uiReferenceCount(0),
																											sFontPath(sFontFilePath),
																											pTextureFont(nullptr),
																											eMode(eRenderMode),
																											fSize(fSize),
																											fOutlineThickness(fOutlineThickness)
	{ }

	~FontTypeface()
	{
		if(pTextureFont)
			texture_font_delete(pTextureFont);
	}

	void SetFont(texture_font_t *pNewFont)
	{
		if(pTextureFont)
			texture_font_delete(pTextureFont);

		pTextureFont = pNewFont;
		pTextureFont->size = fSize;
		pTextureFont->rendermode = eMode;
		pTextureFont->outline_thickness = fOutlineThickness;
	}
};

class FontStateData : public IStateData
{
	FontStateLayersModel *      m_pLayersModel;
	
	SpinBoxMapper *             m_pSbMapper_Size;
	ComboBoxMapper *            m_pCmbMapper_Fonts;
	
public:
	FontStateData(int iStateIndex, IModel &modelRef, QJsonObject stateObj);
	virtual ~FontStateData();

	FontStateLayersModel *GetFontLayersModel();

	SpinBoxMapper *GetSizeMapper();
	ComboBoxMapper *GetFontMapper();

	void GetStateInfo(QJsonObject &stateObjOut);

	QString GetFontFilePath();
	float GetSize();
	
	virtual int AddFrame(AtlasFrame *pFrame) override { return 0; }
	virtual void RelinquishFrame(AtlasFrame *pFrame) override { }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontModel : public IModel
{
	Q_OBJECT

	QDir                        m_FontMetaDir;

	PropertiesTreeModel			m_TypeFacePropertiesModel;
	
//	CheckBoxMapper *            m_pChkMapper_09;
//	CheckBoxMapper *            m_pChkMapper_AZ;
//	CheckBoxMapper *            m_pChkMapper_az;
//	CheckBoxMapper *            m_pChkMapper_Symbols;
//	LineEditMapper *            m_pTxtMapper_AdditionalSymbols;

	AtlasFrame *                m_pTrueAtlasFrame;

	QString                     m_sAvailableTypefaceGlyphs;

	QJsonArray                  m_TypefaceArray;

	QList<FontTypeface *>       m_MasterLayerList;

	texture_atlas_t *           m_pFtglAtlas;
	unsigned char *				m_pSubAtlasPixelData;
	uint						m_uiSubAtlasBufferSize;

public:
	FontModel(ProjectItem &itemRef, QJsonObject fontObj);
	virtual ~FontModel();

	QDir GetMetaDir();

	PropertiesTreeModel *GetTypefaceModel();

//	CheckBoxMapper *Get09Mapper();
//	CheckBoxMapper *GetAZMapper();
//	CheckBoxMapper *GetazMapper();
//	CheckBoxMapper *GetSymbolsMapper();
//	LineEditMapper *GetAdditionalSymbolsMapper();

	QList<FontTypeface *> GetMasterStageList();

	QJsonObject GetTypefaceObj(int iTypefaceIndex);
	
	texture_atlas_t *GetFtglAtlas();
	AtlasFrame *GetAtlasFrame();
	unsigned char *GetAtlasPreviewPixelData();
	uint GetAtlasPreviewPixelDataSize();
	
	void GeneratePreview();

	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;

private:
	QSize GetAtlasGrpSize();
};

#endif // FONTMODELS_H
