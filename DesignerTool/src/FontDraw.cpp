#include "FontDraw.h"
#include "FontWidget.h"
#include "MainWindow.h"

FontDraw::FontDraw(ProjectItem *pProjItem, IHyApplication &hyApp) : IDraw(pProjItem, hyApp),
                                                                    m_bShowAtlasPreview(false),
                                                                    m_pAtlasCamera(nullptr),
                                                                    m_pDrawAtlasPreview(nullptr)
{
    m_pAtlasCamera = m_HyAppRef.Window().CreateCamera2d();
    m_pAtlasCamera->SetViewport(0.0f, 0.0f, 1.0f, 0.5f);
    m_pAtlasCamera->pos.Set(0.0f, -2500.0f);
    
    m_DrawAtlasOutline.SetTint(1.0f, 0.0f, 0.0f);
    ChildAppend(m_DrawAtlasOutline);

    m_DividerLine.SetAsQuad(10000.0f, 10.0f, false);
    m_DividerLine.pos.Set(-5000.0f, hyApp.Window().GetResolution().y / 2 - 5.0f);
    m_DividerLine.SetTint(0.0f, 0.0f, 0.0f);
    //m_DividerLine.SetCoordinateType(HYCOORDTYPE_Screen, NULL);
    
    ChildAppend(m_DividerLine);
    
    ShowSubAtlas(m_bShowAtlasPreview);
}

/*virtual*/ FontDraw::~FontDraw()
{
    delete m_pDrawAtlasPreview;
    m_HyAppRef.Window().RemoveCamera(m_pAtlasCamera);
}

/*virtual*/ void FontDraw::OnApplyJsonData(jsonxx::Value &valueRef) /*override*/
{
}

void FontDraw::PositionDividerLine()
{
    m_DividerLine.pos.Set(-5000.0f, m_HyAppRef.Window().GetResolution().y / 2 - 5.0f);
}

void FontDraw::ShowSubAtlas(bool bShow)
{
    m_bShowAtlasPreview = bShow;
    
    if(m_bShowAtlasPreview)
        m_pCamera->SetViewport(0.0f, 0.5f, 1.0f, 0.5f);
    else
        m_pCamera->SetViewport(0.0f, 0.0f, 1.0f, 1.0f);
    
    m_pAtlasCamera->SetEnabled(m_bShowAtlasPreview);
}

void FontDraw::LoadNewAtlas(texture_atlas_t *pAtlas, unsigned char *pAtlasPixelData, uint uiAtlasPixelDataSize)
{
    if(m_pDrawAtlasPreview && m_pDrawAtlasPreview->GetGraphicsApiHandle() != 0)
        MainWindow::GetCurrentRenderer()->DeleteTextureArray(m_pDrawAtlasPreview->GetGraphicsApiHandle());

    // Upload texture to gfx api
    pAtlas->id = MainWindow::GetCurrentRenderer()->AddTexture(HYTEXTURE_R8G8B8A8, 0, static_cast<uint32>(pAtlas->width), static_cast<uint32>(pAtlas->height), pAtlasPixelData, uiAtlasPixelDataSize, HYTEXTURE_R8G8B8A8);

    // Create a (new) raw 'HyTexturedQuad2d' using a gfx api texture handle
    delete m_pDrawAtlasPreview;
    m_pDrawAtlasPreview = new HyTexturedQuad2d(pAtlas->id, static_cast<uint32>(pAtlas->width), static_cast<uint32>(pAtlas->height), nullptr);
    m_pDrawAtlasPreview->Load();
    m_pDrawAtlasPreview->SetTextureSource(0, 0, static_cast<uint32>(pAtlas->width), static_cast<uint32>(pAtlas->height));

    m_DrawAtlasOutline.SetAsQuad(static_cast<int>(pAtlas->width), static_cast<int>(pAtlas->height), true);

    // Calculate the proper zoom amount to fit the whole atlas width (plus some extra margin) in preview window
    float fExtraMargin = 25.0f;
    float fZoomAmt = ((m_HyAppRef.Window().GetResolution().x * 100.0f) / (pAtlas->width + (fExtraMargin * 2.0f))) / 100.0f;
    m_pCamera->SetZoom(fZoomAmt);

    HyRectangle<float> atlasViewBounds = m_pCamera->GetWorldViewBounds();
    m_pDrawAtlasPreview->pos.Set(atlasViewBounds.left + fExtraMargin, atlasViewBounds.top - pAtlas->height - fExtraMargin);
    m_DrawAtlasOutline.pos.Set(m_pDrawAtlasPreview->pos.Get());
}

