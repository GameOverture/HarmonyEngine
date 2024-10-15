/**************************************************************************
 *	AtlasImportThread.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef AtlasImportThread_H
#define AtlasImportThread_H

#include "IImportThread.h"

class AtlasImportThread : public IImportThread
{
	Q_OBJECT

public:
	AtlasImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList);
	virtual ~AtlasImportThread();

	virtual bool OnRun(QString &sReportOut) override;
};

#endif // AtlasImportThread_H
