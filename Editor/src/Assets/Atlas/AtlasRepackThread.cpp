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

#include "vendor/SOIL2/src/SOIL2/SOIL2.h"

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
			uint32 uiKey = pAtlasFrame->GetTextureInfo().GetBucketId();

			if(curBankRef.m_BucketMap.contains(uiKey) == false)
				curBankRef.m_BucketMap.insert(uiKey, new RepackBank::PackerBucket());

			curBankRef.m_BucketMap[uiKey]->m_FramesList.append(pAtlasFrame);
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
		QMap<uint32, RepackBank::PackerBucket *> &bucketMapRef = m_RepackBankList[iBankCnt].m_BucketMap;

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
		QMap<uint32, RepackBank::PackerBucket *> &bucketMapRef = m_RepackBankList[iBankCnt].m_BucketMap;
		QSize fullAtlasSize(pBankData->m_MetaObj["maxWidth"].toInt(), pBankData->m_MetaObj["maxHeight"].toInt());

		// Go through the packer's bins and ensure textures have a sequential index name
		QDir runtimeBankDir(pBankData->m_sAbsPath);
		QList<int> unfilledTextureIndexList; // Keep track of texture indexes that aren't full size, to be used in next Repack (because it has more room remaining)
		for(auto iter = bucketMapRef.begin(); iter != bucketMapRef.end(); ++iter)
		{
			const int iNUM_NEW_TEXTURES = iter.value()->m_Packer.GetNumBins();

			// Grab 'existingTexturesInfoList' - This is after AtlasModel::OnFlushRepack() has deleted the obsolete textures
			QFileInfoList existingTexturesInfoList = runtimeBankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

			// Using our stock of newly generated textures, fill in any gaps in the texture array.
			// If there aren't enough new textures then shift textures (and their frames) to fill any remaining gaps in the indices.
			const int iTOTAL_NUM_TEXTURES = iNUM_NEW_TEXTURES + existingTexturesInfoList.size();

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
					QSize textureSize = ConstructAtlasTexture(pBankData, iter.value()->m_Packer, HyTextureInfo(iter.key()), iNumNewTexturesUsed, iCurrentIndex);
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

QSize AtlasRepackThread::ConstructAtlasTexture(BankData *pBankData, AtlasPacker &atlasPackerRef, HyTextureInfo texInfo, int iPackerBinIndex, int iActualTextureIndex)
{
	QSize textureSize = atlasPackerRef.GetBinDimensions(iPackerBinIndex);

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
									 packFrameRef.pos.y() + frameMargins.top,
									 textureSize);

		QPoint pos(pFrame->GetX(), pFrame->GetY());
		p.drawImage(pos.x(), pos.y(), QImage(packFrameRef.path), packFrameRef.crop.x(), packFrameRef.crop.y(), packFrameRef.crop.width(), packFrameRef.crop.height());
	}

	QImage *pTexture = static_cast<QImage *>(p.device());
	QDir runtimeBankDir(pBankData->m_sAbsPath);

	switch(texInfo.GetFormat())
	{
	case HYTEXTURE_Uncompressed:
		// Param1: num channels
		// Param2: disk file type (PNG, ...)
		switch(texInfo.m_uiFormatParam2)
		{
		case HyTextureInfo::UNCOMPRESSEDFILE_PNG:
			if(false == pTexture->save(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % texInfo.GetFileExt().c_str())))
				HyGuiLog("AtlasModel::ConstructAtlasTexture failed to generate a PNG atlas", LOGTYPE_Error);
			break;

		default:
			HyGuiLog("AtlasModel::ConstructAtlasTexture unknown uncompressed file type", LOGTYPE_Error);
			break;
		}
		break;

	case HYTEXTURE_DXT: {
		// Param1: num channels
		// Param2: DXT format (1,3,5)
		QImage imgProperlyFormatted = pTexture->convertToFormat(texInfo.m_uiFormatParam1 == 4 ? QImage::Format_RGBA8888 : QImage::Format_RGB888);
		if(0 == SOIL_save_image_quality(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % texInfo.GetFileExt().c_str()).toStdString().c_str(),
										SOIL_SAVE_TYPE_DDS,
										imgProperlyFormatted.width(),
										imgProperlyFormatted.height(),
										texInfo.m_uiFormatParam1,
										imgProperlyFormatted.bits(),
										0))
		{
			HyGuiLog("AtlasModel::ConstructAtlasTexture failed to generate a DTX5 atlas", LOGTYPE_Error);
		}
		break; }

	case HYTEXTURE_ASTC: {
		// Param1: Block Size index (4x4 -> 12x12)
		// Param2: Color Profile (LDR linear, LDR sRGB, HDR RGB, HDR RGBA)
		QString sProgramPath = MainWindow::EngineSrcLocation() % HYGUIPATH_AstcEncDir;
#if defined(Q_OS_WIN)
		sProgramPath += "win/astcenc-sse2.exe";
#elif defined(Q_OS_LINUX)
		sProgramPath += "linux/astcenc-sse2";
#else
		HyGuiLog("ASTC Encoder not found for this platform", LOGTYPE_Error);
#endif

		QStringList sArgList;
		switch(texInfo.m_uiFormatParam2)
		{
		case 0: sArgList << "-cl"; break; // LDR linear
		case 1: sArgList << "-cs"; break; // LDR sRGB
		case 2: sArgList << "-ch"; break; // HDR RGB
		case 3: sArgList << "-cH"; break; // HDR RGBA
		default:
			HyGuiLog("Invalid ASTC Encoder color profile", LOGTYPE_Error);
			break;
		}

		// Create temp PNG file to be used
		QString sTempTexturePath = runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % ".png");
		QString sAstcTexturePath = runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % texInfo.GetFileExt().c_str());
		pTexture->save(sTempTexturePath);

		sArgList << sTempTexturePath;
		sArgList << sAstcTexturePath;

		switch(texInfo.m_uiFormatParam1)
		{
		case 0:  sArgList << "4x4"; break;   // 8.00 bpp
		case 1:  sArgList << "5x4"; break;   // 6.40 bpp
		case 2:  sArgList << "5x5"; break;   // 5.12 bpp
		case 3:  sArgList << "6x5"; break;   // 4.27 bpp
		case 4:  sArgList << "6x6"; break;   // 3.56 bpp
		case 5:  sArgList << "8x5"; break;   // 3.20 bpp
		case 6:  sArgList << "8x6"; break;   // 2.67 bpp
		case 7:  sArgList << "10x5"; break;  // 2.56 bpp
		case 8:  sArgList << "10x6"; break;  // 2.13 bpp
		case 9:  sArgList << "8x8"; break;   // 2.00 bpp
		case 10: sArgList << "10x8"; break;  // 1.60 bpp
		case 11: sArgList << "10x10"; break; // 1.28 bpp
		case 12: sArgList << "12x10"; break; // 1.07 bpp
		case 13: sArgList << "12x12"; break; // 0.89 bpp
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
		break; }

	default:
		HyGuiLog("AtlasModel::ConstructAtlasTexture tried to create an unsupported texture type: " % QString::number(texInfo.GetFormat()), LOGTYPE_Error);
		break;
	}

	return textureSize;
}
