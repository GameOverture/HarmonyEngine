#ifndef WIDGETRENDERER_H
#define WIDGETRENDERER_H

#include "HyGlobal.h"

#include <QWidget>

#include "GL/glew.h"
#include <QGLFormat>
#include <QTimer>

#include "Harmony/HyEngine.h"
#include "HyApp.h"

#include "Item.h"

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

class WidgetRenderer : public QWidget
{
    Q_OBJECT

    QGLFormat           m_glFormat;

    HyApp *             m_pHyApp;
    HyEngine *          m_pHyEngine;
    
    bool                m_bInitialized;

public:
    explicit WidgetRenderer(QWidget *parent = 0);
    ~WidgetRenderer();

    void ClearItems();

    void OpenItem(Item *pItem);
    void CloseItem(Item *pItem);

private:
    Ui::WidgetRenderer *ui;

private slots:
    void Render();
    void on_tabWidget_currentChanged(int index);
};

#endif // WIDGETRENDERER_H
