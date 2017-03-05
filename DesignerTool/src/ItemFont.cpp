/**************************************************************************
 *	ItemFont.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemFont.h"

#include <QMenu>
#include <QAction>
#include <QUndoView>
#include <QJsonObject>
#include <QJsonDocument>

#include "MainWindow.h"
#include "WidgetFont.h"
#include "WidgetAtlasManager.h"

#include "Harmony/HyEngine.h"

ItemFont::ItemFont(const QString sPrefix, const QString sName, QJsonValue initVal, WidgetAtlasManager &atlasManRef, WidgetAudioManager &audioManRef) :  ItemWidget(ITEM_Font, sPrefix, sName, initVal, atlasManRef, audioManRef),
                                                                                                                                                        m_pDrawAtlasPreview(NULL),
                                                                                                                                                        m_pFontCamera(NULL)
{
    m_pWidget = new WidgetFont(this);
}

/*virtual*/ ItemFont::~ItemFont()
{
    delete m_pWidget;
}

/*virtual*/ QList<QAction *> ItemFont::GetActionsForToolBar()
{
    QList<QAction *> returnList;
    
    returnList.append(FindAction(m_pEditMenu->actions(), "Undo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "Redo"));
    returnList.append(FindAction(m_pEditMenu->actions(), "UndoSeparator"));
    
    return returnList;
}

/*virtual*/ void ItemFont::OnLoad(IHyApplication &hyApp)
{
    if(m_pFontCamera == NULL)
        m_pFontCamera = hyApp.Window().CreateCamera2d();
    
    m_pCamera->SetViewport(0.0f, 0.5f, 1.0f, 0.5f);
    m_pCamera->pos.Set(0.0f, 2500.0f);
    
    m_pFontCamera->SetViewport(0.0f, 0.0f, 1.0f, 0.5f);
    m_pFontCamera->pos.Set(0.0f, -2500.0f);
    
    m_DrawAtlasOutline.SetTint(1.0f, 0.0f, 0.0f);
    m_DrawAtlasOutline.Load();

    m_DividerLine.SetAsQuad(10000.0f, 10.0f, false);
    m_DividerLine.pos.Set(-5000.0f, hyApp.Window().GetResolution().y / 2 - 5.0f);
    m_DividerLine.SetTint(0.0f, 0.0f, 0.0f);
    m_DividerLine.SetCoordinateType(HYCOORDTYPE_Screen, NULL);
    m_DividerLine.Load();
}

/*virtual*/ void ItemFont::OnUnload(IHyApplication &hyApp)
{
    if(m_pDrawAtlasPreview)
        m_pDrawAtlasPreview->Unload();

    m_DrawAtlasOutline.Unload();
    
    m_DividerLine.Unload();
}

/*virtual*/ void ItemFont::OnDraw_Show(IHyApplication &hyApp)
{
    if(m_pDrawAtlasPreview)
        m_pDrawAtlasPreview->SetEnabled(true);

    m_DrawAtlasOutline.SetEnabled(true);
    
    if(m_pFontCamera)
        m_pFontCamera->SetEnabled(true);
    
    m_DividerLine.SetEnabled(true);
}

/*virtual*/ void ItemFont::OnDraw_Hide(IHyApplication &hyApp)
{
    if(m_pDrawAtlasPreview)
        m_pDrawAtlasPreview->SetEnabled(false);

    m_DrawAtlasOutline.SetEnabled(false);
    
    if(m_pFontCamera)
        m_pFontCamera->SetEnabled(false);
    
    m_DividerLine.SetEnabled(false);
}

/*virtual*/ void ItemFont::OnDraw_Update(IHyApplication &hyApp)
{
    WidgetFont *pWidget = static_cast<WidgetFont *>(m_pWidget);
    texture_atlas_t *pAtlas = pWidget->GetAtlas();

    m_DividerLine.pos.Set(-5000.0f, hyApp.Window().GetResolution().y / 2 - 5.0f);
    
    if(pAtlas == NULL)
        return;

    if(pAtlas->id == 0)
    {
        if(m_pDrawAtlasPreview && m_pDrawAtlasPreview->GetGraphicsApiHandle() != 0)
            MainWindow::GetCurrentRenderer()->DeleteTextureArray(m_pDrawAtlasPreview->GetGraphicsApiHandle());

        // Upload texture to gfx api
        std::vector<unsigned char *> vPixelData;
        vPixelData.push_back(pWidget->GetAtlasPixelData());
        uint32 uiNumSucceeded = 0;
        pAtlas->id = MainWindow::GetCurrentRenderer()->AddTextureArray(4 /*converted texture depth*/, pAtlas->width, pAtlas->height, vPixelData, uiNumSucceeded);

        // Create a (new) raw 'HyTexturedQuad2d' using a gfx api texture handle
        delete m_pDrawAtlasPreview;
        m_pDrawAtlasPreview = new HyTexturedQuad2d(pAtlas->id, pAtlas->width, pAtlas->height);
        m_pDrawAtlasPreview->Load();
        m_pDrawAtlasPreview->SetCoordinateType(HYCOORDTYPE_Camera, NULL);
        m_pDrawAtlasPreview->SetTextureSource(0, 0, pAtlas->width, pAtlas->height);

        m_DrawAtlasOutline.SetAsQuad(static_cast<int>(pAtlas->width), static_cast<int>(pAtlas->height), true);

        // Calculate the proper zoom amount to fit the whole atlas width (plus some extra margin) in preview window
        float fExtraMargin = 25.0f;
        float fZoomAmt = ((hyApp.Window().GetResolution().x * 100.0f) / (pAtlas->width + (fExtraMargin * 2.0f))) / 100.0f;
        m_pCamera->SetZoom(fZoomAmt);

        HyRectangle<float> atlasViewBounds = m_pCamera->GetWorldViewBounds();
        m_pDrawAtlasPreview->pos.Set(atlasViewBounds.left + fExtraMargin, atlasViewBounds.top - pAtlas->height - fExtraMargin);
        m_DrawAtlasOutline.pos.Set(m_pDrawAtlasPreview->pos.Get());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Generate m_DrawFontPreviewList here if font preview is dirty
    if(pWidget->ClearFontDirtyFlag())
    {
        for(int i = 0; i < m_DrawFontPreviewList.count(); ++i)
            delete m_DrawFontPreviewList[i];

        m_DrawFontPreviewList.clear();

        WidgetFontModel *pFontModel = pWidget->GetCurrentFontModel();

        QString sFontPreviewString = pWidget->GetPreviewString();

        m_pFontCamera->pos.Set(0.0f, -2500.0f);
        glm::vec2 ptGlyphPos = m_pFontCamera->pos.Get();

        float fTextPixelLength = 0.0f;

        // Each font layer
        for(int i = 0; i < pFontModel->rowCount(); ++i)
        {
            ptGlyphPos.x = 0.0f;

            for(int j = 0; j < sFontPreviewString.count(); ++j)
            {
                FontStagePass *pFontStage = pFontModel->GetStageRef(i);

                // NOTE: Assumes LITTLE ENDIAN
                QString sSingleChar = sFontPreviewString[j];
                texture_glyph_t *pGlyph = texture_font_get_glyph(pFontStage->pTextureFont, sSingleChar.toUtf8().data());

                if(pGlyph == NULL)
                {
                    return;
                }

                float fKerning = 0.0f;
                if(j != 0)
                {
                    char cPrevCharacter = sFontPreviewString.toStdString().c_str()[j - 1];
                    fKerning = texture_glyph_get_kerning(pGlyph, &cPrevCharacter);
                }

                ptGlyphPos.x += fKerning;
                ptGlyphPos.y = m_pFontCamera->pos.Y() - (pGlyph->height - pGlyph->offset_y);

                int iX = static_cast<int>(pGlyph->s0 * static_cast<float>(pAtlas->width));
                int iY = static_cast<int>(pGlyph->t0 * static_cast<float>(pAtlas->height));
                int iWidth = static_cast<int>(pGlyph->s1 * static_cast<float>(pAtlas->width)) - iX - 1;
                int iHeight = static_cast<int>(pGlyph->t1 * static_cast<float>(pAtlas->height)) - iY - 1;

                HyTexturedQuad2d *pDrawGlyphQuad = new HyTexturedQuad2d(pAtlas->id, pAtlas->width, pAtlas->height);
                pDrawGlyphQuad->Load();
                pDrawGlyphQuad->SetTextureSource(iX, iY, iWidth, iHeight);
                pDrawGlyphQuad->pos.Set(ptGlyphPos.x + pGlyph->offset_x, ptGlyphPos.y);
                
                QColor topColor = pFontModel->GetLayerTopColor(i);
                QColor botColor = pFontModel->GetLayerBotColor(i);
                pDrawGlyphQuad->topColor.Set(topColor.redF(), topColor.greenF(), topColor.blueF());
                pDrawGlyphQuad->botColor.Set(botColor.redF(), botColor.greenF(), botColor.blueF());

                pDrawGlyphQuad->SetDisplayOrder(i * -1);

                m_DrawFontPreviewList.append(pDrawGlyphQuad);

                ptGlyphPos.x += pGlyph->advance_x;
            }

            if(fTextPixelLength < ptGlyphPos.x)
                fTextPixelLength = ptGlyphPos.x;
        }

        m_pFontCamera->pos.X(fTextPixelLength * 0.5f);
    }
}

/*virtual*/ void ItemFont::OnLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemFont::OnReLink(HyGuiFrame *pFrame)
{
}

/*virtual*/ void ItemFont::OnUnlink(HyGuiFrame *pFrame)
{
}

/*virtual*/ QJsonValue ItemFont::OnSave()
{ 
    WidgetFont *pWidget = static_cast<WidgetFont *>(m_pWidget);
    pWidget->SaveFontFilesToMetaDir();
    
    pWidget->GeneratePreview(true);
    
    QJsonObject fontObj;
    pWidget->GetSaveInfo(fontObj);

    return fontObj;
}
