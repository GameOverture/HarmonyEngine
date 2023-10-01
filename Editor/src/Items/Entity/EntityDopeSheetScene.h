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

#define TIMELINE_HEIGHT 38.0f
#define TIMELINE_LEFT_MARGIN 250.0f
#define TIMELINE_NOTCH_WIDTH 88.0f
#define TIMELINE_NOTCH_MAINLINE_HEIGHT 15.0f
#define TIMELINE_NOTCH_SUBLINES_HEIGHT 9.0f
#define TIMELINE_NOTCH_SUBLINES_WIDTH 18.0f
#define TIMELINE_NOTCH_TEXT_YPOS 10.0f

#define ITEMS_LEFT_MARGIN 32.0f
#define ITEMS_LINE_HEIGHT 22.0f
#define KEYFRAME_HEIGHT 20.0f
#define KEYFRAME_WIDTH 5.0f

class EntityStateData;
class EntityTreeItemData;

class EntityDopeSheetScene : public QGraphicsScene
{
	friend class EntityDopeSheetView;

	EntityStateData *															m_pEntStateData;

	int																			m_iFramesPerSecond;

	QMap<EntityTreeItemData *, QMap<int, QJsonObject>>							m_KeyFramesMap;
	QMap<std::tuple<EntityTreeItemData *, int, QString>, QGraphicsRectItem *>	m_KeyFramesGfxRectMap;

	int																			m_iCurrentFrame;
	float																		m_fZoom;

public:
	EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj);
	virtual ~EntityDopeSheetScene();

	int GetFramesPerSecond() const;
	void SetFramesPerSecond(int iFramesPerSecond);

	int GetCurrentFrame() const;
	float GetZoom() const;

	const QMap<EntityTreeItemData *, QMap<int, QJsonObject>> &GetKeyFramesMap() const;

	// 'm_KeyFrameMap' must be fully updated before using this function
	QList<QPair<QString, QString>> GetUniquePropertiesList(EntityTreeItemData *pItemData) const; // This is mainly useful for rendering the dope sheet

	QJsonArray SerializeAllKeyFrames(EntityTreeItemData *pItemData) const;
	QJsonObject ExtrapolateKeyFramesProperties(EntityTreeItemData *pItemData) const;

	QJsonValue GetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const;
	void SetKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, QJsonObject propsObj);
	void SetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, QJsonValue jsonValue);
	void RemoveKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName);

private:
	void RefreshGfxItems(int iFrameIndex);

	// 'm_KeyFrameMap' must be fully updated before using this function
	//QPointF GetKeyFramePos(EntityTreeItemData *pEntTreeItemData, int iFrameIndex, QString sCategory, QString sProperty) const;
};

#endif // ENTITYDOPESHEETSCENE_H
