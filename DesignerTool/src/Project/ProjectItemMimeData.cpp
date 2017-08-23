#include "ProjectItemMimeData.h"
#include "ProjectItem.h"
#include "Project.h"

#include <QClipboard>

ProjectItemMimeData::ProjectItemMimeData(ProjectItem *pProjItem) : m_pProjItem(pProjItem)
{
    QJsonValue itemValue = m_pProjItem->GetModel()->GetJson();

    // STANDARD INFO
    QJsonObject clipboardObj;
    clipboardObj.insert("project", m_pProjItem->GetProject().GetAbsPath());
    clipboardObj.insert("itemType", HyGlobal::ItemName(m_pProjItem->GetType()));
    clipboardObj.insert("itemName", m_pProjItem->GetName(true));
    clipboardObj.insert("src", itemValue);

    // IMAGE INFO
    QList<AtlasFrame *> atlasFrameList = m_pProjItem->GetModel()->GetAtlasFrames();
    QJsonArray imagesArray;
    for(int i = 0; i < atlasFrameList.size(); ++i)
    {
        QJsonObject atlasFrameObj;
        atlasFrameObj.insert("checksum", QJsonValue(static_cast<qint64>(atlasFrameList[i]->GetImageChecksum())));
        atlasFrameObj.insert("name", QJsonValue(atlasFrameList[i]->GetName()));
        atlasFrameObj.insert("url", QJsonValue(GetCurProjSelected()->GetMetaDataAbsPath() % HyGlobal::ItemName(ITEM_DirAtlases) % "/" % atlasFrameList[i]->ConstructImageFileName()));
        imagesArray.append(atlasFrameObj);
    }
    clipboardObj.insert("images", imagesArray);

    // FONT INFO
    QStringList fontUrlList = m_pProjItem->GetModel()->GetFontUrls();
    QJsonArray fontUrlArray;
    for(int i = 0; i < fontUrlList.size(); ++i)
        fontUrlArray.append(fontUrlList[i]);
    clipboardObj.insert("fonts", fontUrlArray);

    // TODO: AUDIO INFO
    //clipboardObj.insert("audio", GetAudioWavs())

    // Serialize the item info into json source
    QByteArray src = JsonValueToSrc(QJsonValue(clipboardObj));
    QClipboard *pClipboard = QApplication::clipboard();
    pClipboard->setText(src);

    setData(HYGUI_MIMETYPE, src);
}

/*virtual*/ ProjectItemMimeData::~ProjectItemMimeData()
{
}

/*virtual*/ bool ProjectItemMimeData::hasFormat(const QString &sMimeType) const /*override*/
{
    if(HYGUI_MIMETYPE == sMimeType.toLower() || "application/json" == sMimeType.toLower())
        return true;

    return false;
}

/*virtual*/ QStringList ProjectItemMimeData::formats() const /*override*/
{
    QStringList sFormatList;
    sFormatList << HYGUI_MIMETYPE;
    sFormatList << "application/json";

    return sFormatList;
}

/*virtual*/ QVariant ProjectItemMimeData::retrieveData(const QString &mimeType, QVariant::Type type) const /*override*/
{
    if((mimeType.toLower() == HYGUI_MIMETYPE || mimeType.toLower() == "application/json") &&
       (type == QVariant::UserType || type == QVariant::ByteArray || type == QVariant::String))
    {
        return QVariant(QByteArray(data(HYGUI_MIMETYPE)));
    }

    return QVariant();
}
