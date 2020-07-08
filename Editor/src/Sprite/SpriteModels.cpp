/**************************************************************************
 *	SpriteModels.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "SpriteModels.h"
#include "Project.h"
#include "AtlasModel.h"

#include <QJsonArray>

SpriteFramesModel::SpriteFramesModel(QObject *parent) :
	QAbstractTableModel(parent)
{
}

int SpriteFramesModel::Add(AtlasFrame *pFrame)
{
	SpriteFrame *pFrameToInsert = nullptr;

	// See if this frame has been recently removed, and re-add if possible. Otherwise, create a new Frame
	QMap<QUuid, SpriteFrame *>::iterator iter = m_RemovedFrameIdMap.find(pFrame->GetUuid());
	if(iter == m_RemovedFrameIdMap.end())
		pFrameToInsert = new SpriteFrame(pFrame, m_FramesList.count());
	else
	{
		pFrameToInsert = iter.value();
		m_RemovedFrameIdMap.remove(pFrame->GetUuid());
	}

	beginInsertRows(QModelIndex(), pFrameToInsert->m_iRowIndex, pFrameToInsert->m_iRowIndex);
	m_FramesList.insert(pFrameToInsert->m_iRowIndex, pFrameToInsert);
	endInsertRows();

	return pFrameToInsert->m_iRowIndex;
}

void SpriteFramesModel::Remove(AtlasFrame *pFrame)
{
	for(int i = 0; i < m_FramesList.count(); ++i)
	{
		// NOTE: Don't delete this SpriteFrame as the remove may be 'undone'
		if(m_FramesList[i]->m_pFrame == pFrame)
		{
			m_RemovedFrameIdMap[pFrame->GetUuid()] = m_FramesList[i];

			beginRemoveRows(QModelIndex(), i, i);
			m_FramesList.removeAt(i);
			endRemoveRows();
			break;
		}
	}
}

void SpriteFramesModel::MoveRowUp(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex - 1) == false)
		return;

	m_FramesList.swap(iIndex, iIndex - 1);
	endMoveRows();
}

void SpriteFramesModel::MoveRowDown(int iIndex)
{
	if(beginMoveRows(QModelIndex(), iIndex, iIndex, QModelIndex(), iIndex + 2) == false)    // + 2 is here because Qt is retarded
		return;

	m_FramesList.swap(iIndex, iIndex + 1);
	endMoveRows();
}

// iIndex of -1 will apply to all
void SpriteFramesModel::SetFrameOffset(int iIndex, QPoint vOffset)
{
	if(iIndex == -1)
	{
		for(int i = 0; i < m_FramesList.count(); ++i)
			m_FramesList[i]->m_vOffset = vOffset;

		dataChanged(createIndex(0, COLUMN_OffsetX), createIndex(m_FramesList.count() - 1, COLUMN_OffsetY));
	}
	else
	{
		m_FramesList[iIndex]->m_vOffset = vOffset;
		dataChanged(createIndex(iIndex, COLUMN_OffsetX), createIndex(iIndex, COLUMN_OffsetY));
	}
}

// iIndex of -1 will apply to all
void SpriteFramesModel::AddFrameOffset(int iIndex, QPoint vOffset)
{
	if(iIndex == -1)
	{
		for(int i = 0; i < m_FramesList.count(); ++i)
			m_FramesList[i]->m_vOffset += vOffset;

		dataChanged(createIndex(0, COLUMN_OffsetX), createIndex(m_FramesList.count() - 1, COLUMN_OffsetY));
	}
	else
	{
		m_FramesList[iIndex]->m_vOffset += vOffset;
		dataChanged(createIndex(iIndex, COLUMN_OffsetX), createIndex(iIndex, COLUMN_OffsetY));
	}
}

// iIndex of -1 will apply to all
void SpriteFramesModel::DurationFrame(int iIndex, float fDuration)
{
	if(iIndex == -1)
	{
		for(int i = 0; i < m_FramesList.count(); ++i)
			m_FramesList[i]->m_fDuration = fDuration;

		dataChanged(createIndex(0, COLUMN_Duration), createIndex(m_FramesList.count() - 1, COLUMN_Duration));
	}
	else
	{
		m_FramesList[iIndex]->m_fDuration = fDuration;
		dataChanged(createIndex(iIndex, COLUMN_Duration), createIndex(iIndex, COLUMN_Duration));
	}
}

QJsonArray SpriteFramesModel::GetFramesInfo(float &fTotalDurationRef)
{
	QJsonArray framesArray;
	fTotalDurationRef = 0.0f;

	for(int i = 0; i < m_FramesList.count(); ++i)
	{
		QJsonObject frameObj;
		fTotalDurationRef += m_FramesList[i]->m_fDuration;

		frameObj.insert("checksum", QJsonValue(static_cast<qint64>(m_FramesList[i]->m_pFrame->GetChecksum())));
		frameObj.insert("duration", m_FramesList[i]->m_fDuration);
		frameObj.insert("offsetX", m_FramesList[i]->m_vOffset.x() + m_FramesList[i]->m_pFrame->GetCrop().left());
		frameObj.insert("offsetY", m_FramesList[i]->m_vOffset.y() + ((m_FramesList[i]->m_pFrame->GetSize().height() - 1) - m_FramesList[i]->m_pFrame->GetCrop().bottom())); // -1 on height because it's NOT zero based like everything else);

		framesArray.append(frameObj);
	}

	return framesArray;
}

SpriteFrame *SpriteFramesModel::GetFrameAt(int iIndex)
{
	if(iIndex < 0)
		return nullptr;

	return m_FramesList[iIndex];
}

/*virtual*/ int SpriteFramesModel::rowCount(const QModelIndex & /*parent*/) const
{
   return m_FramesList.count();
}

