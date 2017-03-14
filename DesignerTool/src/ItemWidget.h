/**************************************************************************
 *	ItemWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ITEMWIDGET_H
#define ITEMWIDGET_H

#include "Item.h"

#include "Harmony/HyEngine.h"

#include <QUndoStack>
#include <QJsonObject>

class WidgetAtlasManager;
class WidgetAudioManager;
class ItemProject;
class HyGuiFrame;

class ItemWidget : public Item
{
    Q_OBJECT

    friend class MainWindow;
    friend class WidgetAtlasManager;
    friend class WidgetAtlasGroup;
    friend class ItemProject;
    friend class ItemAtlases;

protected:
    ItemProject *       m_pItemProj;
    QJsonValue          m_InitValue;

    HyEntity2d          m_HyEntity;
    QWidget *           m_pWidget;

    QUndoStack *        m_pUndoStack;
    QAction *           m_pActionUndo;
    QAction *           m_pActionRedo;

    QSet<HyGuiFrame *>  m_Links;

    HyCamera2d *        m_pCamera;
    bool                m_bReloadDraw;

    virtual void OnGiveMenuActions(QMenu *pMenu) = 0;

    virtual void OnGuiLoad(IHyApplication &hyApp) = 0;
    virtual void OnGuiUnload(IHyApplication &hyApp) = 0;
    virtual void OnGuiShow(IHyApplication &hyApp) = 0;
    virtual void OnGuiHide(IHyApplication &hyApp) = 0;
    virtual void OnGuiUpdate(IHyApplication &hyApp) = 0;

    virtual void OnLink(HyGuiFrame *pFrame) = 0;
    virtual void OnReLink(HyGuiFrame *pFrame) = 0;
    virtual void OnUnlink(HyGuiFrame *pFrame) = 0;
    
    virtual QJsonValue OnSave() = 0;

public:
    ItemWidget(ItemProject *pItemProj, eItemType eType, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~ItemWidget();
    
    ItemProject *GetItemProject();

    QJsonValue GetInitValue()                       { return m_InitValue; }

    bool IsLoaded() const                           { return (m_pCamera != NULL); }

    QWidget *GetWidget() const                      { return m_pWidget; }
    QUndoStack *GetUndoStack()                      { return m_pUndoStack; }

    
    void GiveMenuActions(QMenu *pMenu);
    void Save();
    bool IsSaveClean();
    void DiscardChanges();

private:
    void Load(IHyApplication &hyApp);
    void Unload(IHyApplication &hyApp);

    void DrawShow(IHyApplication &hyApp);
    void DrawHide(IHyApplication &hyApp);

    void DrawUpdate(IHyApplication &hyApp);

    void Link(HyGuiFrame *pFrame);
    void Relink(HyGuiFrame *pFrame);
    void Unlink(HyGuiFrame *pFrame);

private Q_SLOTS:
    void on_undoStack_cleanChanged(bool bClean);
    
};
Q_DECLARE_METATYPE(ItemWidget *)

#endif // ITEMWIDGET_H
