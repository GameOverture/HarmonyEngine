/**************************************************************************
 *	IRepackThread.cpp
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2020 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IRepackThread.h"

IRepackThread::IRepackThread(QMap<BankData *, QSet<IAssetItemData *>> &affectedAssetsMapRef, QDir metaDir) :
	m_AffectedAssetsMapRef(affectedAssetsMapRef),
	m_MetaDir(metaDir)
{
}

/*virtual*/ IRepackThread::~IRepackThread()
{
}

/*virtual*/ void IRepackThread::run() /*override*/
{
	OnRun(); // Should emit LoadUpdate() throughout OnRun
	Q_EMIT RepackIsFinished();
}
