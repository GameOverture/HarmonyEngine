/**************************************************************************
 *	SurfaceMaterialsModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef SurfaceMaterialsModel_H
#define SurfaceMaterialsModel_H

#include "PropertiesTreeModel.h"

#include <QAbstractListModel>

class SurfaceMaterialsModel : public QAbstractListModel
{
protected:
	struct Surface
	{
		QUuid					m_Uuid;
		QString					m_sName;
		PropertiesTreeModel		m_Properties;

		QList<QUuid>			m_DependeeTileSetCollisions;

		Surface() :
			m_Uuid(QUuid::createUuid()),
			m_sName("Untitled Surface"),
			m_Properties(nullptr, 0, QVariant())
		{
			InitProperties();
		}

		Surface(QJsonObject initObj) :
			m_Uuid(initObj["UUID"].toString()),
			m_sName(initObj["Name"].toString()),
			m_Properties(nullptr, 0, QVariant())
		{
			InitProperties();
			m_Properties.DeserializeJson(initObj["Properties"].toObject());

			QJsonArray tileSetDependantsArray = initObj["TileSetDependants"].toArray();
			for (const QJsonValue &tileSetDependantVal : tileSetDependantsArray)
				m_DependeeTileSetCollisions.append(QUuid(tileSetDependantVal.toString()));
		}

		void InitProperties()
		{
			m_Properties.AppendCategory("Surface Properties");
			m_Properties.AppendProperty("Surface Properties", "Color", PROPERTIESTYPE_Color, QRect(255, 255, 255, 0), "The color used in the editor as well as for physics debug rendering.");
			m_Properties.AppendProperty("Surface Properties", "Friction", PROPERTIESTYPE_double, 0.6, "The Coulomb (dry) friction coefficient, usually in the range [0,1].");
			m_Properties.AppendProperty("Surface Properties", "Restitution", PROPERTIESTYPE_double, 0.0, "The restitution (bounciness) coefficient, usually in the range [0,1].");
			m_Properties.AppendProperty("Surface Properties", "Rolling Resistance", PROPERTIESTYPE_double, 0.0, "The rolling resistance coefficient, usually in the range [0,1].");
			m_Properties.AppendProperty("Surface Properties", "Tangent Speed", PROPERTIESTYPE_double, 0.0, "The speed at which a body will be propelled when in contact with this surface, (the tangent speed for conveyor belts) in the direction of the surface tangent.");
			m_Properties.AppendProperty("Surface Properties", "User ID", PROPERTIESTYPE_int, 0, "An optional user defined id for this surface. This is passed with query results and to friction and restitution combining functions. It is not used internally.");
		}

		QJsonObject Serialize() const
		{
			QJsonObject surfaceObj;
			surfaceObj["UUID"] = m_Uuid.toString();
			surfaceObj["Name"] = m_sName;
			surfaceObj["Properties"] = m_Properties.SerializeJson();

			QJsonArray tileSetDependantsArray;
			for (const QUuid &tileSetCollisionUuid : m_DependeeTileSetCollisions)
				tileSetDependantsArray.append(tileSetCollisionUuid.toString(QUuid::WithoutBraces));
			surfaceObj["TileSetDependants"] = tileSetDependantsArray;

			return surfaceObj;
		}
	};
	QList<Surface *>			m_SurfaceList;
	
public:
	SurfaceMaterialsModel();
	virtual ~SurfaceMaterialsModel();

	void Initialize(QJsonArray initArray);

	int AppendNewSurface(QUuid tileSetDependee);

	int GetNumSurfaces() const;
	QUuid GetUuid(int iIndex) const;
	QString GetName(int iIndex) const;
	QString SetName(int iIndex, QString sNewName);
	void GetInfo(int iIndex, QString &nameOut, HyColor &colorOut) const;
	PropertiesTreeModel *GetPropertiesModel(int iIndex);

	void AddTileSetDependee(int iIndex, QUuid tileSetDependee);
	void RemoveTileSetDependee(int iIndex, QUuid tileSetDependee);

	void MoveSurfaceBack(int iIndex);
	void MoveSurfaceForward(int iIndex);

	QJsonArray Serialize() const;
	
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual QVariant headerData(int iIndex, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
};

#endif // SurfaceMaterialsModel_H
