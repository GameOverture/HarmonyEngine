#ifndef HYGUIATLAS_H
#define HYGUIATLAS_H

#include <QTreeWidgetItem>
#include <QDir>
#include <QMap>

#include "scriptum/imagepacker.h"

// Forward declaration
class WidgetAtlasManager;

class HyGuiAtlas
{
    class Frame
    {
        HyGuiAtlas *const m_pTexOwner;
        
        int                 m_iTag;         // Used as a temp integer index to an import array, and will become the integer index to what texture it belongs to.
        QTreeWidgetItem *   m_pTreeItem;
        
    public:
        Frame(HyGuiAtlas *const pTexOwner, int iTag, QString sName);
        
        int GetTag()            { return m_iTag; }
        void SetTag(int iTag)   { m_iTag = iTag; }
        QString GetName()       { return m_pTreeItem->text(0); }
    };
    
    WidgetAtlasManager *const      m_pAtlasOwner;
    
    QDir                    m_MetaDir;
    QDir                    m_MetaTmpDir;
    QDir                    m_DataDir;
    
    QFileInfo               m_AtlasImg;
    
    ImagePacker             m_Packer;

    QTreeWidgetItem *       m_pTreeItem;
    int                     m_iLoadGroup;
    
    bool                    m_bDirty;
    
public:
    HyGuiAtlas(WidgetAtlasManager *const pAtlasOwner);
    ~HyGuiAtlas();
    
    bool IsDirty()          { return m_bDirty; }
    
    // Returns a list of string lists that contain all the image paths that didn't fit on this texture
    // Each entry in the QList are hints towards what new texture each missing image belongs to.
    QList<QStringList> ImportFrames(const QStringList sImportList);
    
    void LoadFrame(const QImage &img, quint32 uiHash, QString sName, QString sAbsolutePath, bool bSetPackerSettings = true);
    
    // Returns a list of string lists that contain all the image paths that didn't fit on this texture
    // Each entry in the QList are hints towards what new texture each missing image belongs to.
    QList<QStringList> PackFrames();
    
    WidgetAtlasManager *const GetAtlasOwner()  { return m_pAtlasOwner; }
    QTreeWidgetItem *GetTreeItem()      { return m_pTreeItem; }
   
    // Generates a texture and atlas file
    QJsonArray GetFrameArray();
    int GetLoadGroup()                  { return m_iLoadGroup; }
    void GenerateImg();
};

#endif // HYGUIATLAS_H