/*virtual*/ int SpriteFramesModel::columnCount(const QModelIndex & /*parent*/) const
{
	return NUMCOLUMNS;
}

/*virtual*/ QVariant SpriteFramesModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	SpriteFrame *pFrame = m_FramesList[index.row()];

	if (role == Qt::TextAlignmentRole && index.column() != COLUMN_Frame)
	{
		return Qt::AlignCenter;
	}

	if(role == Qt::DisplayRole || role == Qt::EditRole)
	{
		switch(index.column())
		{
		case COLUMN_Frame:
			return pFrame->m_pFrame->GetName();
		case COLUMN_OffsetX:
			return QString::number(pFrame->m_vOffset.x());
		case COLUMN_OffsetY:
			return QString::number(pFrame->m_vOffset.y());
		case COLUMN_Duration:
			return QString::number(pFrame->m_fDuration, 'g', 3) % ((role == Qt::DisplayRole) ? "sec" : "");
		}
	}

	return QVariant();
}

/*virtual*/ QVariant SpriteFramesModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal)
		{
			switch(iIndex)
			{
			case COLUMN_Frame:
				return QString("Frame");
			case COLUMN_OffsetX:
				return QString("X Offset");
			case COLUMN_OffsetY:
				return QString("Y Offset");
			case COLUMN_Duration:
				return QString("Duration");
			}
		}
		else
			return QString::number(iIndex);
	}

	return QVariant();
}

/*virtual*/ bool SpriteFramesModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	HyGuiLog("SpriteFramesModel::setData was invoked", LOGTYPE_Error);

	SpriteFrame *pFrame = m_FramesList[index.row()];

	if(role == Qt::EditRole)
	{
		switch(index.column())
		{
		case COLUMN_OffsetX:
			pFrame->m_vOffset.setX(value.toInt());
			break;
		case COLUMN_OffsetY:
			pFrame->m_vOffset.setY(value.toInt());
			break;
		case COLUMN_Duration:
			pFrame->m_fDuration = value.toFloat();
			break;
		}
	}

	QVector<int> vRolesChanged;
	vRolesChanged.append(role);
	dataChanged(index, index, vRolesChanged);

	return true;
}

