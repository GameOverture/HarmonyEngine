#ifndef HYGUITEXTURE_H
#define HYGUITEXTURE_H

#include <QTreeWidgetItem>
#include <QDir>
#include <QMap>

#include "scriptum/imagepacker.h"

// Forward declaration
class WidgetAtlas;

class HyGuiTexture
{
    WidgetAtlas *const      m_pAtlasOwner;
    
    QDir                    m_MetaDir;
    QDir                    m_DataDir;
    
    QFileInfo               m_AtlasImg;
    
    ImagePacker             m_Packer;
    QTreeWidgetItem *       m_pTreeItem;
    
    QMap<QString, QString>  m_FrameNameMap;
    
    bool                    m_bDirty;
    
public:
    HyGuiTexture(WidgetAtlas *const pAtlasOwner);
    ~HyGuiTexture();
    
    bool IsDirty()          { return m_bDirty; }
    
    // Returns a list of string lists that contain all the image paths that didn't fit on this texture
    // Each entry in the QList are hints towards what new texture each missing image belongs to.
    QList<QStringList> ImportImgs(const QStringList sImportList);
    
    // Returns a list of string lists that contain all the image paths that didn't fit on this texture
    // Each entry in the QList are hints towards what new texture each missing image belongs to.
    QList<QStringList> RepackImgs();
    
    WidgetAtlas *const GetAtlasOwner()  { return m_pAtlasOwner; }
    QTreeWidgetItem *GetTreeItem()      { return m_pTreeItem; }
   
    // Generates a texture and atlas file
    void GetFrameList(QJsonObject &frameList);
    void GenerateImg();
};

class HyGuiFrameData
{
    HyGuiTexture *const m_pTexOwner;
    
    int                 m_iTextureId;         // Just used as a temp integer to a texture index that we're trying to be on
    QString             m_sPath;
    QTreeWidgetItem *   m_pTreeItem;
    
public:
    HyGuiFrameData(HyGuiTexture *const pTexOwner, int iTextureId, QString sName, QString sPath);
    
    int GetTextureId()        { return m_iTextureId; }
    QString GetName()   { return m_pTreeItem->text(0); }
    QString GetPath()   { return m_sPath; }
};

#endif // HYGUITEXTURE_H
