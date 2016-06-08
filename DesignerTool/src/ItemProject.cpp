/**************************************************************************
 *	ItemProject.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "ItemProject.h"

#include "WidgetAtlasManager.h"
#include "WidgetTabsManager.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

ItemProject::ItemProject(const QString sNewProjectFilePath) : Item(ITEM_Project, sNewProjectFilePath),
                                                              m_eDrawState(PROJDRAWSTATE_Nothing),
                                                              m_ePrevDrawState(PROJDRAWSTATE_Nothing),
                                                              m_bHasError(false)
{
    for(int i = 0; i < NUMPROJDRAWSTATE; ++i)
        m_bDrawStateLoaded[i] = false;
    
    QFile projFile(sNewProjectFilePath);
    if(projFile.exists())
    {
        if(!projFile.open(QIODevice::ReadOnly))
        {
            HyGuiLog("ItemProject::ItemProject() could not open the project file: " % sNewProjectFilePath, LOGTYPE_Error);
            m_bHasError = true;
        }
    }
    else
    {
        HyGuiLog("ItemProject::ItemProject() could not find the project file: " % sNewProjectFilePath, LOGTYPE_Error);
        m_bHasError = true;
    }

    if(m_bHasError)
        return;

    QJsonDocument settingsDoc = QJsonDocument::fromJson(projFile.readAll());
    projFile.close();

    QJsonObject projPathsObj = settingsDoc.object();

    m_sRelativeAssetsLocation = projPathsObj["AssetsPath"].toString();
    m_sRelativeMetaDataLocation = projPathsObj["MetaDataPath"].toString();
    m_sRelativeSourceLocation = projPathsObj["SourcePath"].toString();

    m_pTabsManager = new WidgetTabsManager(this);
    m_pAtlasManager = new WidgetAtlasManager(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlasManager;
}

QString ItemProject::GetDirPath() const
{
    QFileInfo file(m_sPATH);
    return file.dir().absolutePath() + '/';
}

/*virtual*/ void ItemProject::OnDraw_Open(IHyApplication &hyApp)
{
    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawOpen(hyApp, *m_pAtlasManager);
}

/*virtual*/ void ItemProject::OnDraw_Close(IHyApplication &hyApp)
{
    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawClose(hyApp, *m_pAtlasManager);
}

/*virtual*/ void ItemProject::OnDraw_Show(IHyApplication &hyApp)
{
    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawShow(hyApp, *m_pAtlasManager);
}

/*virtual*/ void ItemProject::OnDraw_Hide(IHyApplication &hyApp)
{
    if(m_ePrevDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawHide(hyApp, *m_pAtlasManager);
}

/*virtual*/ void ItemProject::OnDraw_Update(IHyApplication &hyApp)
{
    while(m_DrawStateQueue.empty() == false)
    {
        m_ePrevDrawState = m_eDrawState;
        m_eDrawState = m_DrawStateQueue.dequeue();
        
        // This shouldn't happen, but if it did it would break logic below it
        if(m_eDrawState == PROJDRAWSTATE_Nothing && m_ePrevDrawState == PROJDRAWSTATE_Nothing)
        {
            HyGuiLog("ItemProject::OnDraw_Update() has both m_eDrawState and m_ePrevDrawState as 'PROJDRAWSTATE_Nothing'", LOGTYPE_Warning);
            break;
        }
        
        // If our current state is 'nothing', then Hide() what was previous if it was loaded
        if(m_eDrawState == PROJDRAWSTATE_Nothing)
        {
            if(m_bDrawStateLoaded[m_ePrevDrawState])
                DrawHide(hyApp);
            
            m_ePrevDrawState = PROJDRAWSTATE_Nothing;
        }
        else
        {
            if(m_bDrawStateLoaded[m_eDrawState] == false)
            {
                DrawOpen(hyApp);
                m_bDrawStateLoaded[m_eDrawState] = true;
            }

            DrawShow(hyApp);
        }
    }
    
    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawUpdate(hyApp, *m_pAtlasManager);
}

void ItemProject::SetOverrideDrawState(eProjDrawState eDrawState)
{
    m_DrawStateQueue.append(eDrawState);
}

bool ItemProject::IsOverrideDraw()
{
    return (m_eDrawState != PROJDRAWSTATE_Nothing || m_ePrevDrawState != PROJDRAWSTATE_Nothing || m_DrawStateQueue.empty() == false);
}

void ItemProject::Reset()
{
    for(int i = 0; i < NUMPROJDRAWSTATE; ++i)
        m_bDrawStateLoaded[i] = false;
}
