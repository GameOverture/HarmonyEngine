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

class AudioRepackThread : public IRepackThread
{
	Q_OBJECT

public:
	AudioRepackThread(BankData &bankRef, QDir metaDir);
	virtual ~AudioRepackThread();

	virtual void OnRun() override;

private:
	bool PackToOgg(QString sWavFilePath, QString sOggFilePath, uint16 uiNumChannels, float fVbrQuality);
};

#endif // AUDIOREPACKTHREAD_H
