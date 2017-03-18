/**************************************************************************
 *	ItemAudio.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AudioItem.h"
#include "AudioWidget.h"


AudioItem::AudioItem(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal) : IProjItem(pItemProj, ITEM_Audio, sPrefix, sName)
{
}

/*virtual*/ AudioItem::~AudioItem()
{
}

/*virtual*/ void AudioItem::OnGiveMenuActions(QMenu *pMenu)
{
}

/*virtual*/ void AudioItem::OnLink(AtlasFrame *pFrame)
{
}

/*virtual*/ void AudioItem::OnUnlink(AtlasFrame *pFrame)
{
}

/*virtual*/ QJsonValue AudioItem::OnSave()
{
    return QJsonObject();
}

