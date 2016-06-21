/**************************************************************************
 *	WidgetAtlasManager.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETATLASMANAGER_H
#define WIDGETATLASMANAGER_H

#include <QWidget>
#include <QDir>
#include <QMouseEvent>

#include "ItemProject.h"
#include "WidgetAtlasGroup.h"

namespace Ui {
class WidgetAtlasManager;
}

class HyGuiFrameActionInfo
{
    Item *                          m_pAssociatedItem;
    QList<quint32>                  m_RequestFrameList;         // Holds 32bit hashes
    
    // Below is filled out by WidgetAtlasManager
    QList<QPair<quint32, bool> >    m_ReturnedFrameList;        // Holds 32bit hashes
    
public:
    HyGuiFrameActionInfo(Item *pItem) : m_pAssociatedItem(pItem)
    { }
    ~HyGuiFrameActionInfo()
    { }
    
    QList<quint32> &RequestedFrames()                       { return m_RequestFrameList; }
    QList<QPair<quint32, bool> > &ReturnedFrames()          { return m_ReturnedFrameList; }
};
Q_DECLARE_METATYPE(HyGuiFrameActionInfo)

class WidgetAtlasManager : public QWidget
{
    Q_OBJECT

    ItemProject *                   m_pProjOwner;

    QDir                            m_MetaDir;
    QDir                            m_DataDir;
    
    QTreeWidgetItem *               m_pMouseHoverItem;

    // Dependency maps. Both maps hold the same info, each provide access from either frame or item
    QMap<quint32, QStringList>      m_FramesToItemMap;
    QMap<QString, QList<quint32> >  m_ItemToFramesMap;

    QActionGroup *                  m_pFrameRequestActionGroup;
    QActionGroup *                  m_pFrameRelinquishActionGroup;

public:
    explicit WidgetAtlasManager(QWidget *parent = 0);
    explicit WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlasManager();

    void SaveData();

    void PreviewAtlasGroup();
    void HideAtlasGroup();

    friend void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawClose(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawShow(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawHide(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawUpdate(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);

    void Reload();
    
    QAction *CreateRequestFramesAction(Item *pRequester);
    QAction *CreateRelinquishFramesAction(Item *pRequester);

    void SetFramesAvailableForImport();

private slots:
    void on_atlasGroups_currentChanged(int iIndex);

    void on_btnAddGroup_clicked();

    void on_actionRequestFrames_triggered();

    void on_actionRelinqishFrames_triggered();

private:
    Ui::WidgetAtlasManager *ui;

    void AddAtlasGroup(int iId = -1);
};

#endif // WIDGETATLASMANAGER_H
