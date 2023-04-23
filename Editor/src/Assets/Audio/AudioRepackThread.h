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

public:
	AudioRepackThread(QMap<BankData *, QSet<IAssetItemData *>> &affectedAssetsMapRef, QDir metaDir);
	virtual ~AudioRepackThread();

	virtual void OnRun() override;

private:
	bool PackToWav(AudioAsset *pAudio, QDir runtimeBankDir);
	bool PackToOgg(AudioAsset *pAudio, QDir runtimeBankDir);
};

#endif // AUDIOREPACKTHREAD_H
