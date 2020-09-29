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

#include "soil2/SOIL2.h"

#include <QPainter>
#include <QImageWriter>

AtlasRepackThread::AtlasRepackThread(BankData &bankRef, QList<AtlasFrame *> affectedFramesList, QDir metaDir) :
	IRepackThread(metaDir),
	m_BankRef(bankRef)
{
	// Organize all affected frames into buckets (format/filter)
	for(int i = 0; i < affectedFramesList.size(); ++i)
	{
		QPair<HyTextureFormat, HyTextureFiltering> key(affectedFramesList[i]->GetFormat(), affectedFramesList[i]->GetFiltering());

		if(m_BucketMap.contains(key) == false)
			m_BucketMap.insert(key, new PackerBucket());

		m_BucketMap[key]->m_TextureIndexSet.insert(affectedFramesList[i]->GetTextureIndex());
		m_BucketMap[key]->m_FramesList.append(affectedFramesList[i]);
	}
}

/*virtual*/ AtlasRepackThread::~AtlasRepackThread()
{
	for(auto iter = m_BucketMap.begin(); iter != m_BucketMap.end(); ++iter)
		delete iter.value();
}

/*virtual*/ void AtlasRepackThread::OnRun() /*override*/
{
	QDir runtimeBankDir(m_BankRef.m_sAbsPath);

	QList<int> unfilledTextureIndexList;
	
	for(auto iter = m_BucketMap.begin(); iter != m_BucketMap.end(); ++iter)
	{
		// Delete the old obsolete textures. The assets on them will be regenerated
		QList<int> textureIndexList = iter.value()->m_TextureIndexSet.values();
		for(int i = 0; i < textureIndexList.size(); ++i)
		{
			QFile::remove(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".png"));
			QFile::remove(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(textureIndexList[i]) % ".dds"));
		}


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
	
		SetPackerSettings(iter.value()->m_Packer);

		iter.value()->m_Packer.pack(m_BankRef.m_MetaObj["cmbHeuristic"].toInt(),
								    m_BankRef.m_MetaObj["maxWidth"].toInt(),
								    m_BankRef.m_MetaObj["maxHeight"].toInt());


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
				ConstructAtlasTexture(iter.value()->m_Packer, iter.key().first, iNumNewTexturesUsed, iCurrentIndex);
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
							QList<AssetItemData *> &atlasGrpFrameListRef = m_BankRef.m_AssetList;
							for(int j = 0; j < atlasGrpFrameListRef.size(); ++j)
							{
								AtlasFrame *pFrame = static_cast<AtlasFrame *>(atlasGrpFrameListRef[j]);
								if(pFrame->GetTextureIndex() == iExistingTextureIndex)
									pFrame->UpdateInfoFromPacker(iCurrentIndex, pFrame->GetX(), pFrame->GetY());
							}

							// Rename the texture file to be the new index
							QFile::rename(existingTexturesInfoList[i].absoluteFilePath(), runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iCurrentIndex) % "." % existingTexturesInfoList[i].completeSuffix()));

							// Regrab 'existingTexturesInfoList' after renaming a texture
							existingTexturesInfoList = runtimeBankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

							bHandled = true;
							break;
						}
					}
				}
				while(bHandled == false);
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

				pFrame->UpdateInfoFromPacker(pDupFrame->GetTextureIndex(), pDupFrame->GetX(), pDupFrame->GetY());
			}
		}
	}

	// Replace "unfilledIndices" with all the buckets' last (least packed) bin
	QJsonArray unfilledIndicesArray;
	for(auto idx : unfilledTextureIndexList)
		unfilledIndicesArray.append(idx);
	m_BankRef.m_MetaObj["unfilledIndices"] = unfilledIndicesArray;
}

