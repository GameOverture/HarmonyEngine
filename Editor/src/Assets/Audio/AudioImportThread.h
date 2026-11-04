/**************************************************************************
 *	AudioImportThread.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2023 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef AudioImportThread_H
#define AudioImportThread_H

#include "IImportThread.h"

class AudioImportThread : public IImportThread
{
	Q_OBJECT

public:
	AudioImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList);
	virtual ~AudioImportThread();

	virtual bool OnRun(QString &sReportOut) override;
};

#endif // AudioImportThread_H
