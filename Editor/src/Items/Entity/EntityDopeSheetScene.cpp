/**************************************************************************
*	EntityDopeSheetScene.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityDopeSheetScene.h"
#include "EntityModel.h"
#include "EntityWidget.h"
#include "EntityDraw.h"
#include "EntityItemMimeData.h"

#include <QGraphicsRectItem>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>

GraphicsTweenKnobItem::GraphicsTweenKnobItem(KeyFrameKey tupleKey, QGraphicsItem *pParent /*= nullptr*/) :
	QGraphicsEllipseItem(-KEYFRAME_TWEEN_KNOB_RADIUS, -KEYFRAME_TWEEN_KNOB_RADIUS, KEYFRAME_TWEEN_KNOB_RADIUS * 2.0f, KEYFRAME_TWEEN_KNOB_RADIUS * 2.0f, pParent)
{
	setData(GFXDATAKEY_TreeItemData, QVariant::fromValue(std::get<GFXDATAKEY_TreeItemData>(tupleKey)));
	setData(GFXDATAKEY_FrameIndex, std::get<GFXDATAKEY_FrameIndex>(tupleKey));
	setData(GFXDATAKEY_CategoryPropString, std::get<GFXDATAKEY_CategoryPropString>(tupleKey));

	setData(GFXDATAKEY_Type, GFXITEM_TweenKnob);

	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	setBrush(HyGlobal::ConvertHyColor(HyColor::Green));
	setAcceptHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setFlags(QGraphicsItem::ItemIsSelectable);
}

/*virtual*/ GraphicsTweenKnobItem::~GraphicsTweenKnobItem()
{
}

KeyFrameKey GraphicsTweenKnobItem::GetKey() const
{
	return std::make_tuple(data(GFXDATAKEY_TreeItemData).value<EntityTreeItemData *>(),
						   data(GFXDATAKEY_FrameIndex).toInt(),
						   data(GFXDATAKEY_CategoryPropString).toString());
}

/*virtual*/ QVariant GraphicsTweenKnobItem::itemChange(GraphicsItemChange eChange, const QVariant &value) /*override*/
{
	switch(eChange)
	{
	case QGraphicsItem::ItemSelectedHasChanged:
		if(value.toBool())
			setCursor(Qt::SizeHorCursor);
		else
			unsetCursor();
		break;
	}

	return QGraphicsEllipseItem::itemChange(eChange, value);
}

/*virtual*/ void GraphicsTweenKnobItem::hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent) /*override*/
{
	setPen(HyGlobal::ConvertHyColor(HyColor::White));
	scene()->update();
}

