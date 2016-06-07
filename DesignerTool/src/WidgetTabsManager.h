/**************************************************************************
 *	WidgetTabsManager.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef WIDGETTABSMANAGER_H
#define WIDGETTABSMANAGER_H

#include <QWidget>
#include <QQueue>

#include "ItemProject.h"

namespace Ui {
class WidgetTabsManager;
}

class TabPage : public QWidget
{
    Q_OBJECT

    Item *          m_pItem;

public:
    TabPage(Item *pItem, QWidget *pParent) : m_pItem(pItem),
                                             QWidget(pParent)
    { }

    Item *GetItem() { return m_pItem; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WidgetTabsManager : public QWidget, public IHyApplication
{
    Q_OBJECT

    ItemProject *       m_pProjOwner;

    enum eQueuedAction
    {
        QUEUEDITEM_Open = 0,
        QUEUEDITEM_Show,
        QUEUEDITEM_Close
    };
    QQueue<std::pair<Item *, eQueuedAction> > m_ActionQueue;
    
public:
    explicit WidgetTabsManager(QWidget *parent = 0);
    explicit WidgetTabsManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetTabsManager();
    
    void OpenItem(Item *pItem);
    void CloseItem(Item *pItem);

    // IHyApplication overrides
    virtual bool Initialize();
    virtual bool Update();
    virtual bool Shutdown();

private:
    Ui::WidgetTabsManager *ui;

private slots:
    void on_tabWidget_currentChanged(int index);
};

#endif // WIDGETTABSMANAGER_H
