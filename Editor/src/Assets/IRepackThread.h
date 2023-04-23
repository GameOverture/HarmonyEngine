/**************************************************************************
 *	IRepackThread.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
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
	void LoadUpdate(QString sMsg, int iPercComplete);
	void RepackIsFinished();
};

#endif // IREPACKTHREAD_H
