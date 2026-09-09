/**************************************************************************
 *	AtlasRepackThread.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "AtlasRepackThread.h"
#include "MainWindow.h"

#include <QPainter>
#include <QImageWriter>
#include <QProcess>

AtlasRepackThread::AtlasRepackThread(QMap<BankData *, QSet<IAssetItemData *>> &affectedAssetsMapRef, QDir metaDir) :
	IRepackThread(affectedAssetsMapRef, metaDir)
{
	for(auto iter = m_AffectedAssetsMapRef.begin(); iter != m_AffectedAssetsMapRef.end(); ++iter)
	{
		BankData *pBankData = iter.key();
		QSet<IAssetItemData *> &affectedAssetsSet = iter.value();

		m_RepackBankList.push_back(RepackBank());
		RepackBank &curBankRef = m_RepackBankList.back();
		
		curBankRef.m_pBankData = pBankData;

		// Organize all affected frames into buckets (HyTextureInfo's bucket ID)
		QList<IAssetItemData *>affectedFramesList = affectedAssetsSet.values();
		for(int i = 0; i < affectedFramesList.size(); ++i)
		{
			AtlasFrame *pAtlasFrame = static_cast<AtlasFrame *>(affectedFramesList[i]);
			HyImageInfo imgInfo = pAtlasFrame->GetImageInfo();
			imgInfo.SetWidth(0);
			imgInfo.SetHeight(0);
			std::pair<uint64, uint32> uiKeyPair(imgInfo.GetBucketId(), pAtlasFrame->GetTextureInfo().GetBucketId());

			if(curBankRef.m_BucketMap.contains(uiKeyPair) == false)
				curBankRef.m_BucketMap.insert(uiKeyPair, new RepackBank::PackerBucket());

			curBankRef.m_BucketMap[uiKeyPair]->m_FramesList.append(pAtlasFrame);
		}
	}
}

/*virtual*/ AtlasRepackThread::~AtlasRepackThread()
{
}

