/**************************************************************************
 *	IRepackThread.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2020 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef IREPACKTHREAD_H
#define IREPACKTHREAD_H

#include "BanksModel.h"

#include <QThread>
#include <QDir>

class IRepackThread : public QThread
{
	Q_OBJECT

protected:
	QMap<BankData *, QSet<IAssetItemData *>> &	m_AffectedAssetsMapRef;
	QDir										m_MetaDir;

public:
	IRepackThread(QMap<BankData *, QSet<IAssetItemData *>> &affectedAssetsMapRef, QDir metaDir);
	virtual ~IRepackThread();

	virtual void run() override;
	virtual void OnRun() = 0;

Q_SIGNALS:
	void RepackUpdate(int iLoadedBlocks, int iTotalBlocks);
	void RepackIsFinished();
};

#endif // IREPACKTHREAD_H
