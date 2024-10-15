/**************************************************************************
 *	IImportThread.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IImportThread.h"

IImportThread::IImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList) :
	m_ManagerModelRef(managerModelRef),
	m_sImportAssetList(sImportAssetList),
	m_uiBankId(uiBankId),
	m_CorrespondingParentList(correspondingParentList),
	m_CorrespondingUuidList(correspondingUuidList)
{
}

/*virtual*/ IImportThread::~IImportThread()
{
}

/*virtual*/ void IImportThread::run() /*override*/
{
	QString sReport;
	if(OnRun(sReport)) // Must call RegisterAsset() on each asset. Should emit ImportUpdate() throughout OnRun. Allocate newly imported assets to 'm_NewlyImportedAssetList'. Optionally write to sReport if abnormality, returns whether import has occured or entirely canceled
		Q_EMIT ImportIsFinished(true, sReport);
	else
		Q_EMIT ImportIsFinished(false, sReport);
}
