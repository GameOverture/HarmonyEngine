/**************************************************************************
 *	AtlasModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2017 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GltfModel.h"
#include "Project.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

GltfModel::GltfModel(Project *pProjOwner) :
	m_pProjOwner(pProjOwner),
	m_MetaDir(m_pProjOwner->GetMetaDataAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true)),
	m_RootDataDir(m_pProjOwner->GetAssetsAbsPath() + HyGlobal::ItemName(ITEM_AtlasImage, true))
{
	if(m_MetaDir.exists() == false)
	{
		HyGuiLog("Meta atlas directory is missing, recreating", LOGTYPE_Info);
		m_MetaDir.mkpath(m_MetaDir.absolutePath());
	}
	if(m_RootDataDir.exists() == false)
	{
		HyGuiLog("Data atlas directory is missing, recreating", LOGTYPE_Info);
		m_RootDataDir.mkpath(m_RootDataDir.absolutePath());
	}
}

/*virtual*/ GltfModel::~GltfModel()
{
}

Project *GltfModel::GetProjOwner()
{
	return m_pProjOwner;
}

/*virtual*/ int GltfModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
	return 0;//m_AtlasGrpList.size();
}

/*virtual*/ QVariant GltfModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignLeft;

	//if(role == Qt::DisplayRole || role == Qt::EditRole)
	//	return "Id: " % QString::number(m_AtlasGrpList[index.row()]->GetId()) % " - " % m_AtlasGrpList[index.row()]->m_PackerSettings["txtName"].toString();

	return QVariant();
}

/*virtual*/ bool GltfModel::setData(const QModelIndex &index, const QVariant &value, int role /*= Qt::EditRole*/)
{
	return QAbstractItemModel::setData(index, value, role);
}

/*virtual*/ QVariant GltfModel::headerData(int iIndex, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	return QVariant();
}
