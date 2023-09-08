/**************************************************************************
*	EntityDopeSheetScene.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDOPESHEETSCENE_H
#define ENTITYDOPESHEETSCENE_H

#include <QGraphicsScene>
#include <QJsonObject>

class EntityStateData;
class EntityTreeItemData;

class EntityDopeSheetScene : public QGraphicsScene
{
	EntityStateData *										m_pEntStateData;

	int														m_iFramesPerSecond;
	QMap<EntityTreeItemData *, QMap<int, QJsonObject>>		m_KeyFramesMap;

	int														m_iCurrentFrame;

public:
	EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj);
	virtual ~EntityDopeSheetScene();

	int GetFramesPerSecond() const;
	void SetFramesPerSecond(int iFramesPerSecond);

	int GetCurrentFrame() const;

	QJsonArray SerializeAllKeyFrames(EntityTreeItemData *pItemData) const;
	QJsonObject ExtrapolateKeyFramesProperties(EntityTreeItemData *pItemData, int iFrameIndex) const;

	QJsonValue GetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const;
	void SetKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, QJsonObject propsObj);
	void SetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, QJsonValue jsonValue);
};

#endif // ENTITYDOPESHEETSCENE_H
