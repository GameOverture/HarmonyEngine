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

class WidgetAtlasManager;

class ItemWidget : public Item
{
    Q_OBJECT

    friend class WidgetAtlasManager;
    friend class ItemProject;

    void DrawLoad(IHyApplication &hyApp);
    void DrawUnload(IHyApplication &hyApp);

    void DrawShow(IHyApplication &hyApp);
    void DrawHide(IHyApplication &hyApp);

    void DrawUpdate(IHyApplication &hyApp);

    void Link(HyGuiFrame *pFrame);
    void Unlink(HyGuiFrame *pFrame);

protected:
    WidgetAtlasManager &m_AtlasManRef;

    QWidget *           m_pWidget;
    QMenu *             m_pEditMenu;
    QUndoStack *        m_pUndoStack;

    QSet<HyGuiFrame *>  m_Links;

    HyCamera2d *        m_pCamera;

    virtual void OnDraw_Load(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Unload(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Show(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Hide(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Update(IHyApplication &hyApp) = 0;

    virtual void OnLink(HyGuiFrame *pFrame) = 0;
    virtual void OnUnlink(HyGuiFrame *pFrame) = 0;
    virtual void OnUpdateLink(HyGuiFrame *pFrame) = 0;

public:
    ItemWidget(eItemType eType, const QString sPath, WidgetAtlasManager &AtlasManRef);
    virtual ~ItemWidget();

    bool IsDrawLoaded() const                       { return (m_pCamera != NULL); }

    QWidget *GetWidget() const                      { return m_pWidget; }
    QMenu *GetEditMenu() const                      { return m_pEditMenu; }
    QUndoStack *GetUndoStack()                      { return m_pUndoStack; }

    WidgetAtlasManager &GetAtlasManager()           { return m_AtlasManRef; }

protected:

};
Q_DECLARE_METATYPE(ItemWidget *)

#endif // ITEMWIDGET_H
