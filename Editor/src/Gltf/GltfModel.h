/**************************************************************************
 *	GltfModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2019 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GLTFMODEL_H
#define GLTFMODEL_H

#include <QAbstractListModel>

class GltfModel : public QAbstractListModel
{
	Q_OBJECT

	Project *                                       m_pProjOwner;

	quint32                                         m_uiNextFrameId;
	quint32                                         m_uiNextAtlasId;

	QDir                                            m_MetaDir;
	QDir                                            m_RootDataDir;

public:
	GltfModel(Project *pProjOwner);
	virtual ~GltfModel();
	
	Project *GetProjOwner();

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
};

#endif // GLTFMODEL_H
