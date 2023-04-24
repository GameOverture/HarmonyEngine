/**************************************************************************
 *	IModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IModel.h"
#include "AtlasModel.h"
#include "Project.h"

IStateData::IStateData(int iIndex, IModel &modelRef, FileDataPair stateFileData) :
	m_iINDEX(iIndex),
	m_ModelRef(modelRef),
	m_sName(stateFileData.m_Meta["name"].toString())
{ }

/*virtual*/ IStateData::~IStateData()
{ }

int IStateData::GetIndex() const
{
	return m_iINDEX;
}

QString IStateData::GetName() const
{
	return m_sName;
}

void IStateData::SetName(QString sNewName)
{
	m_sName = sNewName;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IModel::IModel(ProjectItemData &itemRef, const FileDataPair &itemFileDataRef) :
	QAbstractListModel(&itemRef),
	m_ItemRef(itemRef)
{
}

/*virtual*/ IModel::~IModel()
{
}

ProjectItemData &IModel::GetItem()
{
	return m_ItemRef;
}

const ProjectItemData &IModel::GetItem() const
{
	return m_ItemRef;
}

const QUuid &IModel::GetUuid() const
{
	return m_ItemRef.GetUuid();
}

int IModel::GetNumStates() const
{
	return m_StateList.size();
}

IStateData *IModel::GetStateData(int iStateIndex)
{
	return m_StateList[iStateIndex];
}

FileDataPair IModel::GetStateFileData(uint32 uiIndex) const
{
	FileDataPair stateFileData;
	stateFileData.m_Meta.insert("name", m_StateList[uiIndex]->GetName());
	InsertStateSpecificData(uiIndex, stateFileData);

	return stateFileData;
}

QString IModel::SetStateName(int iStateIndex, QString sNewName)
{
	QString sOldName = m_StateList[iStateIndex]->GetName();
	m_StateList[iStateIndex]->SetName(sNewName);

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);

	return sOldName;
}

void IModel::MoveStateBack(int iStateIndex)
{
	if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex - 1) == false)
		return;

	m_StateList.swapItemsAt(iStateIndex, iStateIndex - 1);
	endMoveRows();

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

void IModel::MoveStateForward(int iStateIndex)
{
	if(beginMoveRows(QModelIndex(), iStateIndex, iStateIndex, QModelIndex(), iStateIndex + 2) == false)    // + 2 is here because Qt is retarded
		return;

	m_StateList.swapItemsAt(iStateIndex, iStateIndex + 1);
	endMoveRows();

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(iStateIndex, 0), createIndex(iStateIndex, 0), roleList);
}

FileDataPair IModel::PopState(uint32 uiIndex)
{
	FileDataPair retObj = GetStateFileData(uiIndex);

	beginRemoveRows(QModelIndex(), uiIndex, uiIndex);
	m_StateList.removeAt(uiIndex);
	endRemoveRows();

	QVector<int> roleList;
	roleList.append(Qt::DisplayRole);
	dataChanged(createIndex(0, 0), createIndex(m_StateList.size() - 1, 0), roleList);

	return retObj;
}

QList<IAssetItemData *> IModel::GetAssetDependencies(AssetManagerType eAssetType) const
{
	QList<IAssetItemData *> returnList;
	
	QList<TreeModelItemData *> dependeeList = m_ItemRef.GetDependees();
	for(TreeModelItemData *pDependee : dependeeList)
	{
		if(pDependee->IsAssetItem() && static_cast<IAssetItemData *>(pDependee)->GetAssetManagerType() == eAssetType)
			returnList.push_back(static_cast<IAssetItemData *>(pDependee));
	}

	return returnList;


	//// Sprite
	//QList<AssetItemData *> retAtlasFrameList;
	//if(eAssetType == ASSETMAN_Atlases)
	//{
	//	for(int i = 0; i < m_StateList.size(); ++i)
	//	{
	//		QList<AssetItemData *> atlasFrameList = static_cast<SpriteStateData *>(m_StateList[i])->GetAtlasFrames();
	//		retAtlasFrameList += atlasFrameList;
	//	}
	//}
	//return retAtlasFrameList;

	//// Text
	//QList<AssetItemData *> retAtlasFrameList;
	//if(eAssetType == ASSETMAN_Atlases)
	//{
	//	if(m_pAtlasFrame)
	//		retAtlasFrameList.push_back(m_pAtlasFrame);
	//}
	//return retAtlasFrameList;

	//// Spine
	//QList<AssetItemData *> retAtlasFrameList;
	//if(eAssetType == ASSETMAN_Atlases)
	//{
	//	for(auto &subAtlas : m_SubAtlasList)
	//	{
	//		if(subAtlas.m_pAtlasFrame)
	//			retAtlasFrameList.push_back(subAtlas.m_pAtlasFrame);
	//	}
	//}
	//return retAtlasFrameList;

	//// Audio
	//if(eAssetType != ASSETMAN_Audio)
	//	return QList<AssetItemData *>();
	//QList<AssetItemData *> returnList;
	//for(auto pState : m_StateList)
	//{
	//	for(int i = 0; i < static_cast<AudioStateData *>(pState)->GetPlayListModel().rowCount(); ++i)
	//		returnList << static_cast<AudioStateData *>(pState)->GetPlayListModel().GetAudioAssetAt(i)->GetAudioAsset();
	//}

	//return returnList;
}

/*virtual*/ int IModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const /*override*/
{
	return m_StateList.size();
}

/*virtual*/ QVariant IModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const /*override*/
{
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignLeft;

	if(role == Qt::DisplayRole || role == Qt::EditRole)
		return QString::number(index.row()) % " - " % m_StateList[index.row()]->GetName();

	return QVariant();
}

/*virtual*/ bool IModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/) /*override*/
{
	return QAbstractItemModel::setData(index, value, role);
}

/*virtual*/ QVariant IModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const /*override*/
{
	return (iIndex == 0) ? QVariant(QString("State Names")) : QVariant();
}
