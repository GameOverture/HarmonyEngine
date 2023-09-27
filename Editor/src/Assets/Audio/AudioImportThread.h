/**************************************************************************
 *	AudioImportThread.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AudioImportThread_H
#define AudioImportThread_H

#include "IImportThread.h"

class AudioImportThread : public IImportThread
{
	Q_OBJECT

public:
	AudioImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList);
	virtual ~AudioImportThread();

	virtual QString OnRun() override;
};

#endif // AudioImportThread_H
