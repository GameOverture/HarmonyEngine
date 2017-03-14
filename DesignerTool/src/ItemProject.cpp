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
#include "WidgetAudioManager.h"
#include "MainWindow.h"
#include "ItemAudio.h"
#include "ItemSprite.h"
#include "ItemFont.h"
#include "HyGlobal.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>

// Keep this commented out unless you want the entire project to save every item upon boot (used if 'Data.json' layout has changed and needs to propagate all its changes)
//#define RESAVE_ENTIRE_PROJECT

const char * const szCHECKERGRID_VERTEXSHADER = R"src(
#version 400

/*layout(location = 0)*/ in vec2 position;
/*layout(location = 1)*/ in vec2 UVcoord;

smooth out vec2 interpUV;

uniform mat4 transformMtx;
uniform mat4 u_mtxWorldToCamera;
uniform mat4 u_mtxCameraToClip;

void main()
{
    interpUV.x = UVcoord.x;
    interpUV.y = UVcoord.y;

    vec4 temp = transformMtx * vec4(position, 0, 1);
    temp = u_mtxWorldToCamera * temp;
    gl_Position = u_mtxCameraToClip * temp;
}
)src";


const char *const szCHECKERGRID_FRAGMENTSHADER = R"src(
#version 400

in vec2 interpUV;
out vec4 FragColor;

uniform float uGridSize;
uniform vec2 uResolution;
uniform vec4 gridColor1;
uniform vec4 gridColor2;

void main()
{
    vec2 screenCoords = (interpUV.xy * (uResolution /** 0.5f*/)) / uGridSize;
    FragColor = mix(gridColor1, gridColor2, step((float(int(floor(screenCoords.x) + floor(screenCoords.y)) & 1)), 0.9));
}
)src";

CheckerGrid::CheckerGrid()
{
}

CheckerGrid::~CheckerGrid()
{
}

void CheckerGrid::SetSurfaceSize(int iWidth, int iHeight)
{
    m_Resolution.x = iWidth;
    m_Resolution.y = iHeight;
    SetAsQuad(m_Resolution.x, m_Resolution.y, false);
    pos.Set(m_Resolution.x * -0.5f, m_Resolution.y * -0.5f);
}

/*virtual*/ void CheckerGrid::OnUpdateUniforms()
{
    glm::mat4 mtx;
    GetWorldTransform(mtx);

    m_ShaderUniforms.Set("transformMtx", mtx);
    m_ShaderUniforms.Set("uGridSize", 25.0f);
    m_ShaderUniforms.Set("uResolution", m_Resolution);
    m_ShaderUniforms.Set("gridColor1", glm::vec4(106.0f / 255.0f, 105.0f / 255.0f, 113.0f / 255.0f, 1.0f));
    m_ShaderUniforms.Set("gridColor2", glm::vec4(93.0f / 255.0f, 93.0f / 255.0f, 97.0f / 255.0f, 1.0f));
}

/*virtual*/ void CheckerGrid::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
    assert(m_RenderState.GetNumVerticesPerInstance() == 4, "CheckerGrid::OnWriteDrawBufferData is trying to draw a primitive that's not a quad");

    for(int i = 0; i < 4; ++i)
    {
        *reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = m_pDrawBuffer[i];
        pRefDataWritePos += sizeof(glm::vec2);

        glm::vec2 vUV;
        switch(i)
        {
        case 0:
            vUV.x = 1.0f;
            vUV.y = 0.0f;
            break;

        case 1:
            vUV.x = 0.0f;
            vUV.y = 0.0f;
            break;

        case 2:
            vUV.x = 1.0f;
            vUV.y = 1.0f;
            break;

        case 3:
            vUV.x = 0.0f;
            vUV.y = 1.0f;
            break;
        }

        *reinterpret_cast<glm::vec2 *>(pRefDataWritePos) = vUV;
        pRefDataWritePos += sizeof(glm::vec2);
    }
}

