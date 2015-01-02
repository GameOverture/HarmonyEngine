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

// Forward declaration
class HyGuiTexture;

class WidgetAtlas : public QWidget
{
    Q_OBJECT
    
    ItemProject *               m_pProjOwner;
    QList<HyGuiTexture *>       m_Textures;
    
    bool                        m_bSettingsDirty;
    
    ImagePacker                 m_cachedImagePackerSettings;
    
    enum ePageType
    {
        PAGE_Settings = 0,
        PAGE_Frames
    };
    
public:
    explicit WidgetAtlas(QWidget *parent = 0);
    explicit WidgetAtlas(ItemProject *pProjOwner, QWidget *parent = 0);
    ~WidgetAtlas();
    
    void SetProjOwner(ItemProject *pProjOwner)  { m_pProjOwner = pProjOwner; }
    ItemProject *GetProjOwner()                 { return m_pProjOwner; }
    
    void SetPackerSettings(ImagePacker *pPacker);
    int GetTexWidth();
    int GetTexHeight();
    int GetHeuristicIndex();
    
    QTreeWidgetItem *CreateTreeItem(QTreeWidgetItem *pParent, QString sName, eAtlasNodeType eType);
    
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
    
    void on_btnSaveSettings_clicked();
    
private:
    Ui::WidgetAtlas *ui;
    
    HyGuiTexture *GetActiveTexture();
    
    void GenTextureSheets();
    
};

#endif // WIDGETATLAS_H
