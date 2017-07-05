/**************************************************************************
 *	FontModels.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTMODELS_H
#define FONTMODELS_H

#include "ProjectItem.h"
#include "FontModelView.h"
#include "IModel.h"
#include "AtlasFrame.h"

#include <QJsonArray>

// TODO: Add combo box to font widget that can select which atlas group this font's sub-atlas belongs to
#define TEMP_FONT_ATLAS_INDEX 0

struct FontTypeface
{
    int                 iReferenceCount;
    int                 iTmpReferenceCount;

    QString             sFontPath;
    texture_font_t *    pTextureFont;
    rendermode_t        eMode;
    float               fSize;
    float               fOutlineThickness;

    FontTypeface(QString sFontFilePath, rendermode_t eRenderMode, float fSize, float fOutlineThickness) :   iReferenceCount(0),
                                                                                                            iTmpReferenceCount(0),
                                                                                                            sFontPath(sFontFilePath),
                                                                                                            pTextureFont(NULL),
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
	FontTableModel *			m_pFontTableModel;
	
	DoubleSpinBoxMapper *		m_pSbMapper_Size;
    ComboBoxMapper *            m_pCmbMapper_Fonts;
	
public:
    FontStateData(IModel &modelRef, QJsonObject stateObj);
    virtual ~FontStateData();

    FontTableModel *GetFontModel();

    DoubleSpinBoxMapper *GetSizeMapper();
    ComboBoxMapper *GetFontMapper();

    void GetStateInfo(QJsonObject &stateObjOut);

    QString GetFontFilePath();
    float GetSize();
    
    virtual void AddFrame(AtlasFrame *pFrame) override { }
    virtual void RelinquishFrame(AtlasFrame *pFrame) override { }
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class FontModel : public IModel
{
    Q_OBJECT

    QDir                        m_FontMetaDir;
    
    CheckBoxMapper *            m_pChkMapper_09;
    CheckBoxMapper *            m_pChkMapper_AZ;
    CheckBoxMapper *            m_pChkMapper_az;
    CheckBoxMapper *            m_pChkMapper_Symbols;
    LineEditMapper *            m_pTxtMapper_AdditionalSymbols;
	
	AtlasFrame *                m_pTrueAtlasFrame;

    QString                     m_sAvailableTypefaceGlyphs;
	
	QJsonArray					m_TypefaceArray;

    QList<FontTypeface *>       m_MasterStageList;
    bool                        m_bGlyphsDirty;
    bool                        m_bFontPreviewDirty;

    texture_atlas_t *           m_pAtlas;
    unsigned char *             m_pTrueAtlasPixelData;
    uint                        m_uiTrueAtlasPixelDataSize;

public:
    FontModel(ProjectItem *pItem, QJsonObject fontObj);
    virtual ~FontModel();

    QDir GetMetaDir();

    CheckBoxMapper *Get09Mapper();
    CheckBoxMapper *GetAZMapper();
    CheckBoxMapper *GetazMapper();
    CheckBoxMapper *GetSymbolsMapper();
    LineEditMapper *GetAdditionalSymbolsMapper();

    QList<FontTypeface *> GetMasterStageList();
	
	QJsonObject GetTypefaceObj(int iTypefaceIndex);
    
    texture_atlas_t *GetAtlas();
    unsigned char *GetAtlasPixelData();
    uint GetAtlasPixelDataSize();
    
    void GeneratePreview(bool bStoreIntoAtlasManager = false);
    
    void SetGlyphsDirty();

    bool ClearFontDirtyFlag();

    virtual QJsonObject PopStateAt(uint32 uiIndex) override;
    virtual QJsonValue GetSaveInfo() override;
    virtual void Refresh() override;
};

#endif // FONTMODELS_H