/*virtual*/ void GraphicsTweenKnobItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent) /*override*/
{
	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	scene()->update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GraphicsKeyFrameItem::GraphicsKeyFrameItem(KeyFrameKey tupleKey, bool bIsTweenKeyFrame, QGraphicsItem *pParent /*= nullptr*/) :
	QGraphicsRectItem(0.0, 0.0, KEYFRAME_WIDTH, KEYFRAME_HEIGHT, pParent),
	m_pGfxTweenLine(nullptr),
	m_pGfxTweenDurationKnob(nullptr),
	m_iTweenFramesDuration(0)
{
	setData(GFXDATAKEY_TreeItemData, QVariant::fromValue(std::get<GFXDATAKEY_TreeItemData>(tupleKey)));
	setData(GFXDATAKEY_FrameIndex, std::get<GFXDATAKEY_FrameIndex>(tupleKey));
	setData(GFXDATAKEY_CategoryPropString, std::get<GFXDATAKEY_CategoryPropString>(tupleKey));

	setData(GFXDATAKEY_Type, bIsTweenKeyFrame ? GFXITEM_TweenKeyFrame : GFXITEM_PropertyKeyFrame);

	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	setBrush(HyGlobal::ConvertHyColor(bIsTweenKeyFrame ? HyColor::Green : HyColor::LightGray));
	setAcceptHoverEvents(true);
	setAcceptedMouseButtons(Qt::LeftButton);
	setFlags(QGraphicsItem::ItemIsSelectable); // Can't use 'QGraphicsItem::ItemIsMovable' because key frames are only allowed to move horizontally and snapped to frames

	if(bIsTweenKeyFrame)
	{
		m_pGfxTweenLine = new QGraphicsLineItem(this);
		m_pGfxTweenLine->setFlag(QGraphicsItem::ItemStacksBehindParent);

		QPen dashLinePen;
		dashLinePen.setStyle(Qt::DashLine);
		dashLinePen.setColor(HyGlobal::ConvertHyColor(HyColor::Green));
		m_pGfxTweenLine->setPen(dashLinePen);

		m_pGfxTweenDurationKnob = new GraphicsTweenKnobItem(tupleKey, this);
	}
}

/*virtual*/ GraphicsKeyFrameItem::~GraphicsKeyFrameItem()
{
	delete m_pGfxTweenDurationKnob;
	delete m_pGfxTweenLine;
}

KeyFrameKey GraphicsKeyFrameItem::GetKey() const
{
	return std::make_tuple(data(GFXDATAKEY_TreeItemData).value<EntityTreeItemData *>(),
						   data(GFXDATAKEY_FrameIndex).toInt(),
						   data(GFXDATAKEY_CategoryPropString).toString());
}

bool GraphicsKeyFrameItem::IsTweenKeyFrame() const
{
	return data(GFXDATAKEY_Type).toInt() == GFXITEM_TweenKeyFrame;
}

void GraphicsKeyFrameItem::SetTweenLineLength(qreal fLength, int iFramesDuration)
{
	m_pGfxTweenLine->setLine(0.0f, KEYFRAME_HEIGHT * 0.5f, fLength, KEYFRAME_HEIGHT * 0.5f);
	m_pGfxTweenDurationKnob->setPos(fLength, KEYFRAME_HEIGHT * 0.5f);
	m_iTweenFramesDuration = iFramesDuration;
}

int GraphicsKeyFrameItem::GetTweenFramesDuration() const
{
	return m_iTweenFramesDuration;
}

/*virtual*/ QVariant GraphicsKeyFrameItem::itemChange(GraphicsItemChange eChange, const QVariant &value) /*override*/
{
	switch(eChange)
	{
	case QGraphicsItem::ItemSelectedHasChanged:
		if(value.toBool())
			setCursor(Qt::SizeHorCursor);
		else
			unsetCursor();
		break;
	}

	return QGraphicsRectItem::itemChange(eChange, value);
}

/*virtual*/ void GraphicsKeyFrameItem::hoverEnterEvent(QGraphicsSceneHoverEvent *pEvent) /*override*/
{
	setPen(HyGlobal::ConvertHyColor(HyColor::White));
	scene()->update();
}

/*virtual*/ void GraphicsKeyFrameItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *pEvent) /*override*/
{
	setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	scene()->update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EntityDopeSheetScene::EntityDopeSheetScene(EntityStateData *pStateData, QJsonObject metaFileObj) :
	QGraphicsScene(),
	m_pEntStateData(pStateData),
	m_iCurrentFrame(0),
	m_pCurrentFrameLine(nullptr),
	m_iFinalFrame(0)
{
	QJsonObject keyFramesObj = metaFileObj["keyFrames"].toObject();
	for(auto iter = keyFramesObj.begin(); iter != keyFramesObj.end(); ++iter)
	{
		EntityTreeItemData *pItemData = static_cast<EntityModel &>(m_pEntStateData->GetModel()).GetTreeModel().FindTreeItemData(QUuid(iter.key()));
		if(pItemData == nullptr)
		{
			HyGuiLog("EntityStateData::EntityStateData - item " % iter.key() % " was not found", LOGTYPE_Error);
			continue;
		}

		QJsonArray keyFramesArray = iter.value().toArray();
		for(int i = 0; i < keyFramesArray.size(); ++i)
		{
			const QJsonObject &propsObj = keyFramesArray[i].toObject()["props"].toObject();
			if(propsObj.isEmpty() == false)
				m_KeyFramesMap[pItemData][keyFramesArray[i].toObject()["frame"].toInt()] = propsObj;
			else
				HyGuiLog("EntityStateData::EntityStateData - item " % iter.key() % " has an empty keyframe at frame " % QString::number(keyFramesArray[i].toObject()["frame"].toInt()), LOGTYPE_Info);
		}
	}

	QJsonArray callbacksArray = metaFileObj["callbacks"].toArray();
	for(int i = 0; i < callbacksArray.size(); ++i)
	{
		const QJsonObject &callbacksObj = callbacksArray[i].toObject();

		QJsonArray functionsArray = callbacksObj["functions"].toArray();
		for(int iFuncIndex = 0; iFuncIndex < functionsArray.size(); ++iFuncIndex)
		{
			QString sFunc = functionsArray[iFuncIndex].toString();
			SetCallback(callbacksObj["frame"].toInt(), sFunc);
		}
	}

	setBackgroundBrush(HyGlobal::ConvertHyColor(HyColor::WidgetPanel));

	// These lines allow QGraphicsView to align itself to the top-left corner of the scene
	addLine(0.0, 0.0, 10.0, 0.0f)->setAcceptedMouseButtons(Qt::NoButton);
	addLine(0.0, 0.0, 0.0, 10.0f)->setAcceptedMouseButtons(Qt::NoButton);
	m_pCurrentFrameLine = addLine(0.0f, 0.0f, 0.0f, 10.0f); // This line will hide behind the timeline, but is placed in "scene space" so it will embiggen the sceneRect which sets QGraphicsViews' scrollbars accordingly
	m_pCurrentFrameLine->setAcceptedMouseButtons(Qt::NoButton);

	RefreshAllGfxItems();
	SetCurrentFrame(0);
}

/*virtual*/ EntityDopeSheetScene::~EntityDopeSheetScene()
{
}

EntityStateData *EntityDopeSheetScene::GetStateData() const
{
	return m_pEntStateData;
}

int EntityDopeSheetScene::GetCurrentFrame() const
{
	return m_iCurrentFrame;
}

void EntityDopeSheetScene::SetCurrentFrame(int iFrame)
{
	m_iCurrentFrame = HyMath::Max(iFrame, 0);
	m_pCurrentFrameLine->setPos(TIMELINE_LEFT_MARGIN + (m_iCurrentFrame * TIMELINE_NOTCH_SUBLINES_WIDTH), 0.0f);
	update();

	IWidget *pWidget = m_pEntStateData->GetModel().GetItem().GetWidget();
	if(pWidget)
		static_cast<EntityWidget *>(pWidget)->SetExtrapolatedProperties();

	IDraw *pDraw = m_pEntStateData->GetModel().GetItem().GetDraw();
	if(pDraw)
		static_cast<EntityDraw *>(pDraw)->SetExtrapolatedProperties(false);
}

int EntityDopeSheetScene::GetFinalFrame() const
{
	return m_iFinalFrame;
}

const QMap<EntityTreeItemData *, QMap<int, QJsonObject>> &EntityDopeSheetScene::GetKeyFramesMap() const
{
	return m_KeyFramesMap;
}

const QMap<int, QStringList> &EntityDopeSheetScene::GetCallbackMap() const
{
	return m_CallbackMap;
}

bool EntityDopeSheetScene::ContainsKeyFrameProperty(KeyFrameKey tupleKey) const
{
	return m_KeyFramesGfxRectMap.contains(tupleKey);
}

bool EntityDopeSheetScene::ContainsKeyFrameTween(KeyFrameKey tupleKey) const
{
	return m_TweenGfxRectMap.contains(tupleKey);
}

QList<ContextTweenData> EntityDopeSheetScene::DetermineIfContextQuickTween() const
{
	QList<ContextTweenData> contextTweenList;

	QList<QGraphicsItem *> selectedItemsList = selectedItems();

	// Sort the selected items by frame index
	std::sort(selectedItemsList.begin(), selectedItemsList.end(), [](QGraphicsItem *pA, QGraphicsItem *pB) -> bool {
		return pA->data(GFXDATAKEY_FrameIndex).toInt() < pB->data(GFXDATAKEY_FrameIndex).toInt();
	});

	for(QGraphicsItem *pGfxItem : selectedItemsList)
	{
		bool bAcquiredDataType = false;
		
		DopeSheetGfxItemType eItemType = static_cast<DopeSheetGfxItemType>(pGfxItem->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
		if(bAcquiredDataType == false)
			continue;

		if(eItemType == GFXITEM_TweenKnob)
		{
			pGfxItem = pGfxItem->parentItem(); // If item is a tween knob, reassign it to its parent (which is a tween keyframe)
			eItemType = GFXITEM_TweenKeyFrame;
		}
		int iStartFrame = pGfxItem->data(GFXDATAKEY_FrameIndex).toInt();
		int iEndFrame = iStartFrame;
		if(static_cast<GraphicsKeyFrameItem *>(pGfxItem)->IsTweenKeyFrame())
			iEndFrame += static_cast<GraphicsKeyFrameItem *>(pGfxItem)->GetTweenFramesDuration();

		// Compare against all other selected items to see if they are tweenable
		bool bFoundQuickTween = false;
		for(QGraphicsItem *pGfxItem2 : selectedItemsList)
		{
			if(pGfxItem == pGfxItem2)
				continue;

			// The right side item must be a regular property keyframe
			DopeSheetGfxItemType eItemType2 = static_cast<DopeSheetGfxItemType>(pGfxItem2->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
			if(bAcquiredDataType == false || eItemType2 != GFXITEM_PropertyKeyFrame)
				continue;

			int iFrame2 = pGfxItem2->data(GFXDATAKEY_FrameIndex).toInt();
			if(iEndFrame >= iFrame2)
				continue;

			// Collect all the info stored in the FrameKey for 'left' item
			KeyFrameKey tupleKey = static_cast<GraphicsKeyFrameItem *>(pGfxItem)->GetKey();
			QString sCategoryProp = std::get<GFXDATAKEY_CategoryPropString>(tupleKey);

			// Collect all the info stored in the FrameKey for 'right' item
			KeyFrameKey tupleKey2 = static_cast<GraphicsKeyFrameItem *>(pGfxItem2)->GetKey();
			QString sCategoryProp2 = std::get<GFXDATAKEY_CategoryPropString>(tupleKey2);

			// Final checks to determine if a quick-tween button should be shown, and if so, which one
			if(std::get<GFXDATAKEY_TreeItemData>(tupleKey) == std::get<GFXDATAKEY_TreeItemData>(tupleKey2) &&
				sCategoryProp == sCategoryProp2)
			{
				TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sCategoryProp.split('/')[1]);
				if(eTweenProp == TWEENPROP_None)
					continue;

				// Ensure that the 'left' and 'right' keyframes are adjacent to each other
				const QMap<int, QJsonObject> &treeItemFramesMapRef = m_KeyFramesMap[std::get<GFXDATAKEY_TreeItemData>(tupleKey)];
				QList<int> frameIndicesList = treeItemFramesMapRef.keys();
				if(frameIndicesList.indexOf(iStartFrame) + 1 != frameIndicesList.indexOf(iFrame2))
					continue;

				// All checks complete - valid context tween: determine the start and end values of the tween
				QJsonValue startValue;
				if(eItemType == GFXITEM_PropertyKeyFrame)
					startValue = GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey), iStartFrame, sCategoryProp.split('/')[0], sCategoryProp.split('/')[1]);
				else if(eItemType == GFXITEM_TweenKeyFrame)
					startValue = GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey), iStartFrame, "Tween " + HyGlobal::TweenPropName(eTweenProp), "Destination");
				
				QJsonValue endValue = GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey2), iFrame2, sCategoryProp2.split('/')[0], sCategoryProp2.split('/')[1]);

				// Finally append the ContextTweenData
				contextTweenList.push_back(ContextTweenData(false, std::get<GFXDATAKEY_TreeItemData>(tupleKey), eTweenProp, iEndFrame, startValue, iFrame2, endValue));
				bFoundQuickTween = true;
			}
		}

		// Finally check for 'break tween'
		if(bFoundQuickTween == false && eItemType == GFXITEM_TweenKeyFrame)
		{
			KeyFrameKey tupleKey = static_cast<GraphicsKeyFrameItem *>(pGfxItem)->GetKey();
			QString sCategoryProp = std::get<GFXDATAKEY_CategoryPropString>(tupleKey);

			TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sCategoryProp.split('/')[1]);

			// Determine start value - Requires a regular keyframe of the same property on this frame, or a previous tween which finishes on this frame
			QJsonValue startValue;

			const QMap<int, QJsonObject> &treeItemFramesMapRef = m_KeyFramesMap[std::get<GFXDATAKEY_TreeItemData>(tupleKey)];
			QList<int> frameIndicesList = treeItemFramesMapRef.keys();

			QPair<QString, QString> regPropPair = HyGlobal::ConvertTweenPropToRegularPropPair(eTweenProp);
			QJsonValue regPropValue = GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey), iStartFrame, regPropPair.first, regPropPair.second);
			if(regPropValue.isUndefined() == false && regPropValue.isNull() == false)
				startValue = regPropValue;
			else if(frameIndicesList.indexOf(iStartFrame) > 0)// test for previous tween which finishes on this frame
			{
				int iPrevFrameIndex = frameIndicesList[frameIndicesList.indexOf(iStartFrame) - 1];
				TweenJsonValues tweenJsonValues = GetTweenJsonValues(std::get<GFXDATAKEY_TreeItemData>(tupleKey), iPrevFrameIndex, eTweenProp);
				if(tweenJsonValues.m_Destination.isUndefined() == false && tweenJsonValues.m_Destination.isNull() == false)
					startValue = tweenJsonValues.m_Destination;
				else // Finally check for a previous regular keyframe property
					startValue = GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey), iPrevFrameIndex, regPropPair.first, regPropPair.second);
			}

			// The end value is the current tween's destination
			QJsonValue endValue = GetKeyFrameProperty(std::get<GFXDATAKEY_TreeItemData>(tupleKey), iStartFrame, "Tween " + HyGlobal::TweenPropName(eTweenProp), "Destination");

			// Append a ContextTweenData that is for a 'break tween' indicated by the first boolean
			contextTweenList.push_back(ContextTweenData(true, std::get<GFXDATAKEY_TreeItemData>(tupleKey), eTweenProp, std::get<GFXDATAKEY_FrameIndex>(tupleKey), startValue, iEndFrame, endValue));
		}
	}

	return contextTweenList;
}

