/**************************************************************************
 *	ItemWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IDATA_H
#define IDATA_H

#include "ExplorerItem.h"
#include "Harmony/HyEngine.h"
#include "IDraw.h"

#include <QUndoStack>
#include <QJsonObject>

class AtlasesWidget;
class AudioWidgetManager;
class Project;
class AtlasFrame;

class IData : public ExplorerItem
{
    Q_OBJECT

    friend class MainWindow;
    friend class AtlasesWidget;
    friend class WidgetAtlasGroup;
    friend class Project;
    friend class AtlasesData;

protected:
    Project *       m_pItemProj;
    QJsonValue          m_InitValue;

    IDraw *             m_pDraw;
    QWidget *           m_pWidget;

    QUndoStack *        m_pUndoStack;
    QAction *           m_pActionUndo;
    QAction *           m_pActionRedo;

    QSet<AtlasFrame *>  m_Links;

    HyCamera2d *        m_pCamera;
    bool                m_bReloadDraw;

    virtual void OnGiveMenuActions(QMenu *pMenu) = 0;

    virtual void OnGuiLoad(IHyApplication &hyApp) = 0;
    virtual void OnGuiUnload(IHyApplication &hyApp) = 0;
    virtual void OnGuiShow(IHyApplication &hyApp) = 0;
    virtual void OnGuiHide(IHyApplication &hyApp) = 0;
    virtual void OnGuiUpdate(IHyApplication &hyApp) = 0;

    virtual void OnLink(AtlasFrame *pFrame) = 0;
    virtual void OnReLink(AtlasFrame *pFrame) = 0;
    virtual void OnUnlink(AtlasFrame *pFrame) = 0;
    
    virtual QJsonValue OnSave() = 0;

public:
    IData(Project *pItemProj, eItemType eType, const QString sPrefix, const QString sName, QJsonValue initVal);
    virtual ~IData();
    
    Project *GetItemProject();

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

    void Link(AtlasFrame *pFrame);
    void Relink(AtlasFrame *pFrame);
    void Unlink(AtlasFrame *pFrame);

private Q_SLOTS:
    void on_undoStack_cleanChanged(bool bClean);
    
};
Q_DECLARE_METATYPE(IData *)

#endif // IDATA_H
