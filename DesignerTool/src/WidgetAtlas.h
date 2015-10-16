#ifndef WIDGETATLAS_H
#define WIDGETATLAS_H

#include <QWidget>
#include <QTreeWidget>
#include <QList>

#include "ItemProject.h"
#include "scriptum/imagepacker.h"

namespace Ui {
class WidgetAtlasManager;
}

// Forward declaration
class HyGuiAtlasGroup;

class WidgetAtlasManager : public QWidget
{
    Q_OBJECT
    
    ItemProject *               m_pProjOwner;
    
    QFileInfo                   m_MetaDataFile;

    QList<HyGuiAtlasGroup *>    m_AtlasGroups;
    
public:
    explicit WidgetAtlasManager(QWidget *parent = 0);
    explicit WidgetAtlasManager(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlasManager();
    
    void SetProjOwner(ItemProject *pProjOwner)  { m_pProjOwner = pProjOwner; }
    ItemProject *GetProjOwner()                 { return m_pProjOwner; }
    
    void SetPackerSettings(ImagePacker *pPacker);
    int GetTexWidth();
    int GetTexHeight();
    int GetHeuristicIndex();
    
    int GetNextTextureId();
    
    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, QString sName, eAtlasNodeType eType);
    
private slots:    
    void on_btnAddFiles_clicked();
    
    void on_btnAddDir_clicked();
    
    void on_tabWidget_currentChanged(int index);
    
    void on_frameList_itemDoubleClicked(QTreeWidgetItem *item, int column);
    
    void on_frameList_itemClicked(QTreeWidgetItem *item, int column);
    
    void on_btnSettings_clicked();

private:
    Ui::WidgetAtlas *ui;
    
    void LoadData();
    void SaveData();
    
    HyGuiTexture *GetActiveTexture();
    
    void ImportFrames(QStringList sImportImgList);
    void RepackFrames();
    
    void RenderAtlas();
};

#endif // WIDGETATLAS_H
