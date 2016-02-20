#include "ItemProject.h"
#include "WidgetAtlasManager.h"

#include "WidgetRenderer.h"

#include <QFileInfo>

ItemProject::ItemProject(const QString sPath, const QString sRelPathAssets, const QString sRelPathMetaData, const QString sRelPathSource) : Item(ITEM_Project, sPath),
                                                                                                                                            m_sRelativeAssetsLocation(QDir::cleanPath(sRelPathAssets)),
                                                                                                                                            m_sRelativeMetaDataLocation(QDir::cleanPath(sRelPathMetaData)),
                                                                                                                                            m_sRelativeSourceLocation(QDir::cleanPath(sRelPathSource)),
                                                                                                                                            m_eState(DRAWSTATE_Nothing)
{
    m_pAtlasManager = new WidgetAtlasManager(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlasManager;
}

QString ItemProject::GetDirPath() const
{
    QFileInfo file(m_sPath);
    return file.dir().absolutePath();
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
