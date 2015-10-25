#ifndef WIDGETRENDERER_H
#define WIDGETRENDERER_H

#include "HyGlobal.h"

#include <QWidget>

#include "Harmony/HyEngine.h"

#include "Item.h"
#include "ItemProject.h"

namespace Ui {
class WidgetRenderer;
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

class WidgetRenderer : public QWidget, public IHyApplication
{
    Q_OBJECT
    
    HyCamera2d *        m_pCam;
    bool                m_bInitialized;

public:
    explicit WidgetRenderer(QWidget *parent = 0);
    ~WidgetRenderer();

    virtual bool Initialize();
    virtual bool Update();
    virtual bool Shutdown();

    Item *GetItem(int iIndex = -1);
    void ShowItem(Item *pItem);

    void ClearItems();

    void OpenItem(Item *pItem);
    void CloseItem(Item *pItem);

private:
    Ui::WidgetRenderer *ui;

private slots:
    void on_tabWidget_currentChanged(int iIndex);
};

#endif // WIDGETRENDERER_H