/*virtual*/ void AtlasRepackThread::OnRun() /*override*/
{
	// These 'blocks' are just used to update the progress bar
	int iLoadedBlocks = 0;
	int iTotalBlocks = 0;

	// First go over and run the packer, to determine how many textures will be loaded
	for(int32 iBankCnt = 0; iBankCnt < m_RepackBankList.size(); ++iBankCnt)
	{
		BankData *pBankData = m_RepackBankList[iBankCnt].m_pBankData;
		QMap<std::pair<uint64, uint32>, RepackBank::PackerBucket *> &bucketMapRef = m_RepackBankList[iBankCnt].m_BucketMap;

		// Run image packer on each bucket's m_FramesList
		for(auto iter = bucketMapRef.begin(); iter != bucketMapRef.end(); ++iter)
		{
			iter.value()->m_Packer.ClearFrames();

			// Repack the affected frames and determine how many textures this repack took
			for(int i = 0; i < iter.value()->m_FramesList.size(); ++i)
			{
				QRect alphaCropRect(iter.value()->m_FramesList[i]->GetCropL(),
									iter.value()->m_FramesList[i]->GetCropT(),
									iter.value()->m_FramesList[i]->GetCroppedWidth(),
									iter.value()->m_FramesList[i]->GetCroppedHeight());

				iter.value()->m_Packer.AddFrame(iter.value()->m_FramesList[i]->GetSize(),
												alphaCropRect,
												iter.value()->m_FramesList[i]->GetChecksum(),
												iter.value()->m_FramesList[i],
												m_MetaDir.absoluteFilePath(iter.value()->m_FramesList[i]->ConstructMetaFileName()));
			}

			iter.value()->m_Packer.PackFramesToBins(pBankData);

			iTotalBlocks += iter.value()->m_Packer.GetNumBins();
		}
	}

	for(int32 iBankCnt = 0; iBankCnt < m_RepackBankList.size(); ++iBankCnt)
	{
		BankData *pBankData = m_RepackBankList[iBankCnt].m_pBankData;
		QMap<std::pair<uint64, uint32>, RepackBank::PackerBucket *> &bucketMapRef = m_RepackBankList[iBankCnt].m_BucketMap;
		QSize fullAtlasSize(pBankData->m_MetaObj["maxWidth"].toInt(), pBankData->m_MetaObj["maxHeight"].toInt());

		// Go through the packer's bins and ensure textures have a sequential index name
		QDir runtimeBankDir(pBankData->m_sAbsPath);
		QList<int> unfilledTextureIndexList; // Keep track of texture indexes that aren't full size, to be used in next Repack (because it has more room remaining)
		for(auto iter = bucketMapRef.begin(); iter != bucketMapRef.end(); ++iter)
		{
			const int iNUM_NEW_TEXTURES = iter.value()->m_Packer.GetNumBins();

			// Grab 'existingTexturesInfoList' - This is after AtlasManager::OnFlushRepack() has deleted the obsolete textures
			QFileInfoList existingTexturesInfoList = runtimeBankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

			// Using our stock of newly generated textures, fill in any gaps in the texture array.
			// If there aren't enough new textures then shift textures (and their frames) to fill any remaining gaps in the indices.
			const int iTOTAL_NUM_TEXTURES = iNUM_NEW_TEXTURES + existingTexturesInfoList.size();

			// TODO: "textureSizes" isn't getting cleared out, so if a previous pack had more textures than this one, old/stale sizes will be left in the "textureSizes" array

			int iNumNewTexturesUsed = 0;
			int iCurrentIndex = 0;
			for(; iCurrentIndex < iTOTAL_NUM_TEXTURES; ++iCurrentIndex)
			{
				bool bFound = false;
				for(int i = 0; i < existingTexturesInfoList.size(); ++i)
				{
					if(existingTexturesInfoList[i].baseName().toInt() == iCurrentIndex)
					{
						bFound = true;
						break;
					}
				}
				if(bFound)
					continue;

				if(iNumNewTexturesUsed < iNUM_NEW_TEXTURES)
				{
					QSize textureSize = ConstructAtlasTexture(pBankData, iter.value()->m_Packer, HyImageInfo(iter.key().first), HyTextureIn(iter.key().second), iNumNewTexturesUsed, iCurrentIndex);
					iNumNewTexturesUsed++;

					iLoadedBlocks++;
					Q_EMIT RepackUpdate(iLoadedBlocks, iTotalBlocks);

					// If texture size isn't 'fullAtlasSize' then indicate it as "unfilled"
					if(textureSize != fullAtlasSize)
						unfilledTextureIndexList.append(iCurrentIndex);

					// Update the textureSizes array in the meta file
					QJsonArray textureSizesArray = pBankData->m_MetaObj["textureSizes"].toArray();
					while(textureSizesArray.size() <= iCurrentIndex)
					{
						QJsonArray tmpArray;
						tmpArray.append(-1); tmpArray.append(-1);
						textureSizesArray.append(tmpArray);
					}
					QJsonArray newTexSizeArray;
					newTexSizeArray.append(textureSize.width());
					newTexSizeArray.append(textureSize.height());
					textureSizesArray[iCurrentIndex] = newTexSizeArray;
					pBankData->m_MetaObj["textureSizes"] = textureSizesArray;
				}
				else
				{
					// There aren't enough new textures to fill all the gaps in indices. Start shifting assets into the next texture index.
					// Find the next existing texture and assign it to iCurrentIndex
					bool bHandled = false;
					int iNextAvailableFoundIndex = iCurrentIndex;
					do
					{
						++iNextAvailableFoundIndex;
						for(int i = 0; i < existingTexturesInfoList.size(); ++i)
						{
							int iExistingTextureIndex = existingTexturesInfoList[i].baseName().toInt();

							if(iExistingTextureIndex == iNextAvailableFoundIndex)
							{
								// Get the existing texture's size
								QJsonArray textureSizesArray = pBankData->m_MetaObj["textureSizes"].toArray();
								QJsonArray existingTexSizeArray = textureSizesArray[iExistingTextureIndex].toArray();
								QSize textureSize(existingTexSizeArray[0].toInt(), existingTexSizeArray[1].toInt());
								
								// Texture found, start migrating its frames
								QList<IAssetItemData *> &atlasGrpFrameListRef = pBankData->m_AssetList;
								for(int j = 0; j < atlasGrpFrameListRef.size(); ++j)
								{
									AtlasFrame *pFrame = static_cast<AtlasFrame *>(atlasGrpFrameListRef[j]);
									if(pFrame->GetTextureIndex() == iExistingTextureIndex)
										pFrame->UpdateInfoFromPacker(iCurrentIndex, pFrame->GetX(), pFrame->GetY(), textureSize);
								}

								// Rename the texture file to be the new index
								QFile::rename(existingTexturesInfoList[i].absoluteFilePath(), runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iCurrentIndex) % "." % existingTexturesInfoList[i].completeSuffix()));

								// Update the textureSizes array in the meta file
								textureSizesArray[iCurrentIndex] = existingTexSizeArray;
								pBankData->m_MetaObj["textureSizes"] = textureSizesArray;

								// Regrab 'existingTexturesInfoList' after renaming a texture
								existingTexturesInfoList = runtimeBankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

								bHandled = true;
								break;
							}
						}
					} while(bHandled == false);
				}
			}

			// Correct all the duplicate frames
			for(int i = 0; i < iter.value()->m_Packer.GetNumFrames(); ++i)
			{
				PackFrame &packFrameRef = iter.value()->m_Packer.GetPackFrame(i);
				if(packFrameRef.duplicateId != nullptr)
				{
					AtlasFrame *pFrame = packFrameRef.id;
					AtlasFrame *pDupFrame = packFrameRef.duplicateId;

					QSize textureSize = iter.value()->m_Packer.GetBinDimensions(packFrameRef.textureId);
					pFrame->UpdateInfoFromPacker(pDupFrame->GetTextureIndex(), pDupFrame->GetX(), pDupFrame->GetY(), textureSize);
				}
			}
		}

		// Replace "unfilledIndices" with all the buckets' last (least packed) bin
		QJsonArray unfilledIndicesArray;
		for(auto idx : unfilledTextureIndexList)
			unfilledIndicesArray.append(idx);
		pBankData->m_MetaObj["unfilledIndices"] = unfilledIndicesArray;
	}
}

