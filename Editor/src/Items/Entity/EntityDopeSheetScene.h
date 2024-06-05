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
#define CALLBACK_DIAMETER 8.0

class EntityStateData;
class EntityTreeItemData;
class EntityFrameMimeData;

// NOTE: If you update this enum, apply the update to AuxDopeSheet's constructor
enum AuxDopeWidgetsSection
{
	AUXDOPEWIDGETSECTION_FramesPerSecond = 0,
	AUXDOPEWIDGETSECTION_AutoInitialize,

	NUM_AUXDOPEWIDGETSECTIONS
};

enum DopeSheetGfxItemType
{
	GFXITEM_PropertyKeyFrame = 0,
	GFXITEM_TweenKeyFrame,
	GFXITEM_TweenKnob
};

typedef std::tuple<EntityTreeItemData *, int, QString> KeyFrameKey;
enum DopeSheetGfxDataKey // NOTE: Order matters, the first 3 are used when accessing a KeyFrameKey tuple
{
	// Data required to assemble KeyFrameKey
	GFXDATAKEY_TreeItemData = 0,
	GFXDATAKEY_FrameIndex,
	GFXDATAKEY_CategoryPropString,	// Category + "/" + Property

	// Additional info saved to QGraphicsItem
	GFXDATAKEY_Type, // DopeSheetItemType
};

// A tween represented in JSON format
struct TweenJsonValues
{
	QJsonValue		m_Destination;
	QJsonValue		m_Duration;
	QJsonValue		m_TweenFuncType;

	TweenJsonValues() :
		m_Destination(QJsonValue::Undefined),
		m_Duration(QJsonValue::Undefined),
		m_TweenFuncType(QJsonValue::Undefined)
	{ }
	TweenJsonValues(const TweenJsonValues &copy) :
		m_Destination(copy.m_Destination),
		m_Duration(copy.m_Duration),
		m_TweenFuncType(copy.m_TweenFuncType)
	{ }
	TweenJsonValues(const TweenJsonValues &&rVal) noexcept :
		m_Destination(std::move(rVal.m_Destination)),
		m_Duration(std::move(rVal.m_Duration)),
		m_TweenFuncType(std::move(rVal.m_TweenFuncType))
	{ }
	TweenJsonValues(const QJsonValue &destVal, const QJsonValue &durVal, const QJsonValue &tweenFuncVal)
	{
		Set(destVal, durVal, tweenFuncVal);
	}
	TweenJsonValues &operator=(const TweenJsonValues &rhs)
	{
		m_Destination = rhs.m_Destination;
		m_Duration = rhs.m_Duration;
		m_TweenFuncType = rhs.m_TweenFuncType;
		return *this;
	}
	void Set(const QJsonValue &destVal, const QJsonValue &durVal, const QJsonValue &tweenFuncVal)
	{
		m_Destination = destVal;
		m_Duration = durVal;
		m_TweenFuncType = tweenFuncVal;
	}
};

// Upon contextual keyframe selection - This structure holds one quick-tween button that should be shown
struct ContextTweenData
{
	bool					m_bIsBreakTween;
	EntityTreeItemData *	m_pTreeItemData;
	TweenProperty			m_eTweenProperty;
	int						m_iStartFrame;
	QJsonValue				m_StartValue; // This value may be invalid/null
	int						m_iEndFrame;
	QJsonValue				m_EndValue; // This value will always be valid

	ContextTweenData(bool bIsBreakTween, EntityTreeItemData *pTreeItemData, TweenProperty eTweenProperty, int iStartFrame, QJsonValue startValue, int iEndFrame, QJsonValue endValue) :
		m_bIsBreakTween(bIsBreakTween),
		m_pTreeItemData(pTreeItemData),
		m_eTweenProperty(eTweenProperty),
		m_iStartFrame(iStartFrame),
		m_StartValue(startValue),
		m_iEndFrame(iEndFrame),
		m_EndValue(endValue)
	{ }

