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
                                                                            m_pDrawPreview(NULL)
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
}

/*virtual*/ void ItemFont::OnUnload(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemFont::OnDraw_Show(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemFont::OnDraw_Hide(IHyApplication &hyApp)
{
}

/*virtual*/ void ItemFont::OnDraw_Update(IHyApplication &hyApp)
{
    WidgetFont *pWidget = static_cast<WidgetFont *>(m_pWidget);
    texture_atlas_t *pAtlas = pWidget->GetAtlas();
    
    if(pAtlas->id == 0)
    {
        if(m_pDrawPreview && m_pDrawPreview->GetGraphicsApiHandle() != 0)
            MainWindow::GetCurrentRenderer()->DeleteTextureArray(m_pDrawPreview->GetGraphicsApiHandle());

        vector<unsigned char *> vPixelData;
        vPixelData.push_back(pAtlas->data);
        pAtlas->id = MainWindow::GetCurrentRenderer()->AddTextureArray(pAtlas->depth, pAtlas->width, pAtlas->height, vPixelData);
        
        delete m_pDrawPreview;
        m_pDrawPreview = new HyTexturedQuad2d(pAtlas->id, pAtlas->width, pAtlas->height);
        m_pDrawPreview->Load();
        m_pDrawPreview->SetTextureSource(0, 0, 0, pAtlas->width, pAtlas->height);
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
