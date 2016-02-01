#ifndef WIDGETATLASMANAGER_H
#define WIDGETATLASMANAGER_H

#include <QWidget>
#include <QDir>
#include <QMouseEvent>

#include "ItemProject.h"
#include "WidgetAtlasGroup.h"\
#include "IHyGuiDrawItem.h"

namespace Ui {
class WidgetAtlasManager;
}

class WidgetAtlasManager : public QWidget, public IHyGuiDrawItem
{
    Q_OBJECT

    ItemProject *                   m_pProjOwner;

    QDir                            m_MetaDir;
    QDir                            m_DataDir;

public:
    explicit WidgetAtlasManager(QWidget *parent = 0);
    explicit WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlasManager();

    void SaveData();

    void PreviewAtlasGroup();
    void HideAtlasGroup();
    
    virtual void Hide();
    virtual void Show();
    virtual void Draw(WidgetRenderer &renderer);

private:
    Ui::WidgetAtlasManager *ui;

    void AddAtlasGroup(int iId = -1);
};

#endif // WIDGETATLASMANAGER_H
