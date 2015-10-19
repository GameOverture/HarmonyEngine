#include <QUuid>
#include <QPainter>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "HyGuiTexture.h"
#include "WidgetAtlasManager.h"

//HyGuiAtlas::Frame::Frame(HyGuiAtlas *const pTexOwner, int iTag, QString sName) :    m_pTexOwner(pTexOwner),
//                                                                                    m_iTag(iTag)
//{
//    m_pTreeItem = m_pTexOwner->GetAtlasOwner()->CreateTreeItem(m_pTexOwner->GetTreeItem(), sName, ATLAS_Frame);
//}

//HyGuiAtlas::HyGuiAtlas(WidgetAtlasManager *const pAtlasOwner) :    m_pAtlasOwner(pAtlasOwner),
//                                                                m_bDirty(true)
//{
//    m_pAtlasOwner->SetPackerSettings(&m_Packer);
    
//    m_MetaDir.setPath(pAtlasOwner->GetProjOwner()->GetPath() % HYGUIPATH_RelMetaAtlasDir);
//    m_MetaTmpDir.setPath(pAtlasOwner->GetProjOwner()->GetPath() % HYGUIPATH_RelMetaDataTmpDir);
//    m_DataDir.setPath(pAtlasOwner->GetProjOwner()->GetPath() % HYGUIPATH_RelDataAtlasDir);
    
    
//    // All textures are named "00000", "00001", "00002", etc.
//    int iTexId = pAtlasOwner->GetNextTextureId();
    
//    QString sNewTexName = HyGlobal::MakeFileNameFromCounter(iTexId);
//    m_AtlasImg.setFile(m_DataDir.path() % "/" % sNewTexName % ".png");
    
//    sNewTexName.sprintf("Texture: %d", iTexId);
//    m_pTreeItem = m_pAtlasOwner->CreateTreeItem(NULL, sNewTexName, ATLAS_Texture);
//}

//HyGuiAtlas::~HyGuiAtlas()
//{
//    // TODO: Remove meta dir for this texture, then rename all other Texture objects (if necessary) to be in order
    
//    // TODO: Properly remove all the HyFrameData's
//    delete m_pTreeItem;
//}

//QJsonArray HyGuiAtlas::GetFrameArray()
//{
//    QJsonArray frameArray;
    
//    for(int i = 0; i < m_Packer.images.size(); ++i)
//    {
//        inputImage &imgInfoRef = m_Packer.images[i];
       
//        QJsonObject frame;
//        frame.insert("hash", QJsonValue(static_cast<qint32>(imgInfoRef.hash)));
//        frame["x"] = imgInfoRef.pos.x();
//        frame["y"] = imgInfoRef.pos.y();
//        frame["width"] = imgInfoRef.size.width();
//        frame["height"] = imgInfoRef.size.height();
//        frame["rotated"] = imgInfoRef.rotated;
        
//        frameArray.append(QJsonValue(frame));
//    }
    
//    return frameArray;
//}

//void HyGuiAtlas::GenerateImg()
//{

//}

//// Returns a list of string lists that contain all the image paths that didn't fit on this texture
//// Each entry in the QList are hints towards what new texture each missing image belongs to.
//QList<QStringList> HyGuiAtlas::ImportFrames(const QStringList sImportImgPathList)
//{
//    m_pAtlasOwner->SetPackerSettings(&m_Packer);
    
//    // Place all the imported images into this current texture (whether they will all fit or not)
//    for(int i = 0; i < sImportImgPathList.size(); ++i)
//    {
//        QFileInfo fileInfo(sImportImgPathList[i]);
//        QImage img(fileInfo.absoluteFilePath());
//        quint32 uiHash = rc_crc32(0, img.bits(), img.byteCount());
        
//        // Create unique filename for metadata image (is this overkill (QUuid::createUuid)?), and save it out
//        QString sNewMetaImgPath;
//        sNewMetaImgPath = sNewMetaImgPath.sprintf("%010u-%s", uiHash, fileInfo.baseName().toStdString().c_str());
//        sNewMetaImgPath += ("." % fileInfo.suffix());
//        sNewMetaImgPath = m_MetaDir.path() % "/" % sNewMetaImgPath;
//        img.save(sNewMetaImgPath);
        
//        LoadFrame(img, uiHash, fileInfo.baseName(), sNewMetaImgPath, false);
//    }

//    return PackFrames();
//}

//void HyGuiAtlas::LoadFrame(const QImage &img, quint32 uiHash, QString sName, QString sAbsolutePath, bool bSetPackerSettings /*= true*/)
//{
//    if(bSetPackerSettings)
//        m_pAtlasOwner->SetPackerSettings(&m_Packer);
    
//    m_Packer.addItem(img, uiHash, new Frame(this, -1, sName), sAbsolutePath);
//}

//// Returns a list of string lists that contain all the image paths that didn't fit on this texture
//// The _metaData source images will be moved to the _metaData's "tmp" directory.
//// Each entry in the QList are hints towards what additional texture each missing image could belong to.
//QList<QStringList> HyGuiAtlas::PackFrames()
//{
//    m_Packer.pack(m_pAtlasOwner->GetHeuristicIndex(), m_pAtlasOwner->GetTexWidth(), m_pAtlasOwner->GetTexHeight());
    
//    // If number of packer's bins (aka texture sheets) exceed '1', remove them, and store the paths split them into other Texture objects
//    QList<QStringList> missingImgPaths;
//    if(m_Packer.bins.size() > 1)
//    {
//        for(int i = 1; i < m_Packer.bins.size(); ++i)
//            missingImgPaths.push_back(QStringList());
        
//        // NOTE: The wacky scriptum library needs to call ClearBin, before removing any images associated with the removed bins
//        while(m_Packer.bins.size() > 1)
//        {
//            m_Packer.bins.removeLast();
//            m_Packer.ClearBin(m_Packer.bins.count());
//        }
        
//        // Find all source images that aren't on texture '0' and store them off in containers
//        for(int i = 0; i < m_Packer.images.size(); ++i)
//        {
//            if(m_Packer.images[i].textureId != 0)
//            {
//                missingImgPaths[m_Packer.images[i].textureId - 1].push_back(m_Packer.images[i].path);
                
//                //  Move the source image frame that didn't fit on texture to the _MetaData's temp directory
//                QFile srcImg(m_Packer.images[i].path);
//                if(srcImg.rename(m_MetaTmpDir.path() % srcImg.fileName()) == false)
//                    HYLOG("Could not move image src metafile to tmp directory: " % m_Packer.images[i].path, LOGTYPE_Warning);
                
//                // Remove the inputImage from packer and delete the HyGuiFrameData (aka inputImage's id)
//                Frame *pData = static_cast<Frame *>(m_Packer.images[i].id);
//                m_Packer.removeId(pData);
//                delete pData;
//            }
//        }
//    }
    
//    m_bDirty = (m_Packer.bins.count() > 0);
//    HyAssert(m_Packer.bins.count() <= 1, "HyGuiTexture::PackFrames() resulted in a packer with more than one texture");
    
//    return missingImgPaths;
//}

