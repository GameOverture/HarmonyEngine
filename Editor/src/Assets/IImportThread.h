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
	IManagerModel &				m_ManagerModelRef;

	QStringList					m_sImportAssetList;
	quint32						m_uiBankId;
	QList<TreeModelItemData *>	m_CorrespondingParentList;
	QList<QUuid>				m_CorrespondingUuidList;

public:
	IImportThread(IManagerModel &managerModelRef, QStringList sImportAssetList, quint32 uiBankId, QList<TreeModelItemData *> correspondingParentList, QList<QUuid> correspondingUuidList);
	virtual ~IImportThread();

	virtual void run() override;
	virtual QString OnRun() = 0; // Must call RegisterAsset() on each asset. Should emit ImportUpdate() throughout OnRun. Allocate newly imported assets to 'm_NewlyImportedAssetList'. If sCancelReason is empty, then the import was successful

Q_SIGNALS:
	void ImportUpdate(int iLoadedBlocks, int iTotalBlocks);
	void ImportCanceled(QString sMsg);
	void ImportIsFinished();
};

#endif // IImportThread_H
