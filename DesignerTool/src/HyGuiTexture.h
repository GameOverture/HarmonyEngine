#ifndef HYGUITEXTURE_H
#define HYGUITEXTURE_H

#include <QTreeWidgetItem>
#include <QDir>

#include "scriptum/imagepacker.h"

// Forward declaration
class WidgetAtlas;

class HyGuiTexture
{
    WidgetAtlas *const  m_pAtlasOwner;
    
    QDir                m_MetaDir;
    ImagePacker         m_Packer;
    QTreeWidgetItem *   m_pTreeItem;
    
    bool                m_bDirty;
    
public:
    HyGuiTexture(WidgetAtlas *const pAtlasOwner);
    ~HyGuiTexture();
    
    // Returns a list of string lists that contain all the image paths that didn't fit on this texture
    // Each entry in the QList are hints towards what new texture each missing image belongs to.
    QList<QStringList> ImportImgs(const QStringList sImportList);
    
    // Returns a list of string lists that contain all the image paths that didn't fit on this texture
    // Each entry in the QList are hints towards what new texture each missing image belongs to.
    QList<QStringList> RepackImgs();
    
public:
    
    WidgetAtlas *const GetAtlasOwner()  { return m_pAtlasOwner; }
    QTreeWidgetItem *GetTreeItem()      { return m_pTreeItem; }
   
};

class HyGuiFrameData
{
    HyGuiTexture *const m_pTexOwner;
    
    int                 m_iTag;
    QString             m_sPath;
    QTreeWidgetItem *   m_pTreeItem;
    
public:
    HyGuiFrameData(HyGuiTexture *const pTexOwner, int iTag, QString sName, QString sPath);
    
    int GetTag()        { return m_iTag; }
    QString GetName()   { return m_pTreeItem->text(0); }
};

#endif // HYGUITEXTURE_H
