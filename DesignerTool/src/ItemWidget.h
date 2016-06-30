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

class WidgetAtlasManager;

class ItemWidget : public Item
{
    friend class WidgetAtlasManager;
    friend class WidgetTabsManager;


    friend class ItemProject;

    void DrawOpen(IHyApplication &hyApp);
    void DrawClose(IHyApplication &hyApp);

    void DrawShow(IHyApplication &hyApp);
    void DrawHide(IHyApplication &hyApp);

    void DrawUpdate(IHyApplication &hyApp);

protected:
    WidgetAtlasManager *m_pAtlasMan;

    QWidget *           m_pWidget;
    QMenu *             m_pEditMenu;

    QSet<HyGuiFrame *>  m_Links;

    HyCamera2d *        m_pCamera;

    virtual void OnDraw_Open(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Close(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Show(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Hide(IHyApplication &hyApp) = 0;
    virtual void OnDraw_Update(IHyApplication &hyApp) = 0;

public:
    ItemWidget(eItemType eType, const QString sPath, WidgetAtlasManager *pAtlasMan);
    virtual ~ItemWidget();

    QWidget *GetWidget() const                      { return m_pWidget; }
    QMenu *GetEditMenu() const                      { return m_pEditMenu; }

    WidgetAtlasManager &GetAtlasManager()           { return *m_pAtlasMan; }

protected:
    virtual void Link(HyGuiFrame *pFrame, QVariant param);
    virtual void UnLink(HyGuiFrame *pFrame);
};

#endif // ITEMWIDGET_H
