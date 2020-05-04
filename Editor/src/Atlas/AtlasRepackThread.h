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

#include <QThread>
#include <QDir>

class AtlasRepackThread : public QThread
{
	Q_OBJECT

	AtlasGrp *			m_pAtlasGrp;
	QList<int>			m_TextureIndexList;
	QList<AtlasFrame *> m_NewFramesList;
	QDir				m_MetaDir;

public:
	AtlasRepackThread(AtlasGrp *pAtlasGrp, QList<int> textureIndexList, QList<AtlasFrame *>newFramesList, QDir metaDir);
	virtual ~AtlasRepackThread();

	virtual void run() override;

	void ConstructAtlasTexture(int iPackerBinIndex, int iActualTextureIndex);

Q_SIGNALS:
	void LoadUpdate(QString sMsg, int iPercComplete);
	void RepackIsFinished();
};

#endif // ATLASREPACKTHREAD_H
