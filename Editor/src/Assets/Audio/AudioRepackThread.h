/**************************************************************************
 *	AudioRepackThread.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2020 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef AUDIOREPACKTHREAD_H
#define AUDIOREPACKTHREAD_H

#include "IRepackThread.h"
#include "SoundClip.h"

class AudioRepackThread : public IRepackThread
{
	Q_OBJECT

public:
	AudioRepackThread(QMap<BankData *, QSet<IAssetItemData *>> &affectedAssetsMapRef, QDir metaDir);
	virtual ~AudioRepackThread();

	virtual void OnRun() override;

private:
	bool PackToWav(SoundClip *pAudio, QDir runtimeBankDir);
	bool PackToOgg(SoundClip *pAudio, QDir runtimeBankDir);
};

#endif // AUDIOREPACKTHREAD_H
