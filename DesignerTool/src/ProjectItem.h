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

#include "ExplorerItem.h"
#include "Harmony/HyEngine.h"
#include "IDraw.h"

#include <QUndoStack>
#include <QJsonObject>

class AtlasWidget;
class AudioWidgetManager;
class Project;
class AtlasFrame;
class IModel;

class ProjectItem : public ExplorerItem
{
    Q_OBJECT

    friend class Project;

    Project &               m_ProjectRef;
    QJsonValue              m_SaveValue;

    // Loaded in constructor
    IModel *                m_pModel;
    QUndoStack *            m_pUndoStack;
    QAction *               m_pActionUndo;
    QAction *               m_pActionRedo;

    // Loaded when item is opened
    QWidget *               m_pWidget;
    
public:
    ProjectItem(Project &projRef, eItemType eType, const QString sPrefix, const QString sName, QJsonValue initValue);
    virtual ~ProjectItem();

    void LoadModel();
    
    Project &GetProject();

    IModel *GetModel()                              { return m_pModel; }
    QWidget *GetWidget() const                      { return m_pWidget; }
    QUndoStack *GetUndoStack()                      { return m_pUndoStack; }
    
    void GiveMenuActions(QMenu *pMenu);
    void Save();
    bool IsSaveClean();
    void DiscardChanges();

    void WidgetRefreshData(QVariant param);
    void WidgetRefreshDraw(IHyApplication &hyApp);
    
private:
    void WidgetLoad(IHyApplication &hyApp);
    void WidgetUnload(IHyApplication &hyApp);
    void WidgetShow(IHyApplication &hyApp);
    void WidgetHide(IHyApplication &hyApp);
    void WidgetUpdate(IHyApplication &hyApp);

private Q_SLOTS:
    void on_undoStack_cleanChanged(bool bClean);
    
};
Q_DECLARE_METATYPE(ProjectItem *)

#endif // PROJECTITEM_H
