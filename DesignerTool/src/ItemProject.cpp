#include "ItemProject.h"
#include "WidgetAtlasManager.h"

#include "WidgetRenderer.h"

ItemProject::ItemProject(const QString sPath) : Item(ITEM_Project, sPath),
                                                m_eState(DRAWSTATE_Nothing),
                                                m_pCam(NULL)
{
    m_pAtlasManager = new WidgetAtlasManager(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlasManager;
    
    foreach(HyTexturedQuad2d *pAtlas, m_Atlases)
    {
        pAtlas->Unload();
    }
    
    delete m_pCam;
}

/*virtual*/ void ItemProject::Show()
{
    foreach(HyTexturedQuad2d *pAtlas, m_Atlases)
        pAtlas->SetEnabled(true);
    
    if(m_pCam)
        m_pCam->SetEnabled(true);
}

/*virtual*/ void ItemProject::Hide()
{
    foreach(HyTexturedQuad2d *pAtlas, m_Atlases)
        pAtlas->SetEnabled(false);
    
    if(m_pCam)
        m_pCam->SetEnabled(false);
}

/*virtual*/ void ItemProject::Draw(WidgetRenderer &renderer)
{
//    if(m_pCam == NULL)
//        m_pCam = renderer.Window().CreateCamera2d();
    
    //renderer
    // well shit
}

void ItemProject::SetAtlasGroupDrawState(int iAtlasGrpId)
{
    m_eState = DRAWSTATE_AtlasManager;
}
