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
	QSizeF													m_ViewportSize;
	float													m_fZoom;

	// Graphic Items ////////////////////////////////////////////////////////////////////////
	struct TimeLine
	{
		float												m_fCurWidth;
		float												m_fCurZoom;

		QGraphicsRectItem *									m_pRectBackground = nullptr; // TimeLine Root
		QGraphicsLineItem *									m_pLineSeparator = nullptr;

		struct Notch
		{
			QGraphicsLineItem *								m_pMainLine = nullptr; // Notch Root
			QGraphicsSimpleTextItem *						m_pTextFrameShadow = nullptr;
			QGraphicsSimpleTextItem *						m_pTextFrame = nullptr;
			QGraphicsLineItem *								m_pSubLine[4] = { nullptr, nullptr, nullptr, nullptr };

			Notch(QGraphicsItem *pParent);
			~Notch();
			void Update(float fPosX, float fSubLineSpacing, int iNumSubLines, QString sFrameText);
			QGraphicsLineItem *Root() { return m_pMainLine; }
		};
		QList<Notch *>										m_NotchLineList;

		TimeLine(QGraphicsScene *pGfxSceneRef);
		~TimeLine();
		void Update(QSizeF viewportSize, float fZoom);
		QGraphicsRectItem *Root() { return m_pRectBackground; }
	};
	TimeLine												m_TimeLine;

public:
	EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj);
	virtual ~EntityDopeSheetScene();

	int GetFramesPerSecond() const;
	void SetFramesPerSecond(int iFramesPerSecond);

	int GetCurrentFrame() const;

	void SetViewportSize(QSizeF size);

	QJsonArray SerializeAllKeyFrames(EntityTreeItemData *pItemData) const;
	QJsonObject ExtrapolateKeyFramesProperties(EntityTreeItemData *pItemData) const;

	QJsonValue GetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const;
	void SetKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, QJsonObject propsObj);
	void SetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, QJsonValue jsonValue);

protected:
	void UpdateTimeLine();
	void UpdateSceneItems();
};

#endif // ENTITYDOPESHEETSCENE_H
