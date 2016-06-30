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

class WidgetAtlasManager : public QWidget
{
    Q_OBJECT

    ItemProject *                   m_pProjOwner;

    QDir                            m_MetaDir;
    QDir                            m_DataDir;

    QFile                           m_DependenciesFile;
    QMap<quint32, HyGuiFrame *>     m_DependencyMap;
    
    QTreeWidgetItem *               m_pMouseHoverItem;

public:
    explicit WidgetAtlasManager(QWidget *parent = 0);
    explicit WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlasManager();

    HyGuiFrame *CreateFrame(quint32 uiCRC, QString sN, QRect rAlphaCrop, uint uiAtlasGroupId, int iW, int iH, int iTexIndex, bool bRot, int iX, int iY);
    void RemoveFrame(HyGuiFrame *pFrame);

    void SaveData();

    void SaveDependencies();
    void LoadDependencies();
    void SetDependency(HyGuiFrame *pFrame, ItemWidget *pItem, QVariant param);
    void RemoveDependency(HyGuiFrame *pFrame, ItemWidget *pItem);

    QList<QPair<HyGuiFrame *, QVariant> > RequestFrames(ItemWidget *pItem, QList<QPair<HyGuiFrame *, QVariant> > optionalRequestList = QList<QPair<HyGuiFrame *, QVariant> >());
    void RelinquishFrames(ItemWidget *pItem, QList<QPair<HyGuiFrame *, QVariant> > relinquishList);

    void PreviewAtlasGroup();
    void HideAtlasGroup();

    friend void AtlasManager_DrawOpen(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawClose(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawShow(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawHide(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);
    friend void AtlasManager_DrawUpdate(IHyApplication &hyApp, WidgetAtlasManager &atlasMan);

private slots:
    void on_atlasGroups_currentChanged(int iIndex);

    void on_btnAddGroup_clicked();

private:
    Ui::WidgetAtlasManager *ui;

    void AddAtlasGroup(int iId = -1);
};

#endif // WIDGETATLASMANAGER_H
