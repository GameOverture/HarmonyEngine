/**************************************************************************
 *	ProjectItem.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PROJECTITEM_H
#define PROJECTITEM_H

#include "DataExplorerItem.h"
#include "Harmony/HyEngine.h"
#include "IDraw.h"

#include <QUndoStack>
#include <QJsonObject>

class AtlasWidget;
class AudioWidgetManager;
class Project;
class AtlasFrame;
class IModel;

class ProjectItem : public DataExplorerItem
{
    Q_OBJECT

    friend class Project;

    Project &               m_ProjectRef;
    QJsonValue              m_SaveValue;
    bool                    m_bExistencePendingSave;

    // Loaded in constructor
    IModel *                m_pModel;
    QUndoStack *            m_pUndoStack;
    QAction *               m_pActionUndo;
    QAction *               m_pActionRedo;

    // Loaded when item is opened
    QWidget *               m_pWidget;
    IDraw *                 m_pDraw;
    
public:
    ProjectItem(Project &projRef, HyGuiItemType eType, const QString sPrefix, const QString sName, QJsonValue initValue, bool bIsPendingSave);
    virtual ~ProjectItem();

    void LoadModel();
    
    Project &GetProject();

    IModel *GetModel()                              { return m_pModel; }
    QWidget *GetWidget()                            { return m_pWidget; }
    IDraw *GetDraw()                                { return m_pDraw; }
    QUndoStack *GetUndoStack()                      { return m_pUndoStack; }
    
    void GiveMenuActions(QMenu *pMenu);
    void Save();
    bool IsExistencePendingSave();
    bool IsSaveClean();
    void DiscardChanges();
    
    void BlockAllWidgetSignals(bool bBlock);

    void FocusWidgetState(int iStateIndex);
    
    void DeleteFromProject();
    
private:
    void WidgetLoad();
    void WidgetUnload();
    
    void DrawLoad(IHyApplication &hyApp);
    void DrawUnload();
    void DrawShow();
    void DrawHide();

private Q_SLOTS:
    void on_undoStack_cleanChanged(bool bClean);
    void on_undoStack_indexChanged(int iIndex);
    
};
Q_DECLARE_METATYPE(ProjectItem *)

#endif // PROJECTITEM_H
