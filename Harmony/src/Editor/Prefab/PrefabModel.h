/**************************************************************************
 *	PrefabModel.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef PREFABMODEL_H
#define PREFABMODEL_H

#include "IModel.h"

#include <QObject>

struct Vertex
{
	glm::vec3 m_pos;
	glm::vec2 m_tex;
	glm::vec3 m_normal;

	Vertex() {}

	Vertex(const glm::vec3& pos, const glm::vec2& tex, const glm::vec3& normal)
	{
		m_pos    = pos;
		m_tex    = tex;
		m_normal = normal;
	}
};

struct MeshEntry
{
	MeshEntry() { }
	~MeshEntry() { }

	void Init(const std::vector<Vertex> &Vertices, const std::vector<unsigned int> &Indices);

	GLuint VB;
	GLuint IB;
	unsigned int NumIndices;
	unsigned int MaterialIndex;
};

class PrefabModel : public IModel
{
	QStringList			m_sTextureDiffuseList;

	QList<MeshEntry>	m_MeshList;
	QStringList			m_sTextureList;

public:
	PrefabModel(ProjectItem &itemRef, QJsonValue initValue);



	virtual void OnSave() override;
	virtual QJsonObject PopStateAt(uint32 uiIndex) override;
	virtual QJsonValue GetJson() const override;
	virtual QList<AtlasFrame *> GetAtlasFrames() const override;
	virtual QStringList GetFontUrls() const override;
	virtual void Refresh() override;
};

#endif // PREFABMODEL_H
