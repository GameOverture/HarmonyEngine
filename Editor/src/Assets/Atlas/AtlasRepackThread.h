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
#include "AtlasPacker.h"

class AtlasRepackThread : public IRepackThread
{
	Q_OBJECT

	struct RepackBank
	{
		BankData *										m_pBankData;

		struct PackerBucket
		{
			AtlasPacker									m_Packer;
			QList<AtlasFrame *>							m_FramesList;
		};
		QMap<std::pair<uint64, uint32>, PackerBucket *>	m_BucketMap;	// Within each bank, there can be multiple buckets of textures. Each bucket has a unique std::pair<uint64, uint32> (specifying the image type, texture filtering, etc)

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
	QSize ConstructAtlasTexture(BankData *pBankData, AtlasPacker &atlasPackerRef, HyImageInfo imageInfo, HyTextureInfo textureInfo, int iPackerBinIndex, int iActualTextureIndex);
};

#endif // ATLASREPACKTHREAD_H
