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

#include "MainWindow.h"
#include "WidgetFont.h"

#include "Harmony/HyEngine.h"

ItemFont::ItemFont(const QString sPath, WidgetAtlasManager &atlasManRef) :  ItemWidget(ITEM_Font, sPath, atlasManRef),
                                                                            m_pDrawPreview(NULL),
                                                                            m_pFontCamera(NULL)
{
}

/*virtual*/ ItemFont::~ItemFont()
{
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
    m_pWidget = new WidgetFont(this);
    
    if(m_pFontCamera == NULL)
        m_pFontCamera = hyApp.Window().CreateCamera2d();
    
    m_pCamera->SetViewport(0.0f, 0.5f, 1.0f, 0.5f);
    m_pCamera->pos.Set(0.0f, 2500.0f);
    
    m_pFontCamera->SetViewport(0.0f, 0.0f, 1.0f, 0.5f);
    m_pFontCamera->pos.Set(0.0f, -2500.0f);
    
    m_DividerLine.SetAsQuad(10000.0f, 10.0f, false);
    m_DividerLine.pos.Set(-5000.0f, -5.0f);
    m_DividerLine.color.Set(0.0f, 0.0f, 0.0f, 1.0f);
    m_DividerLine.SetCoordinateType(HYCOORDTYPE_Screen, NULL);
    m_DividerLine.Load();
}

/*virtual*/ void ItemFont::OnUnload(IHyApplication &hyApp)
{
    if(m_pDrawPreview)
        m_pDrawPreview->Unload();
    
    m_DividerLine.Unload();
    
    delete m_pWidget;
}

/*virtual*/ void ItemFont::OnDraw_Show(IHyApplication &hyApp)
{
    if(m_pDrawPreview)
        m_pDrawPreview->SetEnabled(true);
    
    if(m_pFontCamera)
        m_pFontCamera->SetEnabled(true);
    
    m_DividerLine.SetEnabled(true);
}

/*virtual*/ void ItemFont::OnDraw_Hide(IHyApplication &hyApp)
{
    if(m_pDrawPreview)
        m_pDrawPreview->SetEnabled(false);
    
    if(m_pFontCamera)
        m_pFontCamera->SetEnabled(false);
    
    m_DividerLine.SetEnabled(false);
}

/*virtual*/ void ItemFont::OnDraw_Update(IHyApplication &hyApp)
{
    WidgetFont *pWidget = static_cast<WidgetFont *>(m_pWidget);
    texture_atlas_t *pAtlas = pWidget->GetAtlas();
    
    if(pAtlas)
    {
        if(pAtlas->id == 0)
        {
            if(m_pDrawPreview && m_pDrawPreview->GetGraphicsApiHandle() != 0)
                MainWindow::GetCurrentRenderer()->DeleteTextureArray(m_pDrawPreview->GetGraphicsApiHandle());

            // Make a fully white texture in 'pBuffer', then using the single channel from 'texture_atlas_t', overwrite the alpha channel
            int iNumPixels = pAtlas->width * pAtlas->height;
            unsigned char *pBuffer = new unsigned char[iNumPixels * 4];
            memset(pBuffer, 0xFF, iNumPixels * 4);

            // Overwriting alpha channel
            for(int i = 0; i < iNumPixels; ++i)
                pBuffer[i*4+3] = pAtlas->data[i];

            // Upload texture to gfx api
            vector<unsigned char *> vPixelData;
            vPixelData.push_back(pBuffer);
            pAtlas->id = MainWindow::GetCurrentRenderer()->AddTextureArray(4/*pAtlas->depth*/, pAtlas->width, pAtlas->height, vPixelData);

            // Create a (new) raw 'HyTexturedQuad2d' using a gfx api texture handle
            delete m_pDrawPreview;
            m_pDrawPreview = new HyTexturedQuad2d(pAtlas->id, pAtlas->width, pAtlas->height);
            m_pDrawPreview->Load();
            m_pDrawPreview->SetCoordinateType(HYCOORDTYPE_Camera, NULL);
            m_pDrawPreview->SetTextureSource(0, 0, 0, pAtlas->width, pAtlas->height);
        }
        
        HyRectangle<float> atlasViewBounds = m_pCamera->GetWorldViewBounds();
        m_pDrawPreview->pos.Set(atlasViewBounds.left, atlasViewBounds.top - pAtlas->height); //hyApp.Window().GetResolution().x * -0.5f, -static_cast<float>(pAtlas->height) + (hyApp.Window().GetResolution().y * (m_pCamera->GetViewport().Height() * 0.5f)));
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

/*virtual*/ void ItemFont::OnSave()
{
}
