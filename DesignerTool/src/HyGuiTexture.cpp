#include <QUuid>

#include "HyGuiTexture.h"
#include "WidgetAtlas.h"

HyGuiFrameData::HyGuiFrameData(HyGuiTexture *const pTexOwner, int iTag, QString sName, QString sPath) : m_pTexOwner(pTexOwner),
                                                                                                        m_iTag(iTag),
                                                                                                        m_sPath(sPath)
{
    m_pTreeItem = m_pTexOwner->GetAtlasOwner()->CreateTreeItem(m_pTexOwner->GetTreeItem(), sName, ATLAS_Frame);
}

HyGuiTexture::HyGuiTexture(WidgetAtlas *const pAtlasOwner) :    m_pAtlasOwner(pAtlasOwner),
                                                                m_bDirty(false)
{
    m_pAtlasOwner->SetPackerSettings(&m_Packer);
    
    QString sAtlasMetaDataPath = pAtlasOwner->GetProjOwner()->GetMetaDataPath() % "atlas/";
    m_MetaDir.setPath(sAtlasMetaDataPath);
    
    // All dirs are named "00000", "00001", "00002", etc. and are supposed to always be in order
    QFileInfoList list = m_MetaDir.entryInfoList(QDir::Dirs, QDir::Name);
    QString sNewTexDir = list[list.count() - 1].baseName();
    int iTexId = sNewTexDir.toInt() + 1;  // + 1 will create new dir, following existing order
    sNewTexDir.sprintf("%05d", iTexId);
    
    m_pTreeItem = m_pAtlasOwner->CreateTreeItem(NULL, sNewTexDir, ATLAS_Texture);
    
    sNewTexDir += '/';
    
    if(m_MetaDir.mkdir(sAtlasMetaDataPath % sNewTexDir) && QFile::exists(sAtlasMetaDataPath % sNewTexDir))
        m_MetaDir.setPath(sAtlasMetaDataPath % sNewTexDir);
    else
        HYLOG("Could not create atlas meta directory: " % sNewTexDir, LOGTYPE_Error);
}

HyGuiTexture::~HyGuiTexture()
{
    // TODO: Remove meta dir for this texture, then rename all other Texture objects (if necessary) to be in order
    
    // TODO: Properly remove all the HyFrameData's
    delete m_pTreeItem;
}

// Returns a list of string lists that contain all the image paths that didn't fit on this texture
// Each entry in the QList are hints towards what new texture each missing image belongs to.
QList<QStringList> HyGuiTexture::ImportImgs(const QStringList sImportList)
{
    m_pAtlasOwner->SetPackerSettings(&m_Packer);
    
    // TODO: Track packer's missingImages and store them in the returned QList<QStringList>
    
    // Place all the imported images into this current texture (whether they will all fit or not)
    for(int i = 0; i < sImportList.size(); ++i)
    {
        QString sPath = sImportList[i];
        QImage img(sPath);
        
        // Change sPath from (the passed) imported path, to new meta data path location
        QFileInfo fileInfo(sPath);
        QString sFileName;
        
        // Create unique filename for metadata image (is this overkill?), and save it out
        QUuid uniqueId = QUuid::createUuid();
        sFileName = uniqueId.toString();
        sFileName.replace(QChar('{'), "");
        sFileName.replace(QChar('}'), "");
        sFileName += ("." % fileInfo.suffix());
        sPath = m_MetaDir.path() % sFileName;
        img.save(sPath);
        
        // TODO: Delete pData somewhere
        HyGuiFrameData *pData = new HyGuiFrameData(this, i, fileInfo.baseName(), sPath);
        m_Packer.addItem(img, pData, sPath);
    }

    m_Packer.pack(m_pAtlasOwner->GetHeuristicIndex(), m_pAtlasOwner->GetTexWidth(), m_pAtlasOwner->GetTexHeight());
    
    // If number of packer's bins (aka texture sheets) exceed '1', remove them, and store the paths split them into other Texture objects
    QList<QStringList> missingImgPaths;
    if(m_Packer.bins.size() > 1)
    {
        for(int i = 1; i < m_Packer.bins.size(); ++i)
            missingImgPaths.push_back(QStringList());
        
        // NOTE: The wacky scriptum library needs to call ClearBin, before removing any images associated with the removed bins
        while(m_Packer.bins.size() > 1)
        {
            m_Packer.bins.removeLast();
            m_Packer.ClearBin(m_Packer.bins.count());
        }
        
        QList<HyGuiFrameData *> imagesToRemove;
        for(int i = 0; i < m_Packer.images.size(); ++i)
        {
            if(m_Packer.images[i].textureId != 0)
            {
                QString sImportPath = sImportList[reinterpret_cast<HyGuiFrameData *>(m_Packer.images[i].id)->GetTag()];
                missingImgPaths[m_Packer.images[i].textureId - 1].push_back(sImportPath);
                
                imagesToRemove.push_back(reinterpret_cast<HyGuiFrameData *>(m_Packer.images[i].id));
            }
        }
        
        for(int i = 0; i < imagesToRemove.size(); ++i)
        {
            m_Packer.removeId(imagesToRemove[i]);
            
            HyGuiFrameData *pData = imagesToRemove[i];
            delete pData;
            // TODO: Delete the metadata image sitting on disk
        }
    }
    
    m_bDirty = (m_Packer.bins.count() > 0);
    return missingImgPaths;
}

// Returns a list of string lists that contain all the image paths that didn't fit on this texture
// Each entry in the QList are hints towards what new texture each missing image belongs to.
QList<QStringList> HyGuiTexture::RepackImgs()
{
    // Create tmp directory
    QDir tmpDir(m_MetaDir.path());
    tmpDir.cdUp();
    if(tmpDir.cd("tmp") == false)
        tmpDir.mkpath(".");
    
    // Move images to tmp dir
    for(int i = 0; i < m_Packer.images.size(); ++i)
    {
        QFileInfo info(m_Packer.images[i].path);
        QString sImgName = reinterpret_cast<HyGuiFrameData *>(m_Packer.images[i].id)->GetName();
        QFile::copy(m_Packer.images[i].path, QDir::cleanPath(tmpDir.path() % "/" % sImgName % "." % info.suffix()));
    }
    
    // TODO: Reimport images but somehow avoid images with same name within tmp directory
    return QList<QStringList>();
}