ItemProject::ItemProject(const QString sNewProjectFilePath) :   Item(ITEM_Project, sNewProjectFilePath),
                                                                IHyApplication(HarmonyInit()),
                                                                m_pAtlasesData(nullptr),
                                                                m_pAtlasMan(nullptr),
                                                                m_pAudioMan(nullptr),
                                                                m_pTabBar(nullptr),
                                                                m_eDrawState(PROJDRAWSTATE_Nothing),
                                                                m_ePrevDrawState(PROJDRAWSTATE_Nothing),
                                                                m_pCamera(nullptr),
                                                                m_ActionSave(0),
                                                                m_ActionSaveAll(0),
                                                                m_bHasError(false)
{
    for(int i = 0; i < NUMPROJDRAWSTATE; ++i)
        m_bDrawStateLoaded[i] = false;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    QFile projFile(sNewProjectFilePath);
    if(projFile.exists())
    {
        if(!projFile.open(QIODevice::ReadOnly))
        {
            HyGuiLog("ItemProject::ItemProject() could not open " % sNewProjectFilePath % ": " % projFile.errorString(), LOGTYPE_Error);
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

    m_sGameName                 = projPathsObj["GameName"].toString();
    m_pTreeItemPtr->setText(0, m_sGameName);

    m_sRelativeAssetsLocation   = projPathsObj["DataPath"].toString();
    m_sRelativeMetaDataLocation = projPathsObj["MetaDataPath"].toString();
    m_sRelativeSourceLocation   = projPathsObj["SourcePath"].toString();


    sm_Init.sGameName = GetName(false).toStdString();
    sm_Init.sDataDir = GetAssetsAbsPath().toStdString();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    m_ActionSave.setText("&Save");
    m_ActionSave.setIcon(QIcon(":/icons16x16/file-save.png"));
    m_ActionSave.setShortcuts(QKeySequence::Save);
    m_ActionSave.setShortcutContext(Qt::ApplicationShortcut);
    m_ActionSave.setEnabled(false);
    connect(&m_ActionSave, SIGNAL(triggered(bool)), this, SLOT(on_save_triggered()));

    m_ActionSaveAll.setText("Save &All");
    m_ActionSaveAll.setIcon(QIcon(":/icons16x16/file-saveAll.png"));
    m_ActionSaveAll.setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    m_ActionSaveAll.setShortcutContext(Qt::ApplicationShortcut);
    m_ActionSaveAll.setEnabled(false);
    connect(&m_ActionSaveAll, SIGNAL(triggered(bool)), this, SLOT(on_saveAll_triggered()));
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Load game data items
    QFile dataFile(GetAssetsAbsPath() % HYGUIPATH_DataFile);
    if(dataFile.exists())
    {
        if(!dataFile.open(QIODevice::ReadOnly))
        {
            HyGuiLog("ItemProject::ItemProject() could not open " % sNewProjectFilePath % "'s " % HYGUIPATH_DataFile % " file for project: " % dataFile.errorString(), LOGTYPE_Error);
            m_bHasError = true;
            return;
        }

        QJsonDocument userDoc = QJsonDocument::fromJson(dataFile.readAll());
        dataFile.close();

        m_SaveDataObj = userDoc.object();
    }
    else
    {
        // Initialize the project by processing each type of sub dir
        QList<eItemType> subDirList = HyGlobal::SubDirList();
        for(int i = 0; i < subDirList.size(); ++i)
        {
            if(subDirList[i] == ITEM_DirAtlases || subDirList[i] == ITEM_DirAudioBanks)
                continue;

            QString sSubDirName = HyGlobal::ItemName(subDirList[i]);
            m_SaveDataObj.insert(sSubDirName, QJsonObject());
        }

        SaveGameData();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Initialize the project by processing each type of sub dir
    QList<eItemType> subDirList = HyGlobal::SubDirList();
    for(int i = 0; i < subDirList.size(); ++i)
    {
        if(subDirList[i] == ITEM_DirAtlases || subDirList[i] == ITEM_DirAudioBanks)
            continue;

        QString sSubDirPath = GetAssetsAbsPath() % HyGlobal::ItemName(subDirList[i]) % HyGlobal::ItemExt(subDirList[i]);
        Item *pSubDirItem = new Item(subDirList[i], sSubDirPath);

        // Adding sub dir tree item
        QTreeWidgetItem *pSubDirTreeItem = pSubDirItem->GetTreeItem();
        m_pTreeItemPtr->addChild(pSubDirTreeItem);

        QString sSubDirName = HyGlobal::ItemName(subDirList[i]);
        if(m_SaveDataObj.contains(sSubDirName) == false)
            m_SaveDataObj.insert(sSubDirName, QJsonObject());

        // Get the corresponding sub dir QJsonObject, and iterate through the objects (data items) within
        QJsonObject subDirObj = m_SaveDataObj[sSubDirName].toObject();
        QJsonObject::iterator objsInSubDirIter = subDirObj.begin();

        ///////////////////////////////////////
        /// ITEMS IN SUBDIR
        for(; objsInSubDirIter != subDirObj.end(); ++objsInSubDirIter)
        {
            QString sItemPath = objsInSubDirIter.key();

            // Create prefix folder tree items if they don't exist, and finally adding the tree item for the data itself
            QStringList sPathPartList = sItemPath.split("/");
            QString sCurPrefix = "";

            QTreeWidgetItem *pCurPrefixTreeItem = pSubDirTreeItem;
            ///////////////////////////////////////
            /// PATH PARTS
            for(int iPathPartIndex = 0; iPathPartIndex < sPathPartList.size(); ++iPathPartIndex)
            {
                if(iPathPartIndex != 0)
                    sCurPrefix += "/";

                // Not the last path part, must be a prefix
                if(iPathPartIndex != sPathPartList.size() - 1)
                {
                    sCurPrefix += sPathPartList[iPathPartIndex];

                    bool bPrefixFound = false;
                    for(int iChildIndex = 0; iChildIndex < pCurPrefixTreeItem->childCount(); ++iChildIndex)
                    {
                        if(sPathPartList[iPathPartIndex] == pCurPrefixTreeItem->child(iChildIndex)->text(0))
                        {
                            pCurPrefixTreeItem = pCurPrefixTreeItem->child(iChildIndex);
                            bPrefixFound = true;
                            break;
                        }
                    }

                    if(bPrefixFound == false)
                    {
                        Item *pPrefixItem = new Item(ITEM_Prefix, sCurPrefix);
                        QTreeWidgetItem *pNewPrefixTreeWidget = pPrefixItem->GetTreeItem();

                        pCurPrefixTreeItem->addChild(pNewPrefixTreeWidget);
                        pCurPrefixTreeItem = pNewPrefixTreeWidget;
                    }
                }
                else // Last path part, so must be the actual data item
                {
                    Item *pNewDataItem = nullptr;
                    switch(subDirList[i])
                    {
                    case ITEM_DirAudio:
                        pNewDataItem = new ItemAudio(this, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value());
                        break;
                    case ITEM_DirFonts:
                        pNewDataItem = new ItemFont(this, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value());
                        break;
                    case ITEM_DirSprites:
                        pNewDataItem = new ItemSprite(this, sCurPrefix, sPathPartList[iPathPartIndex], objsInSubDirIter.value());
                        break;
                    case ITEM_DirParticles:
                    case ITEM_DirSpine:
                    case ITEM_DirShaders:
                    case ITEM_DirEntities:
                    case ITEM_DirAtlases:
                    default:
                        { HyGuiLog("Unknown item type in ItemProject!", LOGTYPE_Error); }
                    }

                    pCurPrefixTreeItem->addChild(pNewDataItem->GetTreeItem());

#ifdef RESAVE_ENTIRE_PROJECT
                    static_cast<ItemWidget *>(pNewDataItem)->Save();
#endif

                }
            }
        }
    }

#ifdef RESAVE_ENTIRE_PROJECT
    SaveGameData();
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Load user settings from meta data
    QFile userFile(GetMetaDataAbsPath() % HYGUIPATH_MetaUserFile);
    if(userFile.exists())
    {
        if(!userFile.open(QIODevice::ReadOnly))
        {
            HyGuiLog("ItemProject::ItemProject() could not open " % sNewProjectFilePath % "'s " % HYGUIPATH_MetaUserFile % " file for project: " % userFile.errorString(), LOGTYPE_Error);
        }

        QJsonDocument userDoc = QJsonDocument::fromJson(userFile.readAll());
        userFile.close();

        QJsonObject userObj = userDoc.object();

        //m_pAtlasMan->SetSelectedAtlasGroup(userObj["DefaultAtlasGroup"].toString());

        // TODO:

        //QStringList sListOpenItems = m_Settings.value("openItems").toStringList();
        //sListOpenItems.sort();  // This sort should organize each open item by project to reduce unloading/loading projects
        //foreach(QString sItemPath, sListOpenItems)
        //{
        //    Item *pItem = ui->explorer->GetItemByPath(sItemPath);
        //    if(pItem)
        //        OpenItem(pItem);
        //}
    }

    m_pAtlasesData = new ItemAtlases(this);
}

ItemProject::~ItemProject()
{
    delete m_pAtlasMan;
}

void ItemProject::LoadWidgets()
{
    m_pAtlasMan = new WidgetAtlasManager(*m_pAtlasesData, nullptr);
    m_pAudioMan = new WidgetAudioManager(this, nullptr);

    m_pTabBar = new QTabBar(nullptr);
    m_pTabBar->setTabsClosable(true);
    m_pTabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    connect(m_pTabBar, SIGNAL(currentChanged(int)), this, SLOT(on_tabBar_currentChanged(int)));
    connect(m_pTabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(on_tabBar_closeRequested(int)));
}

bool ItemProject::HasError() const
{
    return m_bHasError;
}

QList<AtlasTreeItem *> ItemProject::GetAtlasTreeItemList()
{
    return m_pAtlasesData->GetAtlasTreeItemList();
}

QString ItemProject::GetDirPath() const
{
    QFileInfo file(m_sPATH);
    return file.dir().absolutePath() + '/';
}

QList<QAction *> ItemProject::GetSaveActions()
{
    QList<QAction *> actionList;
    actionList.append(&m_ActionSave);
    actionList.append(&m_ActionSaveAll);
    
    return actionList;
}

void ItemProject::SetSaveEnabled(bool bSaveEnabled, bool bSaveAllEnabled)
{
    m_ActionSave.setEnabled(bSaveEnabled);
    m_ActionSaveAll.setEnabled(bSaveAllEnabled);
}

// IHyApplication override
/*virtual*/ bool ItemProject::Initialize()
{
    IHyShader *pShader_CheckerGrid = IHyRenderer::MakeCustomShader();
    pShader_CheckerGrid->SetSourceCode(szCHECKERGRID_VERTEXSHADER, HYSHADER_Vertex);
    pShader_CheckerGrid->SetVertexAttribute("position", HYSHADERVAR_vec2);
    pShader_CheckerGrid->SetVertexAttribute("UVcoord", HYSHADERVAR_vec2);
    pShader_CheckerGrid->SetSourceCode(szCHECKERGRID_FRAGMENTSHADER, HYSHADER_Fragment);
    pShader_CheckerGrid->Finalize(HYSHADERPROG_Primitive);

    m_CheckerGridBG.SetCustomShader(pShader_CheckerGrid);
    m_CheckerGridBG.SetDisplayOrder(-1000);
    m_CheckerGridBG.SetSurfaceSize(10000, 10000);  // Use a large size that is a multiple of grid size (25)
    m_CheckerGridBG.Load();

    m_pCamera = Window().CreateCamera2d();
    m_pCamera->SetEnabled(true);

    return true;
}

// IHyApplication override
/*virtual*/ bool ItemProject::Update()
{
    if(IsOverrideDraw())
        OverrideDraw();
    else if(m_pTabBar->count() > 0)
    {
        m_pCamera->SetEnabled(false);
        m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ItemWidget *>()->DrawUpdate(*this);
    }
    else
        m_pCamera->SetEnabled(true);

    return true;
}

// IHyApplication override
/*virtual*/ void ItemProject::Shutdown()
{
}

void ItemProject::SetRenderSize(int iWidth, int iHeight)
{
    Window().SetResolution(glm::ivec2(iWidth, iHeight));
}

void ItemProject::SetOverrideDrawState(eProjDrawState eDrawState)
{
    m_DrawStateQueue.append(eDrawState);
}

bool ItemProject::IsOverrideDraw()
{
    return (m_eDrawState != PROJDRAWSTATE_Nothing || m_ePrevDrawState != PROJDRAWSTATE_Nothing || m_DrawStateQueue.empty() == false);
}

void ItemProject::OverrideDraw()
{
    m_pCamera->SetEnabled(true);

    while(m_DrawStateQueue.empty() == false)
    {
        m_ePrevDrawState = m_eDrawState;
        m_eDrawState = m_DrawStateQueue.dequeue();

        // This shouldn't happen, but if it did it would break logic below it
        if(m_eDrawState == PROJDRAWSTATE_Nothing && m_ePrevDrawState == PROJDRAWSTATE_Nothing)
            continue;

        // If our current state is 'nothing', then Hide() what was previous if it was loaded
        if(m_eDrawState == PROJDRAWSTATE_Nothing)
        {
            if(m_bDrawStateLoaded[m_ePrevDrawState])
            {
                if(m_ePrevDrawState == PROJDRAWSTATE_AtlasManager)
                    AtlasManager_DrawHide(*this, *m_pAtlasMan);
            }

            // Show the selected tab since we're done with override draw
            if(m_pTabBar->currentIndex() != -1)
                m_pTabBar->tabData(m_pTabBar->currentIndex()).value<ItemWidget *>()->DrawShow(*this);
            
            m_ePrevDrawState = PROJDRAWSTATE_Nothing;
        }
        else
        {
            if(m_bDrawStateLoaded[m_eDrawState] == false)
            {
                if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
                    AtlasManager_DrawOpen(*this, *m_pAtlasMan);

                m_bDrawStateLoaded[m_eDrawState] = true;
            }
            
            // Hide any currently shown items, since we're being draw override
            for(int i = 0; i < m_pTabBar->count(); ++i)
                m_pTabBar->tabData(i).value<ItemWidget *>()->DrawHide(*this);

            if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
                AtlasManager_DrawShow(*this, *m_pAtlasMan);
        }
    }

    if(m_eDrawState == PROJDRAWSTATE_AtlasManager)
        AtlasManager_DrawUpdate(*this, *m_pAtlasMan);
}

void ItemProject::Reset()
{
    for(int i = 0; i < NUMPROJDRAWSTATE; ++i)
        m_bDrawStateLoaded[i] = false;

    sm_Init.sGameName = GetName(false).toStdString();
    sm_Init.sDataDir = GetAssetsAbsPath().toStdString();
}

void ItemProject::SaveGameData(eItemType eType, QString sPath, QJsonValue itemVal)
{
    eItemType eSubDirType = HyGlobal::GetCorrespondingDirItem(eType);
    QString sSubDirName = HyGlobal::ItemName(eSubDirType);

    if(m_SaveDataObj.contains(sSubDirName) == false)
    {
        HyGuiLog("Could not find subdir: " % sSubDirName % " within ItemProject::SaveGameData", LOGTYPE_Error);
    }

    QJsonObject subDirObj = m_SaveDataObj[sSubDirName].toObject();

    subDirObj.remove(sPath);
    subDirObj.insert(sPath, itemVal);

    m_SaveDataObj.remove(sSubDirName);
    m_SaveDataObj.insert(sSubDirName, subDirObj);

#ifndef RESAVE_ENTIRE_PROJECT
    SaveGameData();
#endif
}

void ItemProject::SaveGameData()
{
    QFile dataFile(GetAssetsAbsPath() % HYGUIPATH_DataFile);
    if(dataFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog(QString("Couldn't open ") % HYGUIPATH_DataFile % " for writing: " % dataFile.errorString(), LOGTYPE_Error);
    }
    else
    {
        QJsonDocument userDoc;
        userDoc.setObject(m_SaveDataObj);
        qint64 iBytesWritten = dataFile.write(userDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog(QString("Could not write to ") % HYGUIPATH_DataFile % " file: " % dataFile.errorString(), LOGTYPE_Error);
        }

        dataFile.close();
    }
}

void ItemProject::SaveUserData()
{
    QFile userFile(GetMetaDataAbsPath() % HYGUIPATH_MetaUserFile);
    if(userFile.open(QIODevice::WriteOnly | QIODevice::Truncate) == false)
    {
       HyGuiLog(QString("Couldn't open ") % HYGUIPATH_MetaUserFile % " for writing: " % userFile.errorString(), LOGTYPE_Error);
    }
    else
    {
        QJsonObject userObj;
        userObj.insert("DefaultAtlasGroup", "Not used anymore");


        // TODO:

        //QStringList sListOpenItems = m_Settings.value("openItems").toStringList();
        //sListOpenItems.sort();  // This sort should organize each open item by project to reduce unloading/loading projects
        //foreach(QString sItemPath, sListOpenItems)
        //{
        //    Item *pItem = ui->explorer->GetItemByPath(sItemPath);
        //    if(pItem)
        //        OpenItem(pItem);
        //}





        QJsonDocument userDoc;
        userDoc.setObject(userObj);
        qint64 iBytesWritten = userFile.write(userDoc.toJson());
        if(0 == iBytesWritten || -1 == iBytesWritten)
        {
            HyGuiLog(QString("Could not write to ") % HYGUIPATH_MetaUserFile % " file: " % userFile.errorString(), LOGTYPE_Error);
        }

        userFile.close();
    }
}

QJsonObject ItemProject::GetSubDirObj(eItemType eType)
{
    return m_SaveDataObj[HyGlobal::ItemName(HyGlobal::GetCorrespondingDirItem(eType))].toObject();
}

void ItemProject::on_tabBar_currentChanged(int index)
{
    if(index < 0)
        return;

    int iIndex = m_pTabBar->currentIndex();
    QVariant v = m_pTabBar->tabData(iIndex);
    ItemWidget *pItem = v.value<ItemWidget *>();

    MainWindow::OpenItem(pItem);
}

void ItemProject::on_save_triggered()
{
    int iIndex = m_pTabBar->currentIndex();
    QVariant v = m_pTabBar->tabData(iIndex);
    ItemWidget *pItem = v.value<ItemWidget *>();
    pItem->Save();
    
    HyGuiLog(pItem->GetName(true) % " was saved", LOGTYPE_Normal);
}

void ItemProject::on_saveAll_triggered()
{
    for(int i = 0; i < m_pTabBar->count(); ++i)
    {
        // TODO: instead look for dirty?
        if(m_pTabBar->tabText(i).contains('*', Qt::CaseInsensitive))
        {
            ItemWidget *pItem = m_pTabBar->tabData(i).value<ItemWidget *>();
            pItem->Save();
            
            HyGuiLog(pItem->GetName(true) % " was saved", LOGTYPE_Normal);
        }
    }
}

void ItemProject::on_tabBar_closeRequested(int iIndex)
{
    ItemWidget *pItem = m_pTabBar->tabData(iIndex).value<ItemWidget *>();
    MainWindow::CloseItem(pItem);
}
