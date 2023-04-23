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

			//curBankRef.m_BucketMap[uiKey]->m_TextureIndexSet.insert(pAtlasFrame->GetTextureIndex());
			curBankRef.m_BucketMap[uiKey]->m_FramesList.append(pAtlasFrame);
		}
	}
}

/*virtual*/ AtlasRepackThread::~AtlasRepackThread()
{
}

/*virtual*/ void AtlasRepackThread::OnRun() /*override*/
{
	for(int32 iBankCnt = 0; iBankCnt < m_RepackBankList.size(); ++iBankCnt)
	{
		BankData *pBankData = m_RepackBankList[iBankCnt].m_pBankData;
		QMap<uint32, RepackBank::PackerBucket *> &bucketMapRef = m_RepackBankList[iBankCnt].m_BucketMap;

		QDir runtimeBankDir(pBankData->m_sAbsPath);

		// Keep track of the last texture index to be used in next Repack (because it likely has room remaining)
		QList<int> unfilledTextureIndexList;

		// Run image packer on each bucket's m_FramesList, and insert/rename textures to have sequential index name
		for(auto iter = bucketMapRef.begin(); iter != bucketMapRef.end(); ++iter)
		{
			iter.value()->m_Packer.clear();

			// Repack the affected frames and determine how many textures this repack took
			for(int i = 0; i < iter.value()->m_FramesList.size(); ++i)
			{
				iter.value()->m_Packer.addItem(iter.value()->m_FramesList[i]->GetSize(),
					iter.value()->m_FramesList[i]->GetCrop(),
					iter.value()->m_FramesList[i]->GetChecksum(),
					iter.value()->m_FramesList[i],
					m_MetaDir.absoluteFilePath(iter.value()->m_FramesList[i]->ConstructMetaFileName()));
			}

			SetPackerSettings(pBankData, iter.value()->m_Packer);
			QSize fullAtlasSize(pBankData->m_MetaObj["maxWidth"].toInt(), pBankData->m_MetaObj["maxHeight"].toInt());

			iter.value()->m_Packer.pack(pBankData->m_MetaObj["cmbHeuristic"].toInt(), fullAtlasSize.width(), fullAtlasSize.height());

			int iNumNewTextures = iter.value()->m_Packer.bins.size();

			// Grab 'existingTexturesInfoList' after deleting obsolete textures
			QFileInfoList existingTexturesInfoList = runtimeBankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

			// Using our stock of newly generated textures, fill in any gaps in the texture array.
			// If there aren't enough new textures then shift textures (and their frames) to fill any remaining gaps in the indices.
			int iTotalNumTextures = iNumNewTextures + existingTexturesInfoList.size();

			int iNumNewTexturesUsed = 0;
			int iCurrentIndex = 0;
			for(; iCurrentIndex < iTotalNumTextures; ++iCurrentIndex)
			{
				// TODO: Fix this calculation since we use buckets now
				float fPercComplete = (static_cast<float>(iCurrentIndex) / static_cast<float>(iTotalNumTextures));
				fPercComplete *= 100.0f;
				Q_EMIT LoadUpdate("Constructing Atlases", static_cast<int>(fPercComplete));

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

				if(iNumNewTexturesUsed < iNumNewTextures)
				{
					ConstructAtlasTexture(pBankData, iter.value()->m_Packer, HyTextureInfo(iter.key()), iNumNewTexturesUsed, iCurrentIndex);
					iNumNewTexturesUsed++;

					// Store off the last packed texture to indicate it is "unfilled"
					if(iNumNewTexturesUsed == iNumNewTextures)
						unfilledTextureIndexList.append(iCurrentIndex);
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
								// Texture found, start migrating its frames
								QList<IAssetItemData *> &atlasGrpFrameListRef = pBankData->m_AssetList;
								for(int j = 0; j < atlasGrpFrameListRef.size(); ++j)
								{
									AtlasFrame *pFrame = static_cast<AtlasFrame *>(atlasGrpFrameListRef[j]);
									if(pFrame->GetTextureIndex() == iExistingTextureIndex)
										pFrame->UpdateInfoFromPacker(iCurrentIndex, pFrame->GetX(), pFrame->GetY(), fullAtlasSize);
								}

								// Rename the texture file to be the new index
								QFile::rename(existingTexturesInfoList[i].absoluteFilePath(), runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iCurrentIndex) % "." % existingTexturesInfoList[i].completeSuffix()));

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
			for(int i = 0; i < iter.value()->m_Packer.images.size(); ++i)
			{
				inputImage &imgInfoRef = iter.value()->m_Packer.images[i];
				if(imgInfoRef.duplicateId != nullptr)
				{
					AtlasFrame *pFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.id);
					AtlasFrame *pDupFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.duplicateId);

					pFrame->UpdateInfoFromPacker(pDupFrame->GetTextureIndex(), pDupFrame->GetX(), pDupFrame->GetY(), fullAtlasSize);
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

void AtlasRepackThread::ConstructAtlasTexture(BankData *pBankData, ImagePacker &imagePackerRef, HyTextureInfo texInfo, int iPackerBinIndex, int iActualTextureIndex)
{
	if(pBankData->m_MetaObj["maxWidth"].toInt() != imagePackerRef.bins[iPackerBinIndex].width() ||
	   pBankData->m_MetaObj["maxHeight"].toInt() != imagePackerRef.bins[iPackerBinIndex].height())
	{
		HyGuiLog("WidgetAtlasGroup::ConstructAtlasTexture() Mismatching texture dimensions", LOGTYPE_Error);
	}
	QSize fullAtlasSize(pBankData->m_MetaObj["maxWidth"].toInt(), pBankData->m_MetaObj["maxHeight"].toInt());

	QImage newTexture(fullAtlasSize.width(), fullAtlasSize.height(), QImage::Format_ARGB32);
	newTexture.fill(Qt::transparent);

	QPainter p(&newTexture);

	// Iterate through the images that were packed, and update their corresponding AtlasFrame. Then draw them to the blank textures
	for(int i = 0; i < imagePackerRef.images.size(); ++i)
	{
		inputImage &imgInfoRef = imagePackerRef.images[i];
		AtlasFrame *pFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.id);
		bool bValidToDraw = true;

		if(imgInfoRef.pos.x() == 999999) // This is scriptum image packer's magic number to indicate an invalid image...
		{
			pFrame->UpdateInfoFromPacker(-1, -1, -1, fullAtlasSize);
			bValidToDraw = false;
		}
		else
			pFrame->ClearError(ASSETERROR_CouldNotPack);

		if(imgInfoRef.duplicateId != nullptr)
			bValidToDraw = false;

		if(imgInfoRef.textureId != iPackerBinIndex)
			bValidToDraw = false;

		if(bValidToDraw == false)
			continue;

		pFrame->UpdateInfoFromPacker(iActualTextureIndex,
									 imgInfoRef.pos.x() + imagePackerRef.border.l,
									 imgInfoRef.pos.y() + imagePackerRef.border.t,
									 fullAtlasSize);

		QImage imgFrame(imgInfoRef.path);

		QSize size;
		QRect crop;
		if(!imagePackerRef.cropThreshold)
		{
			size = imgInfoRef.size;
			crop = QRect(0, 0, size.width(), size.height());
		}
		else
		{
			size = imgInfoRef.crop.size();
			crop = imgInfoRef.crop;
		}

		QPoint pos(pFrame->GetX(), pFrame->GetY());
		p.drawImage(pos.x(), pos.y(), imgFrame, crop.x(), crop.y(), crop.width(), crop.height());
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
}

void AtlasRepackThread::SetPackerSettings(BankData *pBankData, ImagePacker &imagePackerRef)
{
	imagePackerRef.sortOrder = pBankData->m_MetaObj["cmbSortOrder"].toInt();
	imagePackerRef.border.t = pBankData->m_MetaObj["sbFrameMarginTop"].toInt();
	imagePackerRef.border.l = pBankData->m_MetaObj["sbFrameMarginLeft"].toInt();
	imagePackerRef.border.r = pBankData->m_MetaObj["sbFrameMarginRight"].toInt();
	imagePackerRef.border.b = pBankData->m_MetaObj["sbFrameMarginBottom"].toInt();
	imagePackerRef.extrude = 1;
	imagePackerRef.merge = true;
	imagePackerRef.square = true;
	imagePackerRef.autosize = true;
	imagePackerRef.minFillRate = 80;
	imagePackerRef.mergeBF = false;
	imagePackerRef.rotate = ImagePacker::NEVER;
}
