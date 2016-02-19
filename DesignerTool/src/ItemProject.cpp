#include "ItemProject.h"
#include "WidgetAtlasManager.h"

#include "WidgetRenderer.h"

ItemProject::ItemProject(const QString sPath, const QString sRelPathAssets, const QString sRelPathMetaData, const QString sRelPathSource) : Item(ITEM_Project, sPath),
                                                                                                                                            m_sRelativeAssetsLocation(sRelPathAssets),
                                                                                                                                            m_sRelativeMetaDataLocation(sRelPathMetaData),
                                                                                                                                            m_sRelativeSourceLocation(sRelPathSource),
                                                                                                                                            m_eState(DRAWSTATE_Nothing)
{
    m_pAtlasManager = new WidgetAtlasManager(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlasManager;
}

/*virtual*/ void ItemProject::OnDraw_Open(IHyApplication &hyApp)
{
    if(m_eState == DRAWSTATE_AtlasManager)
        m_pAtlasManager->OnDraw_Open(hyApp);
}

/*virtual*/ void ItemProject::OnDraw_Close(IHyApplication &hyApp)
{
    if(m_eState == DRAWSTATE_AtlasManager)
        m_pAtlasManager->OnDraw_Close(hyApp);
}

/*virtual*/ void ItemProject::OnDraw_Show(IHyApplication &hyApp)
{
    if(m_eState == DRAWSTATE_AtlasManager)
        m_pAtlasManager->OnDraw_Show(hyApp);
}

/*virtual*/ void ItemProject::OnDraw_Hide(IHyApplication &hyApp)
{
    if(m_eState == DRAWSTATE_AtlasManager)
        m_pAtlasManager->OnDraw_Hide(hyApp);
}

/*virtual*/ void ItemProject::OnDraw_Update(IHyApplication &hyApp)
{
    if(m_eState == DRAWSTATE_AtlasManager)
        m_pAtlasManager->OnDraw_Update(hyApp);
}

void ItemProject::SetAtlasGroupDrawState(int iAtlasGrpId)
{
    m_eState = DRAWSTATE_AtlasManager;
}
