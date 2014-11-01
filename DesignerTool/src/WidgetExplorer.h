#ifndef WIDGETEXPLORER_H
#define WIDGETEXPLORER_H

#include <QWidget>
#include <QDir>
#include <QTreeWidget>
#include <QMenu>

#include "HyGlobal.h"
#include "Item.h"

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
    
    Item *GetCurProjSelected();
    Item *GetCurItemSelected();
    Item *GetCurDirSelected(bool bIncludePrefixDirs);
    
    QStringList GetOpenProjectPaths();

private:
    Ui::WidgetExplorer *ui;
    
    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, Item *pItem);
    
private slots:
    void OnContextMenu(const QPoint &pos);
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *);
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
};

#endif // WIDGETEXPLORER_H