// NOTE: Tween properties are represented by a single property keyframe
QList<QPair<QString, QString>> EntityDopeSheetScene::GetUniquePropertiesList(EntityTreeItemData *pItemData, bool bCollapseTweenProps) const
{
	QSet<QPair<QString, QString>> uniquePropertiesSet;
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QList<QPair<QString, QString>>();

	QList<QJsonObject> propsObjList = m_KeyFramesMap[pItemData].values();
	for(QJsonObject propsObj : propsObjList)
	{
		QStringList sCategoryList = propsObj.keys();
		for(QString sCategoryName : sCategoryList)
		{
			// NOTE: If this category is a tween, skip over all the properties and "insert" a single QPair<> to represent it
			if(bCollapseTweenProps && sCategoryName.startsWith("Tween "))
			{
				// Match the tween category to its corresponding property
				QString sTweenProp = sCategoryName.mid(6);
				TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sTweenProp);
				uniquePropertiesSet.insert(HyGlobal::ConvertTweenPropToRegularPropPair(eTweenProp));
			}
			else
			{
				QJsonObject categoryObj = propsObj[sCategoryName].toObject();
				QStringList sPropList = categoryObj.keys();
				for(QString sPropName : sPropList)
				{
					QPair<QString, QString> newPair(sCategoryName, sPropName);
					uniquePropertiesSet.insert(newPair);
				}
			}
		}
	}

	QList<QPair<QString, QString>> uniquePropertiesList = uniquePropertiesSet.values();

	// Sort 'uniquePropertiesList' to match how it is laid out in the Property Tree View
	EntityPropertiesTreeModel &propertiesModelRef = pItemData->GetPropertiesModel();
	std::sort(uniquePropertiesList.begin(), uniquePropertiesList.end(), [&](const QPair<QString, QString> &a, const QPair<QString, QString> &b) -> bool
	{
		QModelIndex indexA = propertiesModelRef.FindPropertyModelIndex(a.first, a.second);
		QModelIndex indexB = propertiesModelRef.FindPropertyModelIndex(b.first, b.second);

		if(indexA.parent().row() == indexB.parent().row())
			return indexA.row() < indexA.row();
		return indexA.parent().row() < indexB.parent().row();
	});

	return uniquePropertiesList;
}

QJsonArray EntityDopeSheetScene::SerializeAllKeyFrames(EntityTreeItemData *pItemData) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonArray();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];

	QJsonArray serializedKeyFramesArray;
	for(QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.begin(); iter != itemKeyFrameMapRef.end(); ++iter)
	{
		if(iter.value().isEmpty())
			continue;

		QJsonObject serializedKeyFramesObj;
		serializedKeyFramesObj["frame"] = iter.key();
		serializedKeyFramesObj["props"] = iter.value();
		serializedKeyFramesArray.append(serializedKeyFramesObj);
	}

	return serializedKeyFramesArray;
}

