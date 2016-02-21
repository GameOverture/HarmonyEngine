#include "ItemProject.h"
#include "WidgetAtlasManager.h"

#include "WidgetRenderer.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

ItemProject::ItemProject(const QString sNewProjectFilePath) : Item(ITEM_Project, sNewProjectFilePath),
                                                              m_eState(DRAWSTATE_Nothing)
{
    QFile projFile(sNewProjectFilePath);
    if(projFile.exists())
    {
        if(!projFile.open(QIODevice::ReadOnly))
            HyGuiLog("ItemProject::ItemProject() could not open the project file: " % sNewProjectFilePath, LOGTYPE_Error);
    }
    else
        HyGuiLog("ItemProject::ItemProject() could not find the project file: " % sNewProjectFilePath, LOGTYPE_Error);

    QJsonDocument settingsDoc = QJsonDocument::fromJson(projFile.readAll());
    projFile.close();

    QJsonObject projPathsObj = settingsDoc.object();

    m_sRelativeAssetsLocation = projPathsObj["AssetsPath"].toString();
    m_sRelativeMetaDataLocation = projPathsObj["MetaDataPath"].toString();
    m_sRelativeSourceLocation = projPathsObj["SourcePath"].toString();

    m_pAtlasManager = new WidgetAtlasManager(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlasManager;
}

QString ItemProject::GetDirPath() const
{
    QFileInfo file(m_sPath);
    return file.dir().absolutePath() + '/';
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
