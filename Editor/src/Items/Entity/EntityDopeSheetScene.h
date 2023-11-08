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
#include <QGraphicsRectItem>
#include <QJsonObject>

#define TIMELINE_HEIGHT 38.0f
#define TIMELINE_LEFT_MARGIN 250.0f
#define TIMELINE_NOTCH_WIDTH 88.0f
#define TIMELINE_NOTCH_MAINLINE_HEIGHT 15.0f
#define TIMELINE_NOTCH_SUBLINES_HEIGHT 9.0f
#define TIMELINE_NOTCH_SUBLINES_WIDTH 18.0f
#define TIMELINE_NOTCH_TEXT_YPOS 10.0f
#define TIMELINE_CURRENTFRAME_TRIANGLE_WIDTH 8.0f
#define TIMELINE_CURRENTFRAME_TRIANGLE_HEIGHT 8.0f

#define ITEMS_WIDTH (TIMELINE_LEFT_MARGIN - 5.0f)
#define ITEMS_LEFT_MARGIN 32.0f
#define ITEMS_LINE_HEIGHT 22.0f
#define KEYFRAME_HEIGHT 19.0f
#define KEYFRAME_WIDTH 4.0f
#define KEYFRAME_TWEEN_KNOB_RADIUS 5.0f

class EntityStateData;
class EntityTreeItemData;

typedef std::tuple<EntityTreeItemData *, int, QString> KeyFrameKey;
typedef std::tuple<QJsonValue, QJsonValue, QJsonValue> TweenJsonValues;

// NOTE: If you update this enum, apply the update to AuxDopeSheet's constructor
enum AuxDopeWidgetsSection
{
	AUXDOPEWIDGETSECTION_FramesPerSecond = 0,
	AUXDOPEWIDGETSECTION_AutoInitialize,

	NUM_AUXDOPEWIDGETSECTIONS
};

class GraphicsTweenKnobItem : public QGraphicsEllipseItem
{
public:
	GraphicsTweenKnobItem(QGraphicsItem *pParent = nullptr);
	virtual ~GraphicsTweenKnobItem();

protected:
	virtual QVariant itemChange(GraphicsItemChange eChange, const QVariant &value) override;
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent) override;
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent) override;
};

class GraphicsKeyFrameItem : public QGraphicsRectItem
{
	bool					m_bIsTweenKeyFrame;
	QGraphicsLineItem *		m_pGfxTweenLine;
	GraphicsTweenKnobItem *	m_pGfxTweenDurationKnob;

public:
	enum DataKey
	{
		DATAKEY_TreeItemData = 0,
		DATAKEY_FrameIndex,
		DATAKEY_CategoryPropString,	// Category + "/" + Property

		DATAKEY_IsTweenKnob
	};

	GraphicsKeyFrameItem(KeyFrameKey tupleKey, bool bIsTweenKeyFrame, qreal x, qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);
	virtual ~GraphicsKeyFrameItem();

	KeyFrameKey GetKey() const;
	bool IsTweenKeyFrame() const;
	void SetTweenLineLength(qreal fLength);

protected:
	virtual QVariant itemChange(GraphicsItemChange eChange, const QVariant &value) override;
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent) override;
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent) override;
};

class EntityDopeSheetScene : public QGraphicsScene
{
	EntityStateData *																m_pEntStateData;

	class AuxWidgetsModel : public QAbstractTableModel
	{
		EntityDopeSheetScene &														m_DopeSheetSceneRef;
		int																			m_iFramesPerSecond;
		bool 																		m_bAutoInitialize;

	public:
		AuxWidgetsModel(EntityDopeSheetScene &dopeSheetSceneRef, int iFramesPerSecond, bool bAutoInitialize);
		virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
		virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
		virtual QVariant data(const QModelIndex &modelIndex, int role = Qt::DisplayRole) const override;
		virtual bool setData(const QModelIndex &modelIndex, const QVariant &value, int role = Qt::EditRole) override;
	};
	AuxWidgetsModel																	m_AuxWidgetsModel;

	QMap<EntityTreeItemData *, QMap<int, QJsonObject>>								m_KeyFramesMap;		// Store properties and tween values
	QMap<int, QString>																m_CallbackMap;

	QMap<KeyFrameKey, GraphicsKeyFrameItem *>										m_KeyFramesGfxRectMap;
	QMap<KeyFrameKey, GraphicsKeyFrameItem *>										m_TweenGfxRectMap;
	QMap<int, GraphicsKeyFrameItem *>												m_CallbackGfxMap;

	int																				m_iCurrentFrame;
	QGraphicsLineItem *																m_pCurrentFrameLine;

public:
	EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj);
	virtual ~EntityDopeSheetScene();

	EntityStateData *GetStateData() const;

	QAbstractItemModel *GetAuxWidgetsModel();

	int GetFramesPerSecond() const;
	bool IsAutoInitialize() const;

	int GetCurrentFrame() const;
	void SetCurrentFrame(int iFrameIndex);

	const QMap<EntityTreeItemData *, QMap<int, QJsonObject>> &GetKeyFramesMap() const;

	bool ContainsKeyFrameProperty(KeyFrameKey tupleKey);
	bool ContainsKeyFrameTween(KeyFrameKey tupleKey);

	// 'm_KeyFrameMap' must be fully updated before using this function
	QList<QPair<QString, QString>> GetUniquePropertiesList(EntityTreeItemData *pItemData) const; // This is mainly useful for rendering the dope sheet

	QJsonArray SerializeAllKeyFrames(EntityTreeItemData *pItemData) const;
	QJsonObject GetKeyFrameProperties(EntityTreeItemData *pItemData) const;
	QJsonObject ExtrapolateKeyFramesProperties(EntityTreeItemData *pItemData) const;
	QJsonValue GetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const;
	QJsonValue ExtrapolateKeyFrameProperty(EntityTreeItemData *pItemData, QString sCategoryName, QString sPropName) const;

	void SetKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, QJsonObject propsObj);
	bool SetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, QJsonValue jsonValue, bool bRefreshGfxItems);
	void RemoveKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, bool bRefreshGfxItems);
	void RemoveKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, bool bRefreshGfxItems);

	TweenJsonValues GetTweenJsonValues(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp) const;
	void SetKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, TweenJsonValues tweenValues, bool bRefreshGfxItems);

	QJsonArray SerializeCallbacks() const;
	QString GetCallback(int iFrameIndex) const;
	void SetCallback(int iFrameIndex, QString sCallback);
	void RemoveCallback(int iFrameIndex);

	void NudgeKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, int iNudgeAmount, bool bRefreshGfxItems);
	void NudgeKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, int iNudgeAmount, bool bRefreshGfxItems);

	void SelectAllItemKeyFrames(EntityTreeItemData *pItemData);

	void RefreshAllGfxItems();

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent) override;
};

#endif // ENTITYDOPESHEETSCENE_H