QJsonObject EntityDopeSheetScene::SerializeSpecifiedKeyFrames(QList<QGraphicsItem *> specifiedFrameList) const
{
	QMap<QUuid, QJsonArray> serializedKeyFrameMap; // Store specifiedFrameList properties into this

	if(specifiedFrameList.isEmpty())
		return QJsonObject();

	// Sort 'specifiedFrameList' by X position
	std::sort(specifiedFrameList.begin(), specifiedFrameList.end(),
		[](const QGraphicsItem *a, const QGraphicsItem *b) -> bool
		{
			return a->scenePos().x() < b->scenePos().x();
		});

	for(QGraphicsItem *pItem : specifiedFrameList)
	{
		// Only process 'GraphicsKeyFrameItem' types
		bool bAcquiredDataType = false;
		DopeSheetGfxItemType eItemType = static_cast<DopeSheetGfxItemType>(pItem->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
		if(bAcquiredDataType == false || (eItemType != GFXITEM_PropertyKeyFrame && eItemType != GFXITEM_TweenKeyFrame))
			continue;
		
		// Extract key frame data
		KeyFrameKey tupleKey = static_cast<GraphicsKeyFrameItem *>(pItem)->GetKey();
		EntityTreeItemData *pCurItemData = std::get<GFXDATAKEY_TreeItemData>(tupleKey);
		int iFrameIndex = std::get<GFXDATAKEY_FrameIndex>(tupleKey);
		QString sCategory = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[0];
		QString sProperty = std::get<GFXDATAKEY_CategoryPropString>(tupleKey).split('/')[1];

		// Nested serialize function
		std::function<void(EntityTreeItemData *, int, QString, QString)> fpSerializeProperty =
			[&](EntityTreeItemData *pCurItemData, int iFrameIndex, QString sCategory, QString sProperty)
			{
				if(serializedKeyFrameMap.contains(pCurItemData->GetUuid()) == false)
					serializedKeyFrameMap[pCurItemData->GetUuid()] = QJsonArray();

				QJsonArray &serializedKeyFramesArrayRef = serializedKeyFrameMap[pCurItemData->GetUuid()];

				// Look for existing 'frame' index in 'serializedKeyFramesArrayRef' and append to 'props' if found
				bool bFoundFrame = false;
				for(int i = 0; i < serializedKeyFramesArrayRef.size(); ++i)
				{
					QJsonObject serializedKeyFrameObj = serializedKeyFramesArrayRef[i].toObject();
					if(serializedKeyFrameObj["frame"].toInt() == iFrameIndex)
					{
						QJsonObject sceneCategoryObj = m_KeyFramesMap[pCurItemData][iFrameIndex][sCategory].toObject();

						// If this category already exists, then merge the property
						if(serializedKeyFrameObj["props"].toObject().contains(sCategory))
						{
							QJsonObject serializedPropsObj = serializedKeyFrameObj["props"].toObject();
							QJsonObject serializedCategoryObj = serializedPropsObj[sCategory].toObject();

							if(sceneCategoryObj.contains(sProperty) == false)
								HyGuiLog("EntityDopeSheetScene::SerializeSelectedKeyFrames() - sceneCategoryObj does not contain property: " % sProperty, LOGTYPE_Error);

							serializedCategoryObj.insert(sProperty, sceneCategoryObj[sProperty]);
							serializedPropsObj.insert(sCategory, serializedCategoryObj);

							serializedKeyFrameObj.insert("props", serializedPropsObj); // Overwrite the 'props' with the merged properties
						}
						else // Otherwise, just add the category and property
						{
							if(sceneCategoryObj.contains(sProperty) == false)
								HyGuiLog("EntityDopeSheetScene::SerializeSelectedKeyFrames() - SceneCategoryPropsObj does not contain category: " % sCategory, LOGTYPE_Error);

							QJsonObject serializedCategoryObj;
							serializedCategoryObj.insert(sProperty, sceneCategoryObj[sProperty]);
							QJsonObject serializedPropsObj = serializedKeyFrameObj["props"].toObject();
							serializedPropsObj.insert(sCategory, serializedCategoryObj);

							serializedKeyFrameObj.insert("props", serializedPropsObj); // Overwrite the 'props' with the merged properties
						}

						// Overwrite the 'frame' with the merged properties
						serializedKeyFramesArrayRef.removeAt(i);
						serializedKeyFramesArrayRef.insert(i, serializedKeyFrameObj);
						bFoundFrame = true;
						break;
					}
				}

				if(bFoundFrame == false)
				{
					QJsonObject propertyObj;
					propertyObj.insert(sProperty, m_KeyFramesMap[pCurItemData][iFrameIndex][sCategory].toObject()[sProperty]);

					QJsonObject categoryObj;
					categoryObj.insert(sCategory, propertyObj);

					QJsonObject serializedKeyFramesObj;
					serializedKeyFramesObj["frame"] = iFrameIndex;
					serializedKeyFramesObj["props"] = categoryObj;
					serializedKeyFramesArrayRef.append(serializedKeyFramesObj);
				}
			}; // fpSerializeProperty
			
		// Using above serialize function, serialize the property
		if(eItemType == GFXITEM_TweenKeyFrame)
		{
			// If this is a tween key frame, then we need to serialize all 3 corresponding tween properties: "Destination", "Duration", "Tween Type"
			sCategory = "Tween " + sProperty;

			fpSerializeProperty(pCurItemData, iFrameIndex, sCategory, "Destination");
			fpSerializeProperty(pCurItemData, iFrameIndex, sCategory, "Duration");
			fpSerializeProperty(pCurItemData, iFrameIndex, sCategory, "Tween Type");
		}
		else
			fpSerializeProperty(pCurItemData, iFrameIndex, sCategory, sProperty);
	}

	QJsonObject serializedKeyFramesObj; // This layout will mimic "stateArray" -> "keyFrames" object in the Items.meta file
	for(auto iter = serializedKeyFrameMap.begin(); iter != serializedKeyFrameMap.end(); ++iter)
		serializedKeyFramesObj.insert(iter.key().toString(QUuid::WithoutBraces), iter.value());

	return serializedKeyFramesObj;
}

QJsonObject EntityDopeSheetScene::GetCurrentFrameProperties(EntityTreeItemData *pItemData) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonObject();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.find(m_iCurrentFrame);
	if(iter == itemKeyFrameMapRef.end())
		return QJsonObject();

	return iter.value();
}

QJsonValue EntityDopeSheetScene::GetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonValue();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	if(itemKeyFrameMapRef.contains(iFrameIndex) == false)
		return QJsonValue();

	if(itemKeyFrameMapRef[iFrameIndex].contains(sCategoryName) == false)
		return QJsonValue();

	if(itemKeyFrameMapRef[iFrameIndex][sCategoryName].toObject().contains(sPropName) == false)
		return QJsonValue();

	return itemKeyFrameMapRef[iFrameIndex][sCategoryName].toObject()[sPropName];
}

QJsonValue EntityDopeSheetScene::BasicExtrapolateKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QJsonValue();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];

	// Get the closest key frame that is less than or equal to 'iFrameIndex'
	QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.find(iFrameIndex);
	if(iter == itemKeyFrameMapRef.end())
	{
		// lowerBound() - Returns an iterator pointing to the first item with key 'iFrameIndex' in the map.
		//                If the map contains no item with key 'iFrameIndex', the function returns an iterator
		//                to the nearest item with a greater key.
		iter = itemKeyFrameMapRef.lowerBound(iFrameIndex);
		if(iter != itemKeyFrameMapRef.begin())
			iter--; // Don't want an iterator with a greater key, so go back one
	}

	if(iter == itemKeyFrameMapRef.end())
		return QJsonValue();

	// Starting with this key frame and going backwards in time, search for the property 'sCategoryName/sPropName' and return the value
	while(true)
	{
		QJsonObject curKeyFrameObj = iter.value();
		if(curKeyFrameObj.contains(sCategoryName) && curKeyFrameObj[sCategoryName].toObject().contains(sPropName))
			return curKeyFrameObj[sCategoryName].toObject()[sPropName];

		if(iter == itemKeyFrameMapRef.begin())
			break;
		iter--;
	}

	return QJsonValue();
}

QMap<int, QMap<EntityTreeItemData *, QJsonObject>> EntityDopeSheetScene::GetKeyFrameMapPropertiesByFrame() const
{
	QMap<int, QMap<EntityTreeItemData *, QJsonObject>> keyFrameMapPropertiesByFrame;

	for(auto iterItem = m_KeyFramesMap.begin(); iterItem != m_KeyFramesMap.end(); ++iterItem)
	{
		EntityTreeItemData *pItemData = iterItem.key();
		const QMap<int, QJsonObject> &itemKeyFrameMapRef = iterItem.value();
		for(auto iterKeyFrame = itemKeyFrameMapRef.begin(); iterKeyFrame != itemKeyFrameMapRef.end(); ++iterKeyFrame)
		{
			int iFrameIndex = iterKeyFrame.key();
			const QJsonObject &keyFrameObjRef = iterKeyFrame.value();
			if(keyFrameMapPropertiesByFrame.contains(iFrameIndex) == false)
				keyFrameMapPropertiesByFrame.insert(iFrameIndex, QMap<EntityTreeItemData *, QJsonObject>());
			keyFrameMapPropertiesByFrame[iFrameIndex].insert(pItemData, keyFrameObjRef);
		}
	}

	return keyFrameMapPropertiesByFrame;
}

