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

AtlasRepackThread::AtlasRepackThread(BankData &bankRef, QList<int> textureIndexList, QList<AtlasFrame *> newFramesList, QDir metaDir) :
	m_BankRef(bankRef),
	m_TextureIndexList(textureIndexList),
	m_NewFramesList(newFramesList),
	m_MetaDir(metaDir)
{
}

/*virtual*/ AtlasRepackThread::~AtlasRepackThread()
{
}

/*virtual*/ void AtlasRepackThread::run() /*override*/
{
	m_Packer.clear();

	// Repack the affected frames and determine how many textures this repack took
	for(int i = 0; i < m_NewFramesList.size(); ++i)
	{
		m_Packer.addItem(m_NewFramesList[i]->GetSize(),
						 m_NewFramesList[i]->GetCrop(),
						 m_NewFramesList[i]->GetChecksum(),
						 m_NewFramesList[i],
						 m_MetaDir.absoluteFilePath(m_NewFramesList[i]->ConstructMetaFileName()));
	}
	
	SetPackerSettings();

	m_Packer.pack(m_BankRef.m_MetaObj["cmbHeuristic"].toInt(),
				  m_BankRef.m_MetaObj["sbTextureWidth"].toInt(),
				  m_BankRef.m_MetaObj["sbTextureHeight"].toInt());

	// Subtract '1' from the number of new textures because we want to ensure the last generated (and likely least filled) texture is last
	int iNumNewTextures = m_Packer.bins.size() - 1;

	QDir runtimeBankDir(m_BankRef.m_sAbsPath);

	// Delete the old textures
	for(int i = 0; i < m_TextureIndexList.size(); ++i)
	{
		QFile::remove(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(m_TextureIndexList[i]) % ".png"));
		QFile::remove(runtimeBankDir.absoluteFilePath(HyGlobal::MakeFileNameFromCounter(m_TextureIndexList[i]) % ".dds"));
	}

	// Grab 'existingTexturesInfoList' after deleting obsolete textures
	QFileInfoList existingTexturesInfoList = runtimeBankDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

	// Using our stock of newly generated textures, fill in any gaps in the texture array. If there aren't enough new textures then shift textures (and their frames) to fill any remaining gaps in the indices.
	int iTotalNumTextures = iNumNewTextures + existingTexturesInfoList.size();

	int iNumNewTexturesUsed = 0;
	int iCurrentIndex = 0;
	for(; iCurrentIndex < iTotalNumTextures; ++iCurrentIndex)
	{
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
			ConstructAtlasTexture(iNumNewTexturesUsed, iCurrentIndex);
			iNumNewTexturesUsed++;
		}
		else
		{
			// There aren't enough new textures to fill all the gaps in indices. Find the next existing texture and assign it to iCurrentIndex
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

	// Place the last generated texture at the end of the array
	if(m_Packer.bins.empty() == false)
		ConstructAtlasTexture(m_Packer.bins.size() - 1, iCurrentIndex);

	// Assign all the duplicate frames
	for(int i = 0; i < m_Packer.images.size(); ++i)
	{
		inputImage &imgInfoRef = m_Packer.images[i];
		if(imgInfoRef.duplicateId != nullptr)
		{
			AtlasFrame *pFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.id);
			AtlasFrame *pDupFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.duplicateId);

			pFrame->UpdateInfoFromPacker(pDupFrame->GetTextureIndex(), pDupFrame->GetX(), pDupFrame->GetY());
		}
	}

	Q_EMIT RepackIsFinished();
}

