/**************************************************************************
 *	AudioRepackThread.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AUDIOREPACKTHREAD_H
#define AUDIOREPACKTHREAD_H

#include "IRepackThread.h"
#include "AudioAsset.h"

class AudioRepackThread : public IRepackThread
{
	Q_OBJECT

	QList<QPair<BankData *, QSet<AudioAsset *>>>		m_AffectedAssetsList;

public:
	AudioRepackThread(QList<QPair<BankData *, QSet<AudioAsset *>>> affectedAssetsList, QDir metaDir);
	virtual ~AudioRepackThread();

	virtual void OnRun() override;

private:
	bool PackToWav(AudioAsset *pAudio, QDir runtimeBankDir);
	bool PackToOgg(AudioAsset *pAudio, QDir runtimeBankDir);
};

#endif // AUDIOREPACKTHREAD_H