void AtlasRepackThread::ConstructAtlasTexture(ImagePacker &imagePackerRef, HyTextureFormat eFormat, int iPackerBinIndex, int iActualTextureIndex)
{
	if(m_BankRef.m_MetaObj["maxWidth"].toInt() != imagePackerRef.bins[iPackerBinIndex].width() ||
	   m_BankRef.m_MetaObj["maxHeight"].toInt() != imagePackerRef.bins[iPackerBinIndex].height())
	{
		HyGuiLog("WidgetAtlasGroup::ConstructAtlasTexture() Mismatching texture dimensions", LOGTYPE_Error);
	}

	QImage newTexture(m_BankRef.m_MetaObj["maxWidth"].toInt(), m_BankRef.m_MetaObj["maxHeight"].toInt(), QImage::Format_ARGB32);
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
			pFrame->UpdateInfoFromPacker(-1, -1, -1);
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
									 imgInfoRef.pos.y() + imagePackerRef.border.t);

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
	QDir runtimeBankDir(m_BankRef.m_sAbsPath);

	switch(eFormat)
	{
		case HYTEXTURE_R8G8B8A8: {
			if(false == pTexture->save(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % ".png"))) {
				HyGuiLog("AtlasModel::ConstructAtlasTexture failed to generate a PNG atlas", LOGTYPE_Error);
			}
		} break;

		case HYTEXTURE_DTX5: {
			QImage imgProperlyFormatted = pTexture->convertToFormat(QImage::Format_RGBA8888);
			if(0 == SOIL_save_image_quality(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % ".dds").toStdString().c_str(),
											SOIL_SAVE_TYPE_DDS,
											imgProperlyFormatted.width(),
											imgProperlyFormatted.height(),
											4,
											imgProperlyFormatted.bits(),
											0))
			{
				HyGuiLog("AtlasModel::ConstructAtlasTexture failed to generate a DTX5 atlas", LOGTYPE_Error);
			}
		} break;

		case HYTEXTURE_RGB_DTX1: {
			QImage imgProperlyFormatted = pTexture->convertToFormat(QImage::Format_RGB888);
			if(0 == SOIL_save_image_quality(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % ".dds").toStdString().c_str(),
											SOIL_SAVE_TYPE_DDS,
											imgProperlyFormatted.width(),
											imgProperlyFormatted.height(),
											3,
											imgProperlyFormatted.bits(),
											0))
			{
				HyGuiLog("AtlasModel::ConstructAtlasTexture failed to generate a RGB DTX1 atlas", LOGTYPE_Error);
			}
		} break;

		case HYTEXTURE_RGBA_DTX1: {
			QImage imgProperlyFormatted = pTexture->convertToFormat(QImage::Format_RGBA8888);
			if(0 == SOIL_save_image_quality(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(iActualTextureIndex) % ".dds").toStdString().c_str(),
											SOIL_SAVE_TYPE_DDS,
											imgProperlyFormatted.width(),
											imgProperlyFormatted.height(),
											4,
											imgProperlyFormatted.bits(),
											0))
			{
				HyGuiLog("AtlasModel::ConstructAtlasTexture failed to generate a RGBA DTX1 atlas", LOGTYPE_Error);
			}
		} break;

		default: {
			HyGuiLog("AtlasModel::ConstructAtlasTexture tried to create an unsupported texture type: " % QString::number(eFormat), LOGTYPE_Error);
		} break;
	}
}

void AtlasRepackThread::SetPackerSettings(ImagePacker &imagePackerRef)
{
	imagePackerRef.sortOrder = m_BankRef.m_MetaObj["cmbSortOrder"].toInt();
	imagePackerRef.border.t = m_BankRef.m_MetaObj["sbFrameMarginTop"].toInt();
	imagePackerRef.border.l = m_BankRef.m_MetaObj["sbFrameMarginLeft"].toInt();
	imagePackerRef.border.r = m_BankRef.m_MetaObj["sbFrameMarginRight"].toInt();
	imagePackerRef.border.b = m_BankRef.m_MetaObj["sbFrameMarginBottom"].toInt();
	imagePackerRef.extrude = 1;
	imagePackerRef.merge = true;
	imagePackerRef.square = true;
	imagePackerRef.autosize = true;
	imagePackerRef.minFillRate = 80;
	imagePackerRef.mergeBF = false;
	imagePackerRef.rotate = ImagePacker::NEVER;
}
