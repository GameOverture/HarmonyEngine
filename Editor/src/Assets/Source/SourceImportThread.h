/**************************************************************************
 *	SourceImportThread.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2023 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef SourceImportThread_H
#define SourceImportThread_H

#include "IImportThread.h"

class SourceImportThread : public IImportThread
{
	Q_OBJECT

public:
	SourceImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList);
	virtual ~SourceImportThread();

	virtual bool OnRun(QString &sReportOut) override;
};

#endif // SourceImportThread_H