QSize AtlasRepackThread::ConstructAtlasTexture(BankData *pBankData, AtlasPacker &atlasPackerRef, HyImageInfo imageInfo, HyTextureIn textureInfo, int iPackerBinIndex, int iActualTextureIndex)
{
	QSize textureSize = atlasPackerRef.GetBinDimensions(iPackerBinIndex);
	imageInfo.SetWidth(textureSize.width());
	imageInfo.SetHeight(textureSize.height());

	QImage newTexture(textureSize.width(), textureSize.height(), QImage::Format_ARGB32);
	newTexture.fill(Qt::transparent);

	QPainter p(&newTexture);

	// Iterate through the images that were packed, and update their corresponding AtlasFrame. Then draw them to the blank textures
	for(int i = 0; i < atlasPackerRef.GetNumFrames(); ++i)
	{
		PackFrame &packFrameRef = atlasPackerRef.GetPackFrame(i);
		AtlasFrame *pFrame = packFrameRef.id;
		bool bValidToDraw = true;

		if(packFrameRef.pos.x() == 999999) // This is scriptum image packer's magic number to indicate an invalid image...
		{
			pFrame->UpdateInfoFromPacker(-1, -1, -1, QSize(-1, -1));
			bValidToDraw = false;
		}
		else
			pFrame->ClearError(ASSETERROR_CouldNotPack);

		if(packFrameRef.duplicateId != nullptr)
			bValidToDraw = false;

		if(packFrameRef.textureId != iPackerBinIndex)
			bValidToDraw = false;

		if(bValidToDraw == false)
			continue;

		HyMargins<int> frameMargins = atlasPackerRef.GetFrameMargins();
		pFrame->UpdateInfoFromPacker(iActualTextureIndex,
									 packFrameRef.pos.x() + frameMargins.left,
									 textureSize.height() - packFrameRef.pos.y() - packFrameRef.crop.height() - frameMargins.top, // NOTE: Packer uses top-left for position, but we store it bottom-left
									 textureSize);

		QPoint pos(pFrame->GetX(), packFrameRef.pos.y() + frameMargins.top);
		p.drawImage(pos.x(), pos.y(), QImage(packFrameRef.path), packFrameRef.crop.x(), packFrameRef.crop.y(), packFrameRef.crop.width(), packFrameRef.crop.height());
	}

	QImage *pTexture = static_cast<QImage *>(p.device());
	QDir runtimeBankDir(pBankData->m_sAbsPath);

	if(imageInfo.GetType() != HYIMAGE_ASTC)
	{
		std::string sFilePath = runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % HyImageInfo::GetExt(imageInfo.GetType()).c_str()).toStdString();
		switch(imageInfo.GetNumChannels())
		{
		case 4:
			HyIO::WriteImage(sFilePath, imageInfo, pTexture->convertToFormat(QImage::Format_RGBA8888).bits());
			break;
		case 3:
			if(imageInfo.GetType() == HYIMAGE_DDS) // NOTE: HyIO::WriteImage for DDS files require its data to be padded to 4 bytes even when writing RGB DXT1
				HyIO::WriteImage(sFilePath, imageInfo, pTexture->convertToFormat(QImage::Format_RGBX8888).bits());
			else
				HyIO::WriteImage(sFilePath, imageInfo, pTexture->convertToFormat(QImage::Format_RGB888).bits());
			break;
		default:
			HyGuiLog("AtlasManager::ConstructAtlasTexture - Invalid number of color channels", LOGTYPE_Error);
			break;
		}
	}
	else // Saving as ASTC
	{
		QString sProgramPath = MainWindow::EngineSrcLocation() % HYGUIPATH_AstcEncDir;
#if defined(Q_OS_WIN)
		sProgramPath += "win/astcenc-sse2.exe";
#elif defined(Q_OS_LINUX)
		sProgramPath += "linux/astcenc-sse2";
#else
		HyGuiLog("ASTC Encoder not found for this platform", LOGTYPE_Error);
#endif

		QStringList sArgList;
		switch(imageInfo.GetFormatParam())
		{
		case HYASTC_Linear:			// LDR (Low Dynamic Range) linear color data
			sArgList << "-cl";
			break;
		case HYASTC_Unknown:
			[[fallthrough]];
		case HYASTC_Standard:		// LDR Standard RGBA - Recommended for standard color textures as gamma curve better matches human perception
			sArgList << "-cs";
			break;
		case HYASTC_HDR_LinearA:	// HDR (High Dynamic Range) RGB data combined with Low Dynamic Range alpha
			sArgList << "-ch";
			break;
		case HYASTC_HDR_RGBA:		// HDR RGB data combined with High Dynamic Range alpha (usually not needed)
			sArgList << "-cH";
			break;
		default:
			HyGuiLog("Invalid ASTC Encoder color profile", LOGTYPE_Error);
			break;
		}

		// Create temp PNG file to be used
		QString sTempTexturePath = runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % ".png");
		QString sAstcTexturePath = runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % HyImageInfo::GetExt(HYIMAGE_ASTC).c_str());
		pTexture->save(sTempTexturePath);

		sArgList << sTempTexturePath;
		sArgList << sAstcTexturePath;

		switch(imageInfo.GetFormat())
		{
		case HYTEXFORMAT_ASTC_LINEAR_4x4:
		case HYTEXFORMAT_ASTC_sRGB_A8_4x4:
			sArgList << "4x4"; // 8.00 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_5x4:
		case HYTEXFORMAT_ASTC_sRGB_A8_5x4:
			sArgList << "5x4"; // 6.40 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_5x5:
		case HYTEXFORMAT_ASTC_sRGB_A8_5x5:
			sArgList << "5x5"; // 5.12 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_6x5:
		case HYTEXFORMAT_ASTC_sRGB_A8_6x5:
			sArgList << "6x5"; // 4.27 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_6x6:
		case HYTEXFORMAT_ASTC_sRGB_A8_6x6:
			sArgList << "6x6"; // 3.56 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_8x5:
		case HYTEXFORMAT_ASTC_sRGB_A8_8x5:
			sArgList << "8x5"; // 3.20 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_8x6:
		case HYTEXFORMAT_ASTC_sRGB_A8_8x6:
			sArgList << "8x6"; // 2.67 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_10x5:
		case HYTEXFORMAT_ASTC_sRGB_A8_10x5:
			sArgList << "10x5"; // 2.56 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_10x6:
		case HYTEXFORMAT_ASTC_sRGB_A8_10x6:
			sArgList << "10x6"; // 2.13 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_8x8:
		case HYTEXFORMAT_ASTC_sRGB_A8_8x8:
			sArgList << "8x8"; // 2.00 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_10x8:
		case HYTEXFORMAT_ASTC_sRGB_A8_10x8:
			sArgList << "10x8"; // 1.60 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_10x10:
		case HYTEXFORMAT_ASTC_sRGB_A8_10x10:
			sArgList << "10x10"; // 1.28 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_12x10:
		case HYTEXFORMAT_ASTC_sRGB_A8_12x10:
			sArgList << "12x10"; // 1.07 bpp
			break;
		case HYTEXFORMAT_ASTC_LINEAR_12x12:
		case HYTEXFORMAT_ASTC_sRGB_A8_12x12:
			sArgList << "12x12"; // 0.89 bpp
			break;
		default:
			HyGuiLog("Invalid ASTC Encoder block footprint", LOGTYPE_Error);
			break;
		}

		sArgList << "-thorough";

		QProcess astcEncProcess;
		astcEncProcess.start(sProgramPath, sArgList);
		if(!astcEncProcess.waitForStarted())
			HyGuiLog("ASTC Encoder failed to start", LOGTYPE_Error);
		else
		{
			if(!astcEncProcess.waitForFinished(-1))
				HyGuiLog("ASTC Encoder failed while encoding", LOGTYPE_Error);
		}

		// Remove the temp texture
		if(false == QFile::remove(sTempTexturePath))
			HyGuiLog("Could not remove temp PNG texture when encoding into ASTC: " % sTempTexturePath, LOGTYPE_Warning);
	}

	return textureSize;
}
