/**************************************************************************
 *	HyGuiDependencies.h
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HYGUIDEPENDENCIES_H
#define HYGUIDEPENDENCIES_H

#include "WidgetAtlasManager.h"

#include <QDir>

class HyGuiDependencies
{
    QDir                        m_MetaDir;
    WidgetAtlasManager *        m_pAtlasMan;

    enum eType
    {
        TYPE_Frame = 0,
        TYPE_Item,
    };

    QString

public:
    HyGuiDependencies(QString sMetaDataPath, WidgetAtlasManager *pAtlasMan);
    ~HyGuiDependencies();

    bool IsAtlasFramesAvailable();
    void GetAtlasFrames(Item *pRequester, QList<const HyGuiFrame *> frameListOut);

    void Save(Item *pItem);
};

#endif // HYGUIDEPENDENCIES_H