void EntityDopeSheetScene::SetKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, QJsonObject propsObj)
{
	QJsonObject curPropsObj = m_KeyFramesMap[pItemData][iFrameIndex];

	if(curPropsObj.empty() && propsObj.empty())
	{
		m_KeyFramesMap[pItemData].remove(iFrameIndex);
		return;
	}

	// Merge 'propsObj' into 'curPropsObj', overwrite any properties that are already set, but preserve any properties in that aren't
	for(auto iterCategory = propsObj.begin(); iterCategory != propsObj.end(); ++iterCategory)
	{
		// Check if category exists
		if(curPropsObj.contains(iterCategory.key()))
		{
			// Category already exists, take existing category ('curPropsObj') object and merge it with passed-in 'iterCategory'
			QJsonObject curCategoryObj = curPropsObj[iterCategory.key()].toObject();
			QJsonObject newCategoryObj = iterCategory.value().toObject();
			for(auto iterProp = newCategoryObj.begin(); iterProp != newCategoryObj.end(); ++iterProp)
				curCategoryObj.insert(iterProp.key(), iterProp.value()); // Update the category with new properties
			
			curPropsObj.insert(iterCategory.key(), curCategoryObj);
		}
		else
			curPropsObj.insert(iterCategory.key(), iterCategory.value());
	}

	if(curPropsObj.empty())
		HyGuiLog("EntityDopeSheetScene::SetKeyFrameProperties() - curPropsObj is empty", LOGTYPE_Error);

	m_KeyFramesMap[pItemData][iFrameIndex] = curPropsObj;

	RefreshAllGfxItems();
}

bool EntityDopeSheetScene::SetKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, QJsonValue jsonValue, bool bRefreshGfxItems)
{
	bool bIsNewKeyFrame = false;

	if(m_KeyFramesMap.contains(pItemData) == false)
		m_KeyFramesMap.insert(pItemData, QMap<int, QJsonObject>());

	QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	if(itemKeyFrameMapRef.contains(iFrameIndex) == false)
		itemKeyFrameMapRef.insert(iFrameIndex, QJsonObject());

	QJsonObject &keyFrameObjRef = itemKeyFrameMapRef[iFrameIndex];
	if(keyFrameObjRef.contains(sCategoryName) == false)
	{
		keyFrameObjRef.insert(sCategoryName, QJsonObject());
		bIsNewKeyFrame = true;
	}
	else if(keyFrameObjRef[sCategoryName].toObject().contains(sPropName) == false)
		bIsNewKeyFrame = true;

	QJsonObject categoryObj = keyFrameObjRef[sCategoryName].toObject();
	categoryObj.insert(sPropName, jsonValue);

	keyFrameObjRef.insert(sCategoryName, categoryObj);

	if(bRefreshGfxItems)
		RefreshAllGfxItems();

	return bIsNewKeyFrame;
}

void EntityDopeSheetScene::RemoveKeyFrameProperties(EntityTreeItemData *pItemData, int iFrameIndex, bool bRefreshGfxItems)
{
	if(m_KeyFramesMap.contains(pItemData) == false || m_KeyFramesMap[pItemData].contains(iFrameIndex) == false)
		return;

	QJsonObject keyFrameObjCopy = m_KeyFramesMap[pItemData][iFrameIndex];
	for(auto iterCategory = keyFrameObjCopy.begin(); iterCategory != keyFrameObjCopy.end(); ++iterCategory)
	{
		QString sCategoryName = iterCategory.key();
		QJsonObject &categoryObjRef = iterCategory.value().toObject();
		QStringList sPropList = categoryObjRef.keys();
		for(int i = 0; i < sPropList.size(); ++i)
			RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, sPropList[i], bRefreshGfxItems && (i == (sPropList.size()-1)));
	}
}

void EntityDopeSheetScene::RemoveKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, bool bRefreshGfxItems)
{
	if(m_KeyFramesMap.contains(pItemData) == false || m_KeyFramesMap[pItemData].contains(iFrameIndex) == false)
		return;

	QJsonObject &keyFrameObjRef = m_KeyFramesMap[pItemData][iFrameIndex];
	if(keyFrameObjRef.empty())
	{
		m_KeyFramesMap[pItemData].remove(iFrameIndex);
		pItemData->SetReallocateDrawItem(true);
		return;
	}

	if(keyFrameObjRef.contains(sCategoryName) == false)
		return;

	QJsonObject categoryObj = keyFrameObjRef[sCategoryName].toObject();
	categoryObj.remove(sPropName);

	if(categoryObj.isEmpty())
	{
		keyFrameObjRef.remove(sCategoryName);
		if(keyFrameObjRef.isEmpty())
		{
			m_KeyFramesMap[pItemData].remove(iFrameIndex);
			if(m_KeyFramesMap[pItemData].isEmpty())
				m_KeyFramesMap.remove(pItemData);
		}
	}
	else
		keyFrameObjRef.insert(sCategoryName, categoryObj);

	pItemData->SetReallocateDrawItem(true);

	// Remove the corresponding gfx rect for this property
	KeyFrameKey gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, sCategoryName % "/" % sPropName);
	if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey))
	{
		removeItem(m_KeyFramesGfxRectMap[gfxRectMapKey]);
		delete m_KeyFramesGfxRectMap[gfxRectMapKey];
		m_KeyFramesGfxRectMap.remove(gfxRectMapKey);
	}

	// Tweens
	if(sCategoryName.startsWith("Tween "))
	{
		QString sTweenName = sCategoryName.mid(6);
		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sTweenName);
		QPair<QString, QString> tweenPair = HyGlobal::ConvertTweenPropToRegularPropPair(eTweenProp);
		gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, tweenPair.first % "/" % tweenPair.second);
		if(m_TweenGfxRectMap.contains(gfxRectMapKey))
		{
			removeItem(m_TweenGfxRectMap[gfxRectMapKey]);
			delete m_TweenGfxRectMap[gfxRectMapKey];
			m_TweenGfxRectMap.remove(gfxRectMapKey);
		}
	}

	if(bRefreshGfxItems)
		RefreshAllGfxItems();
}

void EntityDopeSheetScene::RemoveKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, bool bRefreshGfxItems)
{
	QString sCategoryName = "Tween " % HyGlobal::TweenPropName(eTweenProp);

	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Destination", false);
	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Duration", false);
	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Tween Type", bRefreshGfxItems);
}

