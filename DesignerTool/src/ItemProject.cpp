#include "ItemProject.h"
#include "WidgetAtlasManager.h"

#include "WidgetRenderer.h"

ItemProject::ItemProject(const QString sPath) : Item(ITEM_Project, sPath),
                                                m_eState(DRAWSTATE_Nothing)
{
    m_pAtlasManager = new WidgetAtlasManager(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlasManager;
}

/*virtual*/ void ItemProject::Show()
{
    if(m_eState == DRAWSTATE_AtlasManager)
        m_pAtlasManager->Show();
}

/*virtual*/ void ItemProject::Hide()
{
    if(m_eState == DRAWSTATE_AtlasManager)
        m_pAtlasManager->Hide();
}

/*virtual*/ void ItemProject::Draw(IHyApplication &hyApp)
{
    if(m_eState == DRAWSTATE_AtlasManager)
        m_pAtlasManager->Draw(hyApp);
}

void ItemProject::SetAtlasGroupDrawState(int iAtlasGrpId)
{
    m_eState = DRAWSTATE_AtlasManager;
}
