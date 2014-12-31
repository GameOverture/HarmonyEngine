#ifndef WIDGETATLAS_H
#define WIDGETATLAS_H

#include <QWidget>
#include <QTreeWidget>
#include <QList>

#include "ItemProject.h"
#include "scriptum/imagepacker.h"

namespace Ui {
class WidgetAtlas;
}

class WidgetAtlas : public QWidget
{
    Q_OBJECT
    
    ItemProject * const     m_pProjOwner;
    
    struct tTexture
    {
        ImagePacker         packer;
        QTreeWidgetItem     treeItem;
        
        bool                bDirty;
        
        tTexture() : bDirty(true)
        { }
    };
    
    QList<tTexture *>       m_Textures;
    
    bool                    m_bDirty;
    
    
    
public:
    explicit WidgetAtlas(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlas();
    
    
    
private slots:
    void on_btnTexSize256_clicked();
    
    void on_btnTexSize512_clicked();
    
    void on_btnTexSize1024_clicked();
    
    void on_btnTexSize2048_clicked();
    
    void on_btnAddFiles_clicked();
    
    void on_btnAddDir_clicked();
    
    void on_cmbHeuristic_currentIndexChanged(const QString &arg1);
    
    void on_cmbSortOrder_currentIndexChanged(const QString &arg1);
    
    void on_cmbRotationStrategy_currentIndexChanged(const QString &arg1);
    
    void on_sbFrameMarginTop_valueChanged(int arg1);
    
    void on_sbFrameMarginRight_valueChanged(int arg1);
    
    void on_sbFrameMarginBottom_valueChanged(int arg1);
    
    void on_sbFrameMarginLeft_valueChanged(int arg1);
    
    void on_tabWidget_currentChanged(int index);
    
private:
    Ui::WidgetAtlas *ui;
    
    bool ImportImage(QString sImagePath);
    
    bool TryPackNew(QString sImagePath, tTexture *pTex);
    
    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, QString sName, eAtlasNodeType eType);
    
    void RebuildDirtyTextures();
};

#endif // WIDGETATLAS_H