void EntityDopeSheetScene::PasteSerializedKeyFrames(EntityTreeItemData *pItemData, QJsonObject keyFrameMimeObj, int iStartFrameIndex)
{
	if(pItemData == nullptr)
	{
		HyGuiLog("EntityDopeSheetScene::PasteSerializedKeyFrames() - pItemData is nullptr", LOGTYPE_Error);
		return;
	}

	if(keyFrameMimeObj.size() != 1)
	{
		HyGuiLog("EntityDopeSheetScene::PasteSerializedKeyFrames() - Invalid keyFrameMimeObj size", LOGTYPE_Error);
		return;
	}

	if(m_KeyFramesMap.contains(pItemData) == false)
		m_KeyFramesMap.insert(pItemData, QMap<int, QJsonObject>());

	QJsonArray frameDataArray = keyFrameMimeObj.begin()->toArray();

	int iFrameOffset = 0;
	if(iStartFrameIndex >= 0 && frameDataArray.count() > 0)
	{
		int iPasteStartFrame = frameDataArray[0].toObject()["frame"].toInt();
		iFrameOffset = iStartFrameIndex - iPasteStartFrame;
	}
	
	for(int i = 0; i < frameDataArray.size(); ++i)
	{
		QJsonObject frameDataObj = frameDataArray[i].toObject();

		QJsonObject propsObj = frameDataObj["props"].toObject();
		for(QString sCategory : propsObj.keys())
		{
			QJsonObject categoryObj = propsObj[sCategory].toObject();

			for(QString sPropName : categoryObj.keys())
			{
				// First determine if this frame has valid properties for 'pItemData'
				if(pItemData->GetPropertiesModel().FindPropertyDefinition(sCategory, sPropName).IsValid())
					SetKeyFrameProperty(pItemData, frameDataObj["frame"].toInt() + iFrameOffset, sCategory, sPropName, categoryObj[sPropName], false);
			}
		}
	}

	RefreshAllGfxItems();
}

void EntityDopeSheetScene::UnpasteSerializedKeyFrames(EntityTreeItemData *pItemData, QJsonObject keyFrameMimeObj, int iStartFrameIndex)
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return;

	if(keyFrameMimeObj.size() != 1)
	{
		HyGuiLog("EntityDopeSheetScene::UnpasteSerializedKeyFrames() - Invalid keyFrameMimeObj size", LOGTYPE_Error);
		return;
	}

	QJsonArray frameDataArray = keyFrameMimeObj.begin()->toArray();

	int iFrameOffset = 0;
	if(iStartFrameIndex >= 0 && frameDataArray.count() > 0)
	{
		int iPasteStartFrame = frameDataArray[0].toObject()["frame"].toInt();
		iFrameOffset = iStartFrameIndex - iPasteStartFrame;
	}

	for(int i = 0; i < frameDataArray.size(); ++i)
	{
		QJsonObject frameDataObj = frameDataArray[i].toObject();
		QJsonObject propsObj = frameDataObj["props"].toObject();
		for(QString sCategory : propsObj.keys())
		{
			QJsonObject categoryObj = propsObj[sCategory].toObject();
			for(QString sPropName : categoryObj.keys())
			{
				// First determine if this frame has valid properties for 'pItemData'
				if(pItemData->GetPropertiesModel().FindPropertyDefinition(sCategory, sPropName).IsValid())
					RemoveKeyFrameProperty(pItemData, frameDataObj["frame"].toInt() + iFrameOffset, sCategory, sPropName, false);
			}
		}
	}

	RefreshAllGfxItems();
}

void EntityDopeSheetScene::InsertSerializedKeyFrames(QJsonObject keyFrameMimeObj)
{
	for(auto iter = keyFrameMimeObj.begin(); iter != keyFrameMimeObj.end(); ++iter)
	{
		QJsonArray frameDataArray = iter.value().toArray();
		for(int i = 0; i < frameDataArray.size(); ++i)
		{
			EntityTreeItemData *pItemData = static_cast<EntityModel &>(GetStateData()->GetModel()).GetTreeModel().FindTreeItemData(QUuid(iter.key()));
			if(pItemData == nullptr)
			{
				HyGuiLog("EntityDopeSheetScene::InsertSerializedKeyFrames() - pItemData is nullptr", LOGTYPE_Error);
				continue;
			}

			QJsonObject frameDataObj = frameDataArray[i].toObject();
			QJsonObject propsObj = frameDataObj["props"].toObject();
			for(QString sCategory : propsObj.keys())
			{
				QJsonObject categoryObj = propsObj[sCategory].toObject();
				for(QString sPropName : categoryObj.keys())
				{
					// First determine if this frame has valid properties for 'pItemData'
					if(pItemData->GetPropertiesModel().FindPropertyDefinition(sCategory, sPropName).IsValid())
						SetKeyFrameProperty(pItemData, frameDataObj["frame"].toInt(), sCategory, sPropName, categoryObj[sPropName], false);
				}
			}
		}
	}

	RefreshAllGfxItems();
}

void EntityDopeSheetScene::RemoveSerializedKeyFrames(QJsonObject keyFrameMimeObj)
{
	for(auto iter = keyFrameMimeObj.begin(); iter != keyFrameMimeObj.end(); ++iter)
	{
		QJsonArray frameDataArray = iter.value().toArray();
		for(int i = 0; i < frameDataArray.size(); ++i)
		{
			EntityTreeItemData *pItemData = static_cast<EntityModel &>(GetStateData()->GetModel()).GetTreeModel().FindTreeItemData(QUuid(iter.key()));
			if(pItemData == nullptr)
			{
				HyGuiLog("EntityDopeSheetScene::RemoveSerializedKeyFrames() - pItemData is nullptr", LOGTYPE_Error);
				continue;
			}

			QJsonObject frameDataObj = frameDataArray[i].toObject();
			QJsonObject propsObj = frameDataObj["props"].toObject();
			for(QString sCategory : propsObj.keys())
			{
				QJsonObject categoryObj = propsObj[sCategory].toObject();
				for(QString sPropName : categoryObj.keys())
					RemoveKeyFrameProperty(pItemData, frameDataObj["frame"].toInt(), sCategory, sPropName, false);
			}
		}
	}

	RefreshAllGfxItems();
}

TweenJsonValues EntityDopeSheetScene::GetTweenJsonValues(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp) const
{
	QString sCategoryName = "Tween " % HyGlobal::TweenPropName(eTweenProp);

	QJsonValue destinationValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Destination");
	QJsonValue durationValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Duration");
	QJsonValue tweenTypeValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Tween Type");

	return TweenJsonValues(destinationValue, durationValue, tweenTypeValue);
}

void EntityDopeSheetScene::SetKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, const TweenJsonValues &tweenValues, bool bRefreshGfxItems)
{
	QString sCategoryName = "Tween " % HyGlobal::TweenPropName(eTweenProp);

	SetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Destination", tweenValues.m_Destination, false);
	SetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Duration", tweenValues.m_Duration, false);
	SetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Tween Type", tweenValues.m_TweenFuncType, bRefreshGfxItems);
}

void EntityDopeSheetScene::PopAllKeyFrames(EntityTreeItemData *pItemData, bool bRefreshGfxItems)
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return;

	// Store the "popped" key frames in case the user wants to readd ("Push") this item back
	m_PoppedKeyFramesMap[pItemData] = m_KeyFramesMap[pItemData];

	QMap<int, QJsonObject> &keyFrameMapRef = m_KeyFramesMap[pItemData];
	QList<int> frameIndexList = keyFrameMapRef.keys();
	for(int i = 0; i < frameIndexList.size(); ++i)
		RemoveKeyFrameProperties(pItemData, frameIndexList[i], false);

	if(bRefreshGfxItems)
		RefreshAllGfxItems();
}

void EntityDopeSheetScene::PushAllKeyFrames(EntityTreeItemData *pItemData, bool bRefreshGfxItems)
{
	if(m_PoppedKeyFramesMap.contains(pItemData) == false)
		return;
	
	for(auto iter = m_PoppedKeyFramesMap[pItemData].begin(); iter != m_PoppedKeyFramesMap[pItemData].end(); ++iter)
		SetKeyFrameProperties(pItemData, iter.key(), iter.value());

	m_PoppedKeyFramesMap.remove(pItemData);

	if(bRefreshGfxItems)
		RefreshAllGfxItems();
}

