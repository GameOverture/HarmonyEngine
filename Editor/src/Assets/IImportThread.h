/**************************************************************************
 *	IImportThread.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2023 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IImportThread_H
#define IImportThread_H

#include <QThread>

class IManagerModel;
class TreeModelItemData;
class IAssetItemData;

class IImportThread : public QThread
{
	Q_OBJECT

protected:
	IManagerModel &					m_ManagerModelRef;

	QStringList						m_sImportAssetList;
	quint32							m_uiBankId;
	QVector<TreeModelItemData *>	m_CorrespondingParentList;
	QVector<QUuid>					m_CorrespondingUuidList;

public:
	IImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QVector<TreeModelItemData *> correspondingParentList, QVector<QUuid> correspondingUuidList);
	virtual ~IImportThread();

	virtual void run() override;
	virtual bool OnRun(QString &sReportOut) = 0; // Must call RegisterAsset() on each asset. Should emit ImportUpdate() throughout OnRun. Allocate newly imported assets to 'm_NewlyImportedAssetList'. Optionally write to sReport if abnormality, returns whether import has occured or entirely canceled

Q_SIGNALS:
	void ImportUpdate(int iLoadedBlocks, int iTotalBlocks);
	void ImportIsFinished(bool bImportOccured, QString sMsg);
};

#endif // IImportThread_H