/*virtual*/ Qt::ItemFlags SpriteFramesModel::flags(const QModelIndex &index) const
{
	if(index.column() == COLUMN_Frame)
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	else
		return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteStateData::SpriteStateData(int iStateIndex, IModel &modelRef, FileDataPair stateFileData) :
	IStateData(iStateIndex, modelRef, stateFileData),
	m_pChkMapper_Loop(nullptr),
	m_pChkMapper_Reverse(nullptr),
	m_pChkMapper_Bounce(nullptr),
	m_pFramesModel(nullptr)
{
	m_pChkMapper_Loop = new CheckBoxMapper(&m_ModelRef);
	m_pChkMapper_Reverse = new CheckBoxMapper(&m_ModelRef);
	m_pChkMapper_Bounce = new CheckBoxMapper(&m_ModelRef);
	m_pFramesModel = new SpriteFramesModel(&m_ModelRef);

	m_pChkMapper_Loop->SetChecked(stateFileData.m_Data["loop"].toBool(false));
	m_pChkMapper_Reverse->SetChecked(stateFileData.m_Data["reverse"].toBool(false));
	m_pChkMapper_Bounce->SetChecked(stateFileData.m_Data["bounce"].toBool(false));

	QJsonArray metaFrameArray = stateFileData.m_Meta["assetUUIDs"].toArray();
	QJsonArray dataFrameArray = stateFileData.m_Data["frames"].toArray();

	QList<QUuid> uuidRequestList;
	for(int i = 0; i < metaFrameArray.size(); ++i)
		uuidRequestList.append(QUuid(metaFrameArray[i].toString()));

	int iAffectedFrameIndex = 0;

	//QList<AtlasFrame *> requestedAtlasFramesList = m_ModelRef.RequestFramesByUuid(this, uuidRequestList, iAffectedFrameIndex);
	
	QList<AssetItemData *> requestedAtlasFramesList = m_ModelRef.GetItem().GetProject().GetAtlasModel().RequestAssetsByUuid(&m_ModelRef.GetItem(), uuidRequestList);
		

	for(int i = 0; i < requestedAtlasFramesList.size(); ++i)
		AddFrame(static_cast<AtlasFrame *>(requestedAtlasFramesList[i]));

		
	if(dataFrameArray.size() != requestedAtlasFramesList.size())
		HyGuiLog("SpriteStatesModel::AppendState() failed to acquire all the stored frames", LOGTYPE_Error);

	for(int i = 0; i < requestedAtlasFramesList.size(); ++i)
	{
		AtlasFrame *pFrame = static_cast<AtlasFrame *>(requestedAtlasFramesList[i]);

		QJsonObject spriteFrameObj = dataFrameArray[i].toObject();
		QPoint vOffset(spriteFrameObj["offsetX"].toInt() - pFrame->GetCrop().left(),
						spriteFrameObj["offsetY"].toInt() - ((pFrame->GetSize().height() - 1) - pFrame->GetCrop().bottom()));  // -1 on height because it's NOT zero based like everything else

		m_pFramesModel->SetFrameOffset(i, vOffset);
		m_pFramesModel->DurationFrame(i, spriteFrameObj["duration"].toDouble());
	}
}

/*virtual*/ SpriteStateData::~SpriteStateData()
{
	delete m_pChkMapper_Loop;
	delete m_pChkMapper_Reverse;
	delete m_pChkMapper_Bounce;
	delete m_pFramesModel;
}

CheckBoxMapper *SpriteStateData::GetLoopMapper()
{
	return m_pChkMapper_Loop;
}

CheckBoxMapper *SpriteStateData::GetReverseMapper()
{
	return m_pChkMapper_Reverse;
}

CheckBoxMapper *SpriteStateData::GetBounceMapper()
{
	return m_pChkMapper_Bounce;
}

SpriteFramesModel *SpriteStateData::GetFramesModel()
{
	return m_pFramesModel;
}

//void SpriteStateData::GetStateFileData(FileDataPair &stateFileDataOut)
//{
//	stateFileDataOut
//
//	QJsonArray frameArray;
//	float fTotalDuration = 0.0f;
//	for(int i = 0; i < m_pFramesModel->rowCount(); ++i)
//	{
//		SpriteFrame *pSpriteFrame = m_pFramesModel->GetFrameAt(i);
//
//		QJsonObject frameObj;
//		frameObj.insert("checksum", QJsonValue(static_cast<qint64>(pSpriteFrame->m_pFrame->GetImageChecksum())));
//		frameObj.insert("duration", QJsonValue(pSpriteFrame->m_fDuration));
//		frameObj.insert("id", QJsonValue(static_cast<qint64>(pSpriteFrame->m_pFrame->GetId()))); uuid;
//		frameObj.insert("offsetX", QJsonValue(pSpriteFrame->m_vOffset.x() + pSpriteFrame->m_pFrame->GetCrop().left()));
//		frameObj.insert("offsetY", QJsonValue(pSpriteFrame->m_vOffset.y() + ((pSpriteFrame->m_pFrame->GetSize().height() - 1) - pSpriteFrame->m_pFrame->GetCrop().bottom()))); // -1 on height because it's NOT zero based like everything else
//		fTotalDuration += pSpriteFrame->m_fDuration;
//
//		frameArray.append(frameObj);
//	}
//
//	stateObjOut.insert("bounce", m_pChkMapper_Bounce->IsChecked());
//	stateObjOut.insert("duration", QJsonValue(fTotalDuration));
//	stateObjOut.insert("frames", QJsonValue(frameArray));
//	stateObjOut.insert("loop", m_pChkMapper_Loop->IsChecked());
//	stateObjOut.insert("name", QJsonValue(GetName()));
//	stateObjOut.insert("reverse", m_pChkMapper_Reverse->IsChecked());
//}

QList<AssetItemData *> SpriteStateData::GetAtlasFrames() const
{
	QList<AssetItemData *> atlasList;
	for(int i = 0; i < m_pFramesModel->rowCount(); ++i)
		atlasList.push_back(m_pFramesModel->GetFrameAt(i)->m_pFrame);

	return atlasList;
}

/*virtual*/ int SpriteStateData::AddFrame(AtlasFrame *pFrame)
{
	return m_pFramesModel->Add(pFrame);
}

/*virtual*/ void SpriteStateData::RelinquishFrame(AtlasFrame *pFrame) /*override*/
{
	m_pFramesModel->Remove(pFrame);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SpriteModel::SpriteModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef)
{
	InitStates<SpriteStateData>(itemFileDataRef);
}

/*virtual*/ SpriteModel::~SpriteModel()
{
}

/*virtual*/ void SpriteModel::InsertItemSpecificData(FileDataPair &itemFileDataOut) /*override*/
{
}

/*virtual*/ FileDataPair SpriteModel::GetStateFileData(uint32 uiIndex) const /*override*/
{
	FileDataPair stateFileData;

	SpriteStateData *pState = static_cast<SpriteStateData *>(m_StateList[uiIndex]);

	stateFileData.m_Meta.insert("name", m_StateList[uiIndex]->GetName());
	QJsonArray frameIdsArray;

	QList<AssetItemData *> frameList = pState->GetAtlasFrames();
	for(int i = 0; i < frameList.size(); ++i)
		frameIdsArray.append(frameList[i]->GetUuid().toString(QUuid::WithoutBraces));
	stateFileData.m_Meta.insert("assetUUIDs", frameIdsArray);

	stateFileData.m_Data.insert("loop", pState->GetLoopMapper()->IsChecked());
	stateFileData.m_Data.insert("reverse", pState->GetReverseMapper()->IsChecked());
	stateFileData.m_Data.insert("bounce", pState->GetBounceMapper()->IsChecked());

	float fTotalDuration = 0.0f;
	QJsonArray framesArray = pState->GetFramesModel()->GetFramesInfo(fTotalDuration);
	stateFileData.m_Data.insert("frames", framesArray);
	stateFileData.m_Data.insert("duration", fTotalDuration);

	return stateFileData;
}

/*virtual*/ QList<AssetItemData *> SpriteModel::GetAssets(HyGuiItemType eType) const /*override*/
{
	QList<AssetItemData *> retAtlasFrameList;

	if(eType == ITEM_AtlasImage)
	{
		for(int i = 0; i < m_StateList.size(); ++i)
		{
			QList<AssetItemData *> atlasFrameList = static_cast<SpriteStateData *>(m_StateList[i])->GetAtlasFrames();
			retAtlasFrameList += atlasFrameList;
		}
	}

	return retAtlasFrameList;
}

/*virtual*/ QStringList SpriteModel::GetFontUrls() const /*override*/
{
	return QStringList();
}
