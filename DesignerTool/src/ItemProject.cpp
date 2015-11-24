#include "ItemProject.h"
#include "WidgetAtlasManager.h"

#include "WidgetRenderer.h"

ItemProject::ItemProject(const QString sPath) : Item(ITEM_Project, sPath),
                                                IHyApplication(HarmonyInit("Gui", sPath.toStdString().c_str())),
                                                m_eState(DRAWSTATE_Nothing),
                                                m_pCurAtlas(NULL),
                                                m_pCam(NULL)
{
    m_pAtlasManager = new WidgetAtlasManager(this);
    
    m_pRenderer = new HyGuiRenderer(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlasManager;
    
    delete m_pCurAtlas;
    delete m_pCam;
}

/*virtual*/ bool ItemProject::Initialize()
{
    return true;
}

/*virtual*/ bool ItemProject::Update()
{
    //if(GetItem())
    //    GetItem()->Draw(*this);

    return true;
}

/*virtual*/ bool ItemProject::Shutdown()
{
    return true;
}

/*virtual*/ void ItemProject::Show()
{
    if(m_pCurAtlas)
        m_pCurAtlas->SetEnabled(true);
    
    if(m_pCam)
        m_pCam->SetEnabled(true);
}

/*virtual*/ void ItemProject::Hide()
{
    if(m_pCurAtlas)
        m_pCurAtlas->SetEnabled(false);
    
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

void ItemProject::SetAtlasGroupDrawState(int iAtlasGrpId, bool bForceLoad)
{
    m_eState = DRAWSTATE_AtlasManager;
    
    if(bForceLoad == false || iAtlasGrpId == -1)
        return;
    
    if(m_pCurAtlas && (m_pCurAtlas->GetAtlasGroupId() != iAtlasGrpId || bForceLoad))
        delete m_pCurAtlas;
    
    m_pCurAtlas = new HyTexturedQuad2d(iAtlasGrpId);
    m_pCurAtlas->Load();
}