void AtlasRepackThread::ConstructAtlasTexture(int iPackerBinIndex, int iActualTextureIndex)
{
	if(m_BankRef.m_MetaObj["sbTextureWidth"].toInt() != m_Packer.bins[iPackerBinIndex].width() ||
	   m_BankRef.m_MetaObj["sbTextureHeight"].toInt() != m_Packer.bins[iPackerBinIndex].height())
	{
		HyGuiLog("WidgetAtlasGroup::ConstructAtlasTexture() Mismatching texture dimensions", LOGTYPE_Error);
	}

	QImage newTexture(m_BankRef.m_MetaObj["sbTextureWidth"].toInt(), m_BankRef.m_MetaObj["sbTextureHeight"].toInt(), QImage::Format_ARGB32);
	newTexture.fill(Qt::transparent);

	QPainter p(&newTexture);

	// Iterate through the images that were packed, and update their corresponding AtlasFrame. Then draw them to the blank textures
	for(int i = 0; i < m_Packer.images.size(); ++i)
	{
		inputImage &imgInfoRef = m_Packer.images[i];
		AtlasFrame *pFrame = reinterpret_cast<AtlasFrame *>(imgInfoRef.id);
		bool bValidToDraw = true;

		if(imgInfoRef.pos.x() == 999999)    // This is scriptum image packer's (dumb) indication of an invalid image...
		{
			pFrame->UpdateInfoFromPacker(-1, -1, -1);
			bValidToDraw = false;
		}
		else
			pFrame->ClearError(ATLASFRAMEERROR_CouldNotPack);

		if(imgInfoRef.duplicateId != nullptr)
			bValidToDraw = false;

		if(imgInfoRef.textureId != iPackerBinIndex)
			bValidToDraw = false;

		if(bValidToDraw == false)
			continue;

		pFrame->UpdateInfoFromPacker(iActualTextureIndex,
									 imgInfoRef.pos.x() + m_Packer.border.l,
									 imgInfoRef.pos.y() + m_Packer.border.t);

		QImage imgFrame(imgInfoRef.path);

		QSize size;
		QRect crop;
		if(!m_Packer.cropThreshold)
		{
			size = imgInfoRef.size;
			crop = QRect(0, 0, size.width(), size.height());
		}
		else
		{
			size = imgInfoRef.crop.size();
			crop = imgInfoRef.crop;
		}

//        if(imgInfoRef.rotated)
//        {
//            QTransform rotateTransform;
//            rotateTransform.rotate(90);
//            imgFrame = imgFrame.transformed(rotateTransform);

//            size.transpose();
//            crop = QRect(imgInfoRef.size.height() - crop.y() - crop.height(),
//                         crop.x(), crop.height(), crop.width());
//        }

		QPoint pos(pFrame->GetX(), pFrame->GetY());
	/*	if(m_pAtlasGrp->m_Packer.extrude)
		{
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			QColor color1 = QColor::fromRgba(imgFrame.pixel(crop.x(), crop.y()));
			p.setPen(color1);
			p.setBrush(color1);
			if(m_pAtlasGrp->m_Packer.extrude == 1)
				p.drawPoint(QPoint(pos.x(), pos.y()));
			else
				p.drawRect(QRect(pos.x(), pos.y(), m_pAtlasGrp->m_Packer.extrude - 1, m_pAtlasGrp->m_Packer.extrude - 1));
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			QColor color2 = QColor::fromRgba(imgFrame.pixel(crop.x(), crop.y() + crop.height() - 1));
			p.setPen(color2);
			p.setBrush(color2);
			if(m_pAtlasGrp->m_Packer.extrude == 1)
				p.drawPoint(QPoint(pos.x(), pos.y() + crop.height() + m_pAtlasGrp->m_Packer.extrude));
			else
				p.drawRect(QRect(pos.x(), pos.y() + crop.height() + m_pAtlasGrp->m_Packer.extrude, m_pAtlasGrp->m_Packer.extrude - 1, m_pAtlasGrp->m_Packer.extrude - 1));
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			QColor color3 = QColor::fromRgba(imgFrame.pixel(crop.x() + crop.width() - 1, crop.y()));
			p.setPen(color3);
			p.setBrush(color3);
			if(m_pAtlasGrp->m_Packer.extrude == 1)
				p.drawPoint(QPoint(pos.x() + crop.width() + m_pAtlasGrp->m_Packer.extrude, pos.y()));
			else
				p.drawRect(QRect(pos.x() + crop.width() + m_pAtlasGrp->m_Packer.extrude, pos.y(), m_pAtlasGrp->m_Packer.extrude - 1, m_pAtlasGrp->m_Packer.extrude - 1));
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			QColor color4 = QColor::fromRgba(imgFrame.pixel(crop.x() + crop.width() - 1, crop.y() + crop.height() - 1));
			p.setPen(color4);
			p.setBrush(color4);
			if(m_pAtlasGrp->m_Packer.extrude == 1)
				p.drawPoint(QPoint(pos.x() + crop.width() + m_pAtlasGrp->m_Packer.extrude, pos.y() + crop.height() + m_pAtlasGrp->m_Packer.extrude));
			else
				p.drawRect(QRect(pos.x() + crop.width() + m_pAtlasGrp->m_Packer.extrude, pos.y() + crop.height() + m_pAtlasGrp->m_Packer.extrude, m_pAtlasGrp->m_Packer.extrude - 1, m_pAtlasGrp->m_Packer.extrude - 1));
			///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			p.drawImage(QRect(pos.x(), pos.y() + m_pAtlasGrp->m_Packer.extrude, m_pAtlasGrp->m_Packer.extrude, crop.height()), imgFrame, QRect(crop.x(), crop.y(), 1, crop.height()));
			p.drawImage(QRect(pos.x() + crop.width() + m_pAtlasGrp->m_Packer.extrude, pos.y() + m_pAtlasGrp->m_Packer.extrude, m_pAtlasGrp->m_Packer.extrude, crop.height()), imgFrame, QRect(crop.x() + crop.width() - 1, crop.y(), 1, crop.height()));
			p.drawImage(QRect(pos.x() + m_pAtlasGrp->m_Packer.extrude, pos.y(), crop.width(), m_pAtlasGrp->m_Packer.extrude), imgFrame, QRect(crop.x(), crop.y(), crop.width(), 1));
			p.drawImage(QRect(pos.x() + m_pAtlasGrp->m_Packer.extrude, pos.y() + crop.height() + m_pAtlasGrp->m_Packer.extrude, crop.width(), m_pAtlasGrp->m_Packer.extrude), imgFrame, QRect(crop.x(), crop.y() + crop.height() - 1, crop.width(), 1));

			p.drawImage(pos.x() + m_pAtlasGrp->m_Packer.extrude, pos.y() + m_pAtlasGrp->m_Packer.extrude, imgFrame, crop.x(), crop.y(), crop.width(), crop.height());
		}
		else*/
			p.drawImage(pos.x(), pos.y(), imgFrame, crop.x(), crop.y(), crop.width(), crop.height());
	}

	QImage *pTexture = static_cast<QImage *>(p.device());
	HyTextureFormat eTextureType = static_cast<HyTextureFormat>(m_BankRef.m_MetaObj["textureType"].toInt()); // TODO: rename to format [also convert to string representation]

	QDir runtimeBankDir(m_BankRef.m_sAbsPath);

	switch(eTextureType)
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
			HyGuiLog("AtlasModel::ConstructAtlasTexture tried to create an unsupported texture type: " % QString::number(eTextureType), LOGTYPE_Error);
		} break;
	}
}