	bool operator <(const ContextTweenData &rhs) const
	{
		return m_iStartFrame < rhs.m_iStartFrame;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsTweenKnobItem : public QGraphicsEllipseItem
{
public:
	GraphicsTweenKnobItem(KeyFrameKey tupleKey, QGraphicsItem *pParent = nullptr);
	virtual ~GraphicsTweenKnobItem();

	KeyFrameKey GetKey() const;

protected:
	virtual QVariant itemChange(GraphicsItemChange eChange, const QVariant &value) override;
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent) override;
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent) override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsKeyFrameItem : public QGraphicsRectItem
{
	QGraphicsLineItem *		m_pGfxTweenLine;
	GraphicsTweenKnobItem *	m_pGfxTweenDurationKnob;
	int						m_iTweenFramesDuration;

public:
	GraphicsKeyFrameItem(KeyFrameKey tupleKey, bool bIsTweenKeyFrame, QGraphicsItem *pParent = nullptr);
	virtual ~GraphicsKeyFrameItem();

	KeyFrameKey GetKey() const;
	bool IsTweenKeyFrame() const;
	void SetTweenLineLength(qreal fLength, int iFramesDuration);
	int GetTweenFramesDuration() const;

protected:
	virtual QVariant itemChange(GraphicsItemChange eChange, const QVariant &value) override;
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent) override;
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent) override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class EntityDopeSheetScene : public QGraphicsScene
{
	EntityStateData *																m_pEntStateData;

	// These maps store the actual property data for the entire entity
	QMap<EntityTreeItemData *, QMap<int, QJsonObject>>								m_KeyFramesMap;			// Store properties and tween values
	QMap<EntityTreeItemData *, QMap<int, QJsonObject>>								m_PoppedKeyFramesMap;	// Keep removed items' keyframes, in case they are re-added
	QMap<int, QStringList>															m_CallbackMap;			// KEY: frame index, VALUE: a list of strings that are the callback name(s)

	// These maps store the visual graphics items that correspond to the above maps
	QMap<KeyFrameKey, GraphicsKeyFrameItem *>										m_KeyFramesGfxRectMap;
	QMap<KeyFrameKey, GraphicsKeyFrameItem *>										m_TweenGfxRectMap;

	int																				m_iCurrentFrame;
	QGraphicsLineItem *																m_pCurrentFrameLine;

	int																				m_iFinalFrame;			// This is the last frame that has a keyframe or event. It includes tweens' duration and what would be the ending frame it finishes

public:
	EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj);
	virtual ~EntityDopeSheetScene();

	EntityStateData *GetStateData() const;

	int GetCurrentFrame() const;
	void SetCurrentFrame(int iFrameIndex);

	int GetFinalFrame() const;

	const QMap<EntityTreeItemData *, QMap<int, QJsonObject>> &GetKeyFramesMap() const;
	const QMap<int, QStringList> &GetCallbackMap() const;

	bool ContainsKeyFrameProperty(KeyFrameKey tupleKey) const;
	bool ContainsKeyFrameTween(KeyFrameKey tupleKey) const;

	// Based on currently selected keyframes, this function will determine if a tween can be created, and if so, what type of tween
	// Determines what quick-tween buttons should be shown in the AuxDopeSheet
	QList<ContextTweenData> DetermineIfContextQuickTween() const;

	// 'm_KeyFrameMap' must be fully updated before using this function
	QList<QPair<QString, QString>> GetUniquePropertiesList(EntityTreeItemData *pItemData, bool bCollapseTweenProps) const; // This is mainly useful for rendering the dope sheet. 'bCollapseTweenProps' will combine tween properties into a single entry (the regular category/property name)

	QJsonArray SerializeAllKeyFrames(EntityTreeItemData *pItemData) const;						// This QJsonArray layout will mimic the "stateArray"->"keyFrames"->"<GUID>" array in the Items.meta file
	QJsonObject SerializeSpecifiedKeyFrames(QList<QGraphicsItem *> specifiedFrameList) const;	// This QJsonObject layout will mimic the "stateArray"->"keyFrames" object in the Items.meta file
	QJsonObject GetCurrentFrameProperties(EntityTreeItemData *pItemData) const;
	QJsonValue GetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const;
	QJsonValue BasicExtrapolateKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const; // Only works on properties that don't tween, or interpolate values between key frames
	QMap<int, QMap<EntityTreeItemData *, QJsonObject>> GetKeyFrameMapPropertiesByFrame() const;

	void SetKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, QJsonObject propsObj);
	bool SetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, QJsonValue jsonValue, bool bRefreshGfxItems);
	void RemoveKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, bool bRefreshGfxItems);
	void RemoveKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, bool bRefreshGfxItems);
	void RemoveKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, bool bRefreshGfxItems);

	void PasteSerializedKeyFrames(EntityTreeItemData *pItemData, QJsonObject keyFrameMimeObj, int iStartFrameIndex);	// Inserts 'keyFrameMimeObj' frames into 'pItemData' - if 'iStartFrameIndex' is not negative, it will offset the paste to start at the specified frame
	void UnpasteSerializedKeyFrames(EntityTreeItemData *pItemData, QJsonObject keyFrameMimeObj, int iStartFrameIndex);	// Removes 'keyFrameMimeObj' frames from 'pItemData' - if 'iStartFrameIndex' is not negative, it will remove the paste that was offset to start at the specified frame
	void InsertSerializedKeyFrames(QJsonObject keyFrameMimeObj);									// Inserts 'keyFrameMimeObj' frames into the serialized items
	void RemoveSerializedKeyFrames(QJsonObject keyFrameMimeObj);									// Removes 'keyFrameMimeObj' frames from the serialized items

	TweenJsonValues GetTweenJsonValues(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp) const;
	void SetKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, const TweenJsonValues &tweenValues, bool bRefreshGfxItems);

	void PopAllKeyFrames(EntityTreeItemData *pItemData, bool bRefreshGfxItems);
	void PushAllKeyFrames(EntityTreeItemData *pItemData, bool bRefreshGfxItems);

	QJsonArray SerializeCallbacks() const;
	bool SetCallback(int iFrameIndex, QString sCallback);
	bool RemoveCallback(int iFrameIndex, QString sCallback);
	QStringList GetCallbackList(int iFrameIndex) const;
	bool RenameCallback(int iFrameIndex, QString sOldCallback, QString sNewCallback);

	// Returns any timeline event properties set on 'pItemData', which would be the root or a sub-entity
	QMap<int, QList<TimelineEvent>> AssembleTimelineEvents(EntityTreeItemData *pItemData) const;

	void NudgeKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, int iNudgeAmount, bool bRefreshGfxItems);
	void NudgeKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, int iNudgeAmount, bool bRefreshGfxItems);

	// pItemData - The item that is being selected, nullptr indicates all items
	// iSelectionPivotFrame - When >= 0, the value is to select prior or subsequent frames
	void SelectKeyFrames(bool bAppendSelection, EntityTreeItemData *pItemData, int iSelectionPivotFrame, bool bPivotLessThan);
	QList<EntityTreeItemData *> GetItemsFromSelectedFrames() const;

	void RefreshAllGfxItems();

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent) override;
};

#endif // ENTITYDOPESHEETSCENE_H
