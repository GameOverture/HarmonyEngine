/**************************************************************************
 *	WidgetFont.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef FONTWIDGET_H
#define FONTWIDGET_H

#include "FontWidgetState.h"

#include <QWidget>
#include <QDir>
#include <QJsonArray>

#include "freetype-gl/freetype-gl.h"

namespace Ui {
class FontWidget;
}

class FontItem;
class FontTableModel;

struct FontStagePass
{
    int                 iReferenceCount;
    int                 iTmpReferenceCount;

    QString             sFontPath;
    texture_font_t *    pTextureFont;
    rendermode_t        eMode;
    float               fSize;
    float               fOutlineThickness;

    FontStagePass(QString sFontFilePath, rendermode_t eRenderMode, float fSize, float fOutlineThickness) :  iReferenceCount(0),
                                                                                                            iTmpReferenceCount(0),
                                                                                                            sFontPath(sFontFilePath),
                                                                                                            pTextureFont(NULL),
                                                                                                            eMode(eRenderMode),
                                                                                                            fSize(fSize),
                                                                                                            fOutlineThickness(fOutlineThickness)
    { }

    ~FontStagePass()
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

class FontWidget : public QWidget
{
    Q_OBJECT

    ProjectItem &               m_ItemRef;
    
    QString                     m_sAvailableTypefaceGlyphs;
    QRegExpValidator            m_PreviewValidator;

    QList<FontStagePass *>      m_MasterStageList;
    bool                        m_bGlyphsDirty;
    bool                        m_bFontPreviewDirty;

    QList<QAction *>            m_StateActionsList;
    FontWidgetState *           m_pCurFontState;
    
    texture_atlas_t *           m_pAtlas;
    unsigned char *             m_pTrueAtlasPixelData;
    AtlasFrame *                m_pTrueAtlasFrame;
    
    QDir                        m_FontMetaDir;
    
    bool                        m_bBlockGeneratePreview;

    QSize                       m_PrevAtlasSize;
    
public:
    explicit FontWidget(ProjectItem &itemRef, QWidget *parent = 0);
    ~FontWidget();

    void OnGiveMenuActions(QMenu *pMenu);
    
    ProjectItem &GetItem();

    QString GetFullItemName();

    QComboBox *GetCmbStates();
    
    FontTableModel *GetCurrentFontModel();
    
    void SetGlyphsDirty();
    
    void GeneratePreview(bool bStoreIntoAtlasManager = false);
    
    texture_atlas_t *GetAtlas();

    unsigned char *GetAtlasPixelData();
    
    QDir GetFontMetaDir();

    void UpdateActions();

    QString GetPreviewString();

    bool ClearFontDirtyFlag();
    
    bool SaveFontFilesToMetaDir();
    
    void GetSaveInfo(QJsonObject &fontObj);

private Q_SLOTS:
    void on_chk_09_clicked();

    void on_chk_az_clicked();

    void on_chk_AZ_clicked();

    void on_chk_symbols_clicked();

    void on_txtAdditionalSymbols_editingFinished();

    void on_cmbStates_currentIndexChanged(int index);

    void on_actionAddState_triggered();

    void on_actionRemoveState_triggered();
    
    void on_actionRenameState_triggered();
    
    void on_actionOrderStateBackwards_triggered();
    
    void on_actionOrderStateForwards_triggered();
    
    void on_actionAddLayer_triggered();
    
    void on_actionRemoveLayer_triggered();

    void on_actionOrderLayerDownwards_triggered();

    void on_actionOrderLayerUpwards_triggered();

    void on_txtPreviewString_editingFinished();

private:
    Ui::FontWidget *ui;
};

#endif // FONTWIDGET_H