void FontDraw::GenerateTextPreview(FontTableModel *pFontModel, QString sFontPreviewString, texture_atlas_t *pAtlas)
{
    // Generate m_DrawFontPreviewList here if font preview is dirty
    for(int i = 0; i < m_DrawFontPreviewList.count(); ++i)
        delete m_DrawFontPreviewList[i];

    m_DrawFontPreviewList.clear();

    m_pCamera->pos.Set(0.0f, 0.0f);
    glm::vec2 ptGlyphPos = m_pCamera->pos.Get();

    float fTextPixelLength = 0.0f;

    // Each font layer
    for(int i = 0; i < pFontModel->rowCount(); ++i)
    {
        ptGlyphPos.x = 0.0f;

        for(int j = 0; j < sFontPreviewString.count(); ++j)
        {
            FontTypeface *pFontStage = pFontModel->GetStageRef(i);

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
            ptGlyphPos.y = m_pCamera->pos.Y() - (pGlyph->height - pGlyph->offset_y);

            int iX = static_cast<int>(pGlyph->s0 * static_cast<float>(pAtlas->width));
            int iY = static_cast<int>(pGlyph->t0 * static_cast<float>(pAtlas->height));
            int iWidth = static_cast<int>(pGlyph->s1 * static_cast<float>(pAtlas->width)) - iX - 1;
            int iHeight = static_cast<int>(pGlyph->t1 * static_cast<float>(pAtlas->height)) - iY - 1;

            HyTexturedQuad2d *pDrawGlyphQuad = new HyTexturedQuad2d(pAtlas->id, static_cast<uint32>(pAtlas->width), static_cast<uint32>(pAtlas->height), nullptr);
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

    m_pCamera->pos.X(fTextPixelLength * 0.5f);
}

/*virtual*/ void FontDraw::OnShow(IHyApplication &hyApp)
{
    if(m_bShowAtlasPreview)
    {
        if(m_pDrawAtlasPreview)
            m_pDrawAtlasPreview->SetEnabled(true);
    
        m_DrawAtlasOutline.SetEnabled(true);
        m_DividerLine.SetEnabled(true);
    }
    
    m_pAtlasCamera->SetEnabled(m_bShowAtlasPreview);

}

/*virtual*/ void FontDraw::OnHide(IHyApplication &hyApp)
{
    SetEnabled(false);
}

/*virtual*/ void FontDraw::OnUpdate() /*override*/
{
    texture_atlas_t *pAtlas = static_cast<FontModel *>(m_pProjItem->GetModel())->GetAtlas();
    unsigned char *pAtlasPixelData = static_cast<FontModel *>(m_pProjItem->GetModel())->GetAtlasPixelData();
    uint uiAtlasPixelDataSize = static_cast<FontModel *>(m_pProjItem->GetModel())->GetAtlasPixelDataSize();
    if(pAtlas == nullptr || pAtlasPixelData == nullptr)
        return;

    if(pAtlas->id == 0)
        LoadNewAtlas(pAtlas, pAtlasPixelData, uiAtlasPixelDataSize);
    
    if(static_cast<FontModel *>(m_pProjItem->GetModel())->ClearFontDirtyFlag())
        GenerateTextPreview(static_cast<FontWidget *>(m_pProjItem->GetWidget())->GetCurStateData()->GetFontModel(), "The quick brown fox jumped over the lazy dog. 01234567890", pAtlas);
}
