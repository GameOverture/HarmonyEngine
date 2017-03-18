/**************************************************************************
 *	ItemWidget.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IPROJDATA_H
#define IPROJDATA_H

#include "ExplorerItem.h"
#include "Harmony/HyEngine.h"
#include "IDraw.h"

#include <QUndoStack>
#include <QJsonObject>

class AtlasesWidget;
class AudioWidgetManager;
class Project;
class AtlasFrame;

class IProjItem : public ExplorerItem
{
    Q_OBJECT

    friend class MainWindow;
    friend class AtlasesWidget;
    friend class WidgetAtlasGroup;
    friend class Project;
    friend class AtlasesData;

protected:
    Project *           m_pItemProj;

    IDraw *             m_pDraw;
    QWidget *           m_pWidget;

    QUndoStack *        m_pUndoStack;
    QAction *           m_pActionUndo;
    QAction *           m_pActionRedo;

    QSet<AtlasFrame *>  m_Links;

    virtual void OnGiveMenuActions(QMenu *pMenu) = 0;

    virtual void OnLink(AtlasFrame *pFrame) = 0;
    virtual void OnUnlink(AtlasFrame *pFrame) = 0;
    
    virtual QJsonValue OnSave() = 0;

public:
    IProjItem(Project *pItemProj, eItemType eType, const QString sPrefix, const QString sName);
    virtual ~IProjItem();
    
    Project *GetProject();

    QWidget *GetWidget() const                      { return m_pWidget; }
    QUndoStack *GetUndoStack()                      { return m_pUndoStack; }

    
    void GiveMenuActions(QMenu *pMenu);
    void Save();
    bool IsSaveClean();
    void DiscardChanges();

private:
    void ProjLoad(IHyApplication &hyApp);
    void ProjUnload(IHyApplication &hyApp);
    void ProjShow(IHyApplication &hyApp);
    void ProjHide(IHyApplication &hyApp);
    void ProjUpdate(IHyApplication &hyApp);

    void Link(AtlasFrame *pFrame);
    void Relink(AtlasFrame *pFrame);
    void Unlink(AtlasFrame *pFrame);

private Q_SLOTS:
    void on_undoStack_cleanChanged(bool bClean);
    
};
Q_DECLARE_METATYPE(IProjItem *)

#endif // IPROJDATA_H
