#ifndef WIDGETEXPLORER_H
#define WIDGETEXPLORER_H

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QMenu>

#include "HyGlobal.h"
#include "ItemProject.h"

#include <vector>
using std::vector;

namespace Ui {
class WidgetExplorer;
}

class WidgetExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetExplorer(QWidget *parent = 0);
    ~WidgetExplorer();
    
    void AddItem(eItemType eNewItemType, const QString sNewItemPath, bool bOpenAfterAdd);
    void RemoveItem(Item *pItem);
    
    void SelectItem(Item *pItem);
    
    ItemProject *GetCurProjSelected();
    Item *GetCurItemSelected();
    Item *GetCurDirSelected(bool bIncludePrefixDirs);
    
    QStringList GetOpenProjectPaths();

private:
    Ui::WidgetExplorer *ui;
    
    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, Item *pItem);
    
    QTreeWidgetItem *GetSelectedTreeItem();
    
private slots:
    void OnContextMenu(const QPoint &pos);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void on_treeWidget_itemSelectionChanged();
};

#endif // WIDGETEXPLORER_H
