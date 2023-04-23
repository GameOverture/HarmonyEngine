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

	struct RepackBank
	{
		BankData *						m_pBankData;

		struct PackerBucket
		{
			ImagePacker					m_Packer;

			//QSet<int>					m_TextureIndexSet;
			QList<AtlasFrame *>			m_FramesList;
		};
		QMap<uint32, PackerBucket *>	m_BucketMap;

		RepackBank() :
			m_pBankData(nullptr)
		{ }

		~RepackBank() {
			for(auto iter = m_BucketMap.begin(); iter != m_BucketMap.end(); ++iter)
				delete iter.value(); // Deletes dynamically allocated PackerBucker *
		}
	};
	QList<RepackBank>					m_RepackBankList;

public:
	AtlasRepackThread(QMap<BankData *, QSet<IAssetItemData *>> &affectedAssetsMapRef, QDir metaDir);
	virtual ~AtlasRepackThread();

	virtual void OnRun() override;

	//void RepackBank(BankData *pBankData, 

private:
	void ConstructAtlasTexture(BankData *pBankData, ImagePacker &imagePackerRef, HyTextureInfo texInfo, int iPackerBinIndex, int iActualTextureIndex);
	void SetPackerSettings(BankData *pBankData, ImagePacker &imagePackerRef);
};

#endif // ATLASREPACKTHREAD_H
