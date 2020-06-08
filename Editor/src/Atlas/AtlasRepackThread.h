/**************************************************************************
 *	AtlasRepackThread.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef ATLASREPACKTHREAD_H
#define ATLASREPACKTHREAD_H

#include "AtlasModel.h"
#include "_Dependencies/scriptum/imagepacker.h"

#include <QThread>
#include <QDir>

class AtlasRepackThread : public QThread
{
	Q_OBJECT

	BankData &			m_BankRef;
	ImagePacker			m_Packer;

	QList<int>			m_TextureIndexList;
	QList<AtlasFrame *> m_NewFramesList;
	QDir				m_MetaDir;

public:
	AtlasRepackThread(BankData &bankRef, QList<int> textureIndexList, QList<AtlasFrame *>newFramesList, QDir metaDir);
	virtual ~AtlasRepackThread();

	virtual void run() override;

	void ConstructAtlasTexture(int iPackerBinIndex, int iActualTextureIndex);

Q_SIGNALS:
	void LoadUpdate(QString sMsg, int iPercComplete);
	void RepackIsFinished();

private:
	void SetPackerSettings();
};

#endif // ATLASREPACKTHREAD_H