QJsonArray EntityDopeSheetScene::SerializeCallbacks() const
{
	QJsonArray callbackArray;
	for(auto iter = m_CallbackMap.begin(); iter != m_CallbackMap.end(); ++iter)
	{
		QJsonObject callbackObj;
		callbackObj.insert("frame", iter.key());

		QJsonArray functionsArray;
		for(int i = 0; i < iter.value().size(); ++i)
			functionsArray.append(iter.value()[i]);
		callbackObj.insert("functions", functionsArray);
		callbackArray.append(callbackObj);
	}
	return callbackArray;
}

bool EntityDopeSheetScene::SetCallback(int iFrameIndex, QString sCallback)
{
	if(m_CallbackMap.contains(iFrameIndex))
	{
		if(m_CallbackMap[iFrameIndex].contains(sCallback))
		{
			HyGuiLog("EntityDopeSheetScene::SetCallback() - '" % sCallback % "' already exists for frame index: " % QString::number(iFrameIndex), LOGTYPE_Error);
			return false;
		}
	}
	else
		m_CallbackMap.insert(iFrameIndex, QStringList());

	m_CallbackMap[iFrameIndex].append(sCallback);
	update();

	return true;
}

bool EntityDopeSheetScene::RemoveCallback(int iFrameIndex, QString sCallback)
{
	if(m_CallbackMap.contains(iFrameIndex) && m_CallbackMap[iFrameIndex].contains(sCallback))
	{
		m_CallbackMap[iFrameIndex].removeOne(sCallback);
		update();
		
		return true;
	}
	
	HyGuiLog("EntityDopeSheetScene::RemoveCallback() - No '" % sCallback % "' found for frame index: " % QString::number(iFrameIndex), LOGTYPE_Error);
	return false;
}

QStringList EntityDopeSheetScene::GetCallbackList(int iFrameIndex) const
{
	if(m_CallbackMap.contains(iFrameIndex))
		return m_CallbackMap[iFrameIndex];
	
	return QStringList();
}

bool EntityDopeSheetScene::RenameCallback(int iFrameIndex, QString sOldCallback, QString sNewCallback)
{
	if(m_CallbackMap.contains(iFrameIndex))
	{
		int iCallbackIndex = m_CallbackMap[iFrameIndex].indexOf(sOldCallback);
		if(iCallbackIndex >= 0)
		{
			m_CallbackMap[iFrameIndex][iCallbackIndex] = sNewCallback;
			return true;
		}
	}
	
	HyGuiLog("EntityDopeSheetScene::RenameCallback() - No callback '" % sOldCallback % "' found for frame index: " % QString::number(iFrameIndex), LOGTYPE_Error);
	return false;
}

QMap<int, QList<TimelineEvent>> EntityDopeSheetScene::AssembleTimelineEvents(EntityTreeItemData *pItemData) const
{
	if(m_KeyFramesMap.contains(pItemData) == false)
		return QMap<int, QList<TimelineEvent>>();

	const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
	return HyGlobal::AssembleTimelineEvents(itemKeyFrameMapRef);
}

void EntityDopeSheetScene::NudgeKeyFrameProperty(EntityTreeItemData *pItemData, int iFrameIndex, QString sCategoryName, QString sPropName, int iNudgeAmount, bool bRefreshGfxItems)
{
	QJsonValue propValue = GetKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, sPropName);
	RemoveKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, sPropName, false);
	SetKeyFrameProperty(pItemData, HyMath::Max(0, iFrameIndex + iNudgeAmount), sCategoryName, sPropName, propValue, bRefreshGfxItems);
}

void EntityDopeSheetScene::NudgeKeyFrameTween(EntityTreeItemData *pItemData, int iFrameIndex, TweenProperty eTweenProp, int iNudgeAmount, bool bRefreshGfxItems)
{
	QString sCategoryName = "Tween " % HyGlobal::TweenPropName(eTweenProp);

	NudgeKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Destination", iNudgeAmount, bRefreshGfxItems);
	NudgeKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Duration", iNudgeAmount, bRefreshGfxItems);
	NudgeKeyFrameProperty(pItemData, iFrameIndex, sCategoryName, "Tween Type", iNudgeAmount, bRefreshGfxItems);
}

void EntityDopeSheetScene::SelectKeyFrames(bool bAppendSelection, EntityTreeItemData *pItemData, int iSelectionPivotFrame, bool bPivotLessThan)
{
	if(bAppendSelection == false)
		clearSelection();

	if(pItemData == nullptr)
	{
		for(auto iter = m_KeyFramesGfxRectMap.begin(); iter != m_KeyFramesGfxRectMap.end(); ++iter)
		{
			int iFrameIndex = std::get<GFXDATAKEY_FrameIndex>(iter.key());
			if(iSelectionPivotFrame < 0 ||
				iSelectionPivotFrame == iFrameIndex ||
				(bPivotLessThan && iFrameIndex < iSelectionPivotFrame) ||
				(bPivotLessThan == false && iFrameIndex > iSelectionPivotFrame))
			{
				iter.value()->setSelected(true);
			}
		}
		for(auto iter = m_TweenGfxRectMap.begin(); iter != m_TweenGfxRectMap.end(); ++iter)
		{
			int iFrameIndex = std::get<GFXDATAKEY_FrameIndex>(iter.key());
			if(iSelectionPivotFrame < 0 ||
				iSelectionPivotFrame == iFrameIndex ||
				(bPivotLessThan && iFrameIndex < iSelectionPivotFrame) ||
				(bPivotLessThan == false && iFrameIndex > iSelectionPivotFrame))
			{
				iter.value()->setSelected(true);
			}
		}
	}
	else // pItemData != nullptr
	{
		const QMap<int, QJsonObject> &itemKeyFrameMapRef = m_KeyFramesMap[pItemData];
		for(QMap<int, QJsonObject>::const_iterator iter = itemKeyFrameMapRef.begin(); iter != itemKeyFrameMapRef.end(); ++iter)
		{
			int iFrameIndex = iter.key();
			
			if(iSelectionPivotFrame < 0 ||
				iSelectionPivotFrame == iFrameIndex ||
				(bPivotLessThan && iFrameIndex < iSelectionPivotFrame) ||
				(bPivotLessThan == false && iFrameIndex > iSelectionPivotFrame))
			{
				QJsonObject propsObj = iter.value();
				QStringList sCategoryList = propsObj.keys();
				for(QString sCategoryName : sCategoryList)
				{
					QJsonObject categoryObj = propsObj[sCategoryName].toObject();
					QStringList sPropList = categoryObj.keys();
					for(QString sPropName : sPropList)
					{
						if(sCategoryName.startsWith("Tween "))
						{
							QString sTweenName = sCategoryName.mid(6);
							TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(sTweenName);
							QPair<QString, QString> tweenPair = HyGlobal::ConvertTweenPropToRegularPropPair(eTweenProp);
							KeyFrameKey gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, tweenPair.first % "/" % tweenPair.second);
							if(m_TweenGfxRectMap.contains(gfxRectMapKey))
								m_TweenGfxRectMap[gfxRectMapKey]->setSelected(true);
						}
						else
						{
							KeyFrameKey gfxRectMapKey = std::make_tuple(pItemData, iFrameIndex, sCategoryName % "/" % sPropName);
							if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey))
								m_KeyFramesGfxRectMap[gfxRectMapKey]->setSelected(true);
						}
					}
				}
			}
		}
	}
}

