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

struct FontTypeface
{
    int                 iReferenceCount;
    int                 iTmpReferenceCount;

    QString             sFontPath;
    texture_font_t *    pTextureFont;
    rendermode_t        eMode;
    float               fSize;
    float               fOutlineThickness;

    FontTypeface(QString sFontFilePath, rendermode_t eRenderMode, float fSize, float fOutlineThickness) :  iReferenceCount(0),
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
    DoubleSpinBoxMapper *		m_pSbMapper_Thickness;
    ComboBoxMapper *            m_pCmbMapper_RenderMode;
    ComboBoxMapper *            m_pCmbMapper_Fonts;
	
public:
    FontStateData(IModel &modelRef, QJsonObject stateObj);
    virtual ~FontStateData();

    FontTableModel *GetFontModel();

    DoubleSpinBoxMapper *GetSizeMapper();
    DoubleSpinBoxMapper *GetThicknessMapper();
    ComboBoxMapper *GetRenderModeMapper();
    ComboBoxMapper *GetFontMapper();

    void GetStateInfo(QJsonObject &stateObjOut);

    QString GetFontFilePath();
    float GetSize();
    
	virtual void AddFrame(AtlasFrame *pFrame) { }
	virtual void RelinquishFrame(AtlasFrame *pFrame) { }
	virtual void RefreshFrame(AtlasFrame *pFrame) { }
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
    void GeneratePreview(bool bStoreIntoAtlasManager = false);
    
    void SetGlyphsDirty();

    bool ClearFontDirtyFlag();

    virtual QJsonObject PopStateAt(uint32 uiIndex);
    virtual QJsonValue GetSaveInfo();
};

#endif // FONTMODELS_H