void AtlasRepackThread::SetPackerSettings()
{
	m_Packer.sortOrder = m_BankRef.m_MetaObj["cmbSortOrder"].toInt();// m_iSortOrderIndex;//ui->cmbSortOrder->currentIndex();
	m_Packer.border.t = m_BankRef.m_MetaObj["sbFrameMarginTop"].toInt();// m_iFrameMarginTop;//ui->sbFrameMarginTop->value();
	m_Packer.border.l = m_BankRef.m_MetaObj["sbFrameMarginLeft"].toInt();// m_iFrameMarginLeft;//ui->sbFrameMarginLeft->value();
	m_Packer.border.r = m_BankRef.m_MetaObj["sbFrameMarginRight"].toInt();// m_iFrameMarginRight;//ui->sbFrameMarginRight->value();
	m_Packer.border.b = m_BankRef.m_MetaObj["sbFrameMarginBottom"].toInt();// m_iFrameMarginBottom;//ui->sbFrameMarginBottom->value();
	m_Packer.extrude = m_BankRef.m_MetaObj["extrude"].toInt();// m_iExtrude;//ui->extrude->value();
	m_Packer.merge = m_BankRef.m_MetaObj["chkMerge"].toBool();// m_bMerge;//ui->chkMerge->isChecked();
	m_Packer.square = m_BankRef.m_MetaObj["chkSquare"].toBool();// m_bSquare;//ui->chkSquare->isChecked();
	m_Packer.autosize = m_BankRef.m_MetaObj["chkAutosize"].toBool();// m_bAutoSize;//ui->chkAutosize->isChecked();
	m_Packer.minFillRate = m_BankRef.m_MetaObj["minFillRate"].toInt();// m_iFillRate;//ui->minFillRate->value();
	m_Packer.mergeBF = false;
	m_Packer.rotate = ImagePacker::NEVER;
}