QList<EntityTreeItemData *> EntityDopeSheetScene::GetItemsFromSelectedFrames() const
{
	QSet<EntityTreeItemData *> itemSet;
	QList<QGraphicsItem *> gfxItemList = selectedItems();
	for(QGraphicsItem *pGfxItem : gfxItemList)
	{
		bool bAcquiredDataType = false;
		DopeSheetGfxItemType eItemType = static_cast<DopeSheetGfxItemType>(pGfxItem->data(GFXDATAKEY_Type).toInt(&bAcquiredDataType));
		if(bAcquiredDataType == false || (eItemType != GFXITEM_PropertyKeyFrame && eItemType != GFXITEM_TweenKeyFrame))
			continue;

		KeyFrameKey tupleKey = static_cast<GraphicsKeyFrameItem *>(pGfxItem)->GetKey();
		itemSet.insert(std::get<GFXDATAKEY_TreeItemData>(tupleKey));
	}
	
	return itemSet.values();
}

void EntityDopeSheetScene::RefreshAllGfxItems()
{
	// Gather all the entity items (root, children, shapes) into one list 'itemList'
	QList<EntityTreeItemData *> entireItemList, shapeList;
	static_cast<EntityModel &>(m_pEntStateData->GetModel()).GetTreeModel().GetTreeItemData(entireItemList, shapeList);
	entireItemList += shapeList;
	entireItemList.prepend(static_cast<EntityModel &>(m_pEntStateData->GetModel()).GetTreeModel().GetRootTreeItemData());

	m_iFinalFrame = 0; // Determine the final frame

	// Process all the items and draw their key frames
	qreal fPosY = TIMELINE_HEIGHT + 2.0f;
	for(EntityTreeItemData *pCurItemData : entireItemList)
	{
		// 'uniquePropList' will contain every row of key frames for this item, across all frames (tweens are collapsed to their regular category/property)
		QList<QPair<QString, QString>> uniquePropList = GetUniquePropertiesList(pCurItemData, true);

		// - If NOT selected, draw all the key frames in one row (its name row)
		// - If selected, skip the name row and then draw each property in its own row
		if(pCurItemData->IsSelected())
			fPosY += ITEMS_LINE_HEIGHT; // skip the name row

		if(m_KeyFramesMap.contains(pCurItemData))
		{
			QList<TweenProperty> tweenPropList = HyGlobal::GetTweenPropList();
			const QMap<int, QJsonObject> &keyFrameMapRef = m_KeyFramesMap[pCurItemData];
			for(int iFrameIndex : keyFrameMapRef.keys())
			{
				if(m_iFinalFrame < iFrameIndex)
					m_iFinalFrame = iFrameIndex;

				const float fITEM_START_POSY = fPosY;
				QJsonObject propsObj = keyFrameMapRef[iFrameIndex];

				// Iterate through 'uniquePropList' and draw a QGraphicsRectItem for each property found in 'propsObj'
				for(auto &propPair : uniquePropList)
				{
					bool bPropKeyFrame = propsObj.contains(propPair.first) && propsObj[propPair.first].toObject().contains(propPair.second);
					
					bool bTweenKeyFrame = std::any_of(tweenPropList.begin(), tweenPropList.end(), [&](TweenProperty eTweenProp) {
						QPair<QString, QString> tweenPair = HyGlobal::ConvertTweenPropToRegularPropPair(eTweenProp);
						return (propPair == tweenPair && propsObj.contains("Tween " % HyGlobal::TweenPropName(eTweenProp)));
					});

					if(bPropKeyFrame == false && bTweenKeyFrame == false)
					{
						if(pCurItemData->IsSelected())
							fPosY += ITEMS_LINE_HEIGHT;
						continue;
					}

					// IMPORTANT NOTE: std::make_tuple() crashes in Release if you try to construct the string directly in the function call
					QString sPropString = propPair.first % "/" % propPair.second;
					KeyFrameKey gfxRectMapKey = std::make_tuple(pCurItemData, iFrameIndex, sPropString);

					qreal fPosX = TIMELINE_LEFT_MARGIN + (iFrameIndex * TIMELINE_NOTCH_SUBLINES_WIDTH) - 2.0f;
					
					// Add or update the GraphicsKeyFrameItem's
					if(bPropKeyFrame)
					{
						if(m_KeyFramesGfxRectMap.contains(gfxRectMapKey) == false)
						{
							GraphicsKeyFrameItem *pNewGfxRectItem = new GraphicsKeyFrameItem(gfxRectMapKey, false);
							pNewGfxRectItem->setPos(fPosX, fPosY);

							m_KeyFramesGfxRectMap[gfxRectMapKey] = pNewGfxRectItem;
							addItem(pNewGfxRectItem);
						}
						else
							m_KeyFramesGfxRectMap[gfxRectMapKey]->setPos(fPosX, fPosY);
					}
					if(bTweenKeyFrame)
					{
						if(m_TweenGfxRectMap.contains(gfxRectMapKey) == false)
						{
							fPosX += (bPropKeyFrame ? (KEYFRAME_WIDTH + 1.0f) : 0.0f);
							GraphicsKeyFrameItem *pNewGfxRectItem = new GraphicsKeyFrameItem(gfxRectMapKey, true);
							pNewGfxRectItem->setPos(fPosX, fPosY);

							m_TweenGfxRectMap[gfxRectMapKey] = pNewGfxRectItem;
							addItem(pNewGfxRectItem);
						}
						else
							m_TweenGfxRectMap[gfxRectMapKey]->setPos(fPosX + (bPropKeyFrame ? (KEYFRAME_WIDTH + 1.0f) : 0.0f), fPosY);

						// Calculate the dash-line "Duration"
						double dDuration = GetKeyFrameProperty(pCurItemData, iFrameIndex, "Tween " % propPair.second, "Duration").toDouble();
						int iNumFrames = (dDuration * static_cast<EntityModel &>(m_pEntStateData->GetModel()).GetFramesPerSecond());
						qreal fLineLength = iNumFrames * TIMELINE_NOTCH_SUBLINES_WIDTH;
						if(bPropKeyFrame)
							fLineLength -= (KEYFRAME_WIDTH + 1.0f);

						m_TweenGfxRectMap[gfxRectMapKey]->SetTweenLineLength(fLineLength, iNumFrames);

						if(m_iFinalFrame < (iFrameIndex + iNumFrames))
							m_iFinalFrame = (iFrameIndex + iNumFrames);
					}

					if(pCurItemData->IsSelected())
						fPosY += ITEMS_LINE_HEIGHT;
				}

				fPosY = fITEM_START_POSY;
			}
		}

		if(pCurItemData->IsSelected() == false)
			fPosY += ITEMS_LINE_HEIGHT; // Move past the name row
		else
			fPosY += uniquePropList.size() * ITEMS_LINE_HEIGHT; // Already accounted for the name row, now move past the property rows

		fPosY += 1.0f; // Account for the space between items
	} // for each 'EntityTreeItemData'

	// Also check events to ensure the final frame is accurate
	for(int iCallbackFrameIndex : m_CallbackMap.keys())
	{
		if(m_iFinalFrame < iCallbackFrameIndex)
			m_iFinalFrame = iCallbackFrameIndex;
	}

	update();
}

/*virtual*/ void EntityDopeSheetScene::mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent) /*override*/
{
	if(pMouseEvent->buttons() & Qt::RightButton)
		pMouseEvent->accept(); // Eat right click to avoid deselecting items
	else
		QGraphicsScene::mousePressEvent(pMouseEvent);
}
