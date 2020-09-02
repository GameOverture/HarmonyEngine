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

#include "IRepackThread.h"
#include "AtlasFrame.h"
#include "_Dependencies/scriptum/imagepacker.h"

class AtlasRepackThread : public IRepackThread
{
	Q_OBJECT

	BankData &			m_BankRef;
	ImagePacker			m_Packer;

	QList<int>			m_TextureIndexList;
	QList<AtlasFrame *> m_NewFramesList;

public:
	AtlasRepackThread(BankData &bankRef, QList<int> textureIndexList, QList<AtlasFrame *>newFramesList, QDir metaDir);
	virtual ~AtlasRepackThread();

	virtual void OnRun() override;

private:
	void ConstructAtlasTexture(int iPackerBinIndex, int iActualTextureIndex);
	void SetPackerSettings();
};

#endif // ATLASREPACKTHREAD_H
