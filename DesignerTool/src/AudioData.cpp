/**************************************************************************
 *	ItemAudio.cpp
 *
 *	Harmony Engine - Designer Tool
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "AudioData.h"
#include "AudioWidget.h"


AudioData::AudioData(Project *pItemProj, const QString sPrefix, const QString sName, QJsonValue initVal) : IData(pItemProj, ITEM_Audio, sPrefix, sName, initVal)
{
}

/*virtual*/ AudioData::~AudioData()
{
}

/*virtual*/ void AudioData::OnGiveMenuActions(QMenu *pMenu)
{
}

/*virtual*/ void AudioData::OnGuiLoad(IHyApplication &hyApp)
{
    m_pWidget = new AudioWidget(this);
}

/*virtual*/ void AudioData::OnGuiUnload(IHyApplication &hyApp)
{
    delete m_pWidget;
}

/*virtual*/ void AudioData::OnGuiShow(IHyApplication &hyApp)
{
}

/*virtual*/ void AudioData::OnGuiHide(IHyApplication &hyApp)
{
}

/*virtual*/ void AudioData::OnGuiUpdate(IHyApplication &hyApp)
{
}

/*virtual*/ void AudioData::OnLink(AtlasFrame *pFrame)
{
}

/*virtual*/ void AudioData::OnReLink(AtlasFrame *pFrame)
{
}

/*virtual*/ void AudioData::OnUnlink(AtlasFrame *pFrame)
{
}

/*virtual*/ QJsonValue AudioData::OnSave()
{
    return QJsonObject();
}

