/**************************************************************************
 *	HyGuiDependencies.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "HyGuiDependencies.h"
#include "HyGlobal.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

HyGuiDependencies::HyGuiDependencies(QString sMetaDataPath, WidgetAtlasManager *pAtlasMan) :    m_MetaDir(sMetaDataPath),
                                                                                                m_pAtlasMan(pAtlasMan)
{
    QFile metaDependFile(m_MetaDir.absoluteFilePath(HYGUIPATH_MetaDataDependenciesFileName));
    if(metaDependFile.exists())
    {
        if(!metaDependFile.open(QIODevice::ReadOnly))
            HyGuiLog(QString("HyGuiDependencies::HyGuiDependencies() could not open ") % HYGUIPATH_MetaDataDependenciesFileName, LOGTYPE_Error);

#ifdef HYGUI_UseBinaryMetaFiles
        QJsonDocument dependenciesDoc = QJsonDocument::fromBinaryData(metaDependFile.readAll());
#else
        QJsonDocument dependenciesDoc = QJsonDocument::fromJson(metaDependFile.readAll());
#endif
        metaDependFile.close();

        QJsonObject dependenciesObj = dependenciesDoc.object();
        QJsonArray frameArray = dependenciesObj["frames"].toArray();
        for(int i = 0; i < frameArray.size(); ++i)
        {
            QJsonObject frameObj = frameArray[i].toObject();

            QRect rAlphaCrop(QPoint(frameObj["cropLeft"].toInt(), frameObj["cropTop"].toInt()), QPoint(frameObj["cropRight"].toInt(), frameObj["cropBottom"].toInt()));

            QJsonArray frameLinksArray = frameObj["links"].toArray();
        }
    }
}

HyGuiDependencies::~HyGuiDependencies()
{
    
}


bool HyGuiDependencies::IsAtlasFramesAvailable()
{
    return m_pAtlasMan->IsSelectedFrames();
}

void HyGuiDependencies::GetAtlasFrames(Item *pRequester, QList<const HyGuiFrame *> frameListOut)
{
}

void HyGuiDependencies::Save(Item *pItem)
{

    pItem->GetName
}
