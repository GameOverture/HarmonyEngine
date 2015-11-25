#ifndef WIDGETATLASMANAGER_H
#define WIDGETATLASMANAGER_H

#include <QWidget>
#include <QDir>
#include <QMouseEvent>

#include "ItemProject.h"
#include "WidgetAtlasGroup.h"

namespace Ui {
class WidgetAtlasManager;
}

class WidgetAtlasManager : public QWidget
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

private:
    Ui::WidgetAtlasManager *ui;

    void AddAtlasGroup(int iId = -1);
};

#endif // WIDGETATLASMANAGER_H
